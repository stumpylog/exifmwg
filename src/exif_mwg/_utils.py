# SPDX-FileCopyrightText: 2025-present Trenton H <rda0128ou@mozmail.com>
#
# SPDX-License-Identifier: MPL-2.0

import logging
from collections import defaultdict
from datetime import datetime
from datetime import timezone
from typing import TYPE_CHECKING
from typing import Callable

from exif_mwg.models import ImageMetadata
from exif_mwg.models import KeywordInfoModel
from exif_mwg.models import KeywordStruct


def now_string() -> str:
    """
    Formats the current time how exiftool likes to see it
    """
    return datetime.now(tz=timezone.utc).strftime("%Y:%m:%d %H:%M:%S.%fZ")


def process_separated_list(parent: KeywordStruct, remaining: list[str]) -> None:
    """
    Given a list of strings, build a tree structure from them, rooted at the given parent

    Example:
        Root|Child|ChildChild
        Root|OtherChild

        becomes

        Root -->
          Child -->
            ChildChild
          OtherChild
    """
    if not remaining:
        # TODO(trenton): Should this set Applied?  Leaf nodes are assumed to be applied
        return
    new_parent = KeywordStruct(Keyword=remaining[0])

    parent.Children.append(new_parent)
    process_separated_list(new_parent, remaining[1:])


def remove_duplicate_children(root: KeywordStruct) -> None:
    """
    Removes duplicated children, which may exist as multiple fields above contain the same data
    """
    if not root.Children:
        return
    for child in root.Children:
        remove_duplicate_children(child)
    root.Children = list(set(root.Children))


def combine_same_keywords(root: KeywordStruct) -> KeywordStruct:
    if not root.Children:
        return root

    # Group children by their "Keyword"
    groups: dict[str, list[KeywordStruct]] = defaultdict(list)
    for child in root.Children:
        groups[child.Keyword].append(child)

    merged_children: list[KeywordStruct] = []
    for keyword, nodes in groups.items():
        if len(nodes) == 1:
            merged_children.append(combine_same_keywords(nodes[0]))
        else:
            # Merge children of nodes with the same keyword
            merged_node = KeywordStruct(Keyword=keyword)
            all_children = []
            for node in nodes:
                if node.Children:
                    all_children.extend(node.Children)

            if all_children:
                merged_node.Children = all_children
                # Recursively merge the children of the merged node
                merged_children.append(combine_same_keywords(merged_node))
            else:
                merged_children.append(merged_node)

    root.Children = merged_children
    return root


def combine_keyword_structures(metadata: ImageMetadata) -> ImageMetadata:
    """
    Reads the various other possible keyword values, and generates a tree from them,
    then combines with anything existing and removes duplicates
    """
    keywords: list[KeywordStruct] = []

    if metadata.KeywordInfo and metadata.KeywordInfo.Hierarchy:
        keywords.extend(metadata.KeywordInfo.Hierarchy)

    # Check for other keywords which might get set as a flat structure
    # Parse them into KeywordStruct trees
    roots: dict[str, KeywordStruct] = {}
    for key, separation in [
        (metadata.HierarchicalSubject, "|"),
        (metadata.CatalogSets, "|"),
        (metadata.TagsList, "/"),
        (metadata.LastKeywordXMP, "/"),
    ]:
        if not key:
            continue
        for line in key:
            if TYPE_CHECKING:
                assert isinstance(line, str)
            values_list = line.split(separation)
            root_value = values_list[0]
            if root_value not in roots:
                roots[root_value] = KeywordStruct(Keyword=values_list[0])
            root = roots[root_value]
            process_separated_list(root, values_list[1:])

    keywords.extend(list(roots.values()))

    for keyword in keywords:
        remove_duplicate_children(keyword)
        combine_same_keywords(keyword)

    # Assign the parsed flat keywords in as well
    if not keywords:
        return metadata
    if not metadata.KeywordInfo:
        metadata.KeywordInfo = KeywordInfoModel(Hierarchy=keywords)
    else:
        metadata.KeywordInfo.Hierarchy = keywords
    return metadata


def expand_keyword_structures(metadata: ImageMetadata, logging_func: Callable[[int, str], None]) -> ImageMetadata:
    """
    Expands the KeywordInfo.Hierarchy to also set the HierarchicalSubject, CatalogSets, TagsList and LastKeywordXMP
    """
    if any([metadata.HierarchicalSubject, metadata.CatalogSets, metadata.TagsList, metadata.LastKeywordXMP]):
        logging_func(logging.WARNING, f"{metadata.SourceFile.name}: One of the flat tags is set, but will be cleared")

    if not metadata.KeywordInfo:
        return metadata

    for keyword in metadata.KeywordInfo.Hierarchy:
        remove_duplicate_children(keyword)

    def flatten_children(internal_root: KeywordStruct, current_words: list[str]) -> None:
        if not internal_root.Children:
            current_words.append(internal_root.Keyword)
        this_child_words = [internal_root.Keyword]
        for internal_child in internal_root.Children:
            flatten_children(internal_child, this_child_words)
            current_words.extend(this_child_words)
            this_child_words = [internal_root.Keyword]

    list_of_lists: list[list[str]] = []

    for root in metadata.KeywordInfo.Hierarchy:
        current_child_words = [root.Keyword]
        for child in root.Children:
            flatten_children(child, current_child_words)
            list_of_lists.append(current_child_words)
            current_child_words = [root.Keyword]

    # Directly overwrite everything
    metadata.HierarchicalSubject = ["|".join(x) for x in list_of_lists]
    metadata.CatalogSets = ["|".join(x) for x in list_of_lists]

    metadata.TagsList = ["/".join(x) for x in list_of_lists]
    metadata.LastKeywordXMP = ["/".join(x) for x in list_of_lists]

    return metadata
