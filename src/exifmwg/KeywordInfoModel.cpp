#include <algorithm>
#include <utility>

#include "KeywordInfoModel.hpp"
#include "Logging.hpp"
#include "MetadataKeys.hpp"

#include "XmpUtils.hpp"

KeywordInfoModel::KeywordStruct::KeywordStruct(std::string keyword, const std::vector<KeywordStruct>& children,
                                               std::optional<bool> applied) :
    Keyword(std::move(keyword)), Applied(applied), Children(children) {
}

KeywordInfoModel::KeywordStruct KeywordInfoModel::KeywordStruct::fromXmp(const Exiv2::XmpData& xmpData,
                                                                         const std::string& basePath) {
  std::string keywordValue;
  std::optional<bool> appliedValue;
  std::vector<KeywordStruct> children;

  // Get keyword value
  std::string keywordKey = basePath + "/mwg-kw:Keyword";
  auto keywordIt = xmpData.findKey(Exiv2::XmpKey(keywordKey));
  if (keywordIt != xmpData.end()) {
    keywordValue = keywordIt->toString();
  } else {
    throw std::runtime_error("mwg-kw:Keyword key not found");
  }

  // Get Applied attribute
  std::string appliedKey = basePath + "/mwg-kw:Applied";
  auto appliedIt = xmpData.findKey(Exiv2::XmpKey(appliedKey));
  if (appliedIt != xmpData.end()) {
    std::string appliedStringValue = appliedIt->toString();
    appliedValue = (appliedStringValue == "True" || appliedStringValue == "true" || appliedStringValue == "1");
  }

  // Parse children recursively
  std::string childrenBasePath = basePath + "/mwg-kw:Children";
  int childIndex = 1;
  while (true) {
    std::string childPath = childrenBasePath + "[" + std::to_string(childIndex) + "]";
    std::string childKeywordKey = childPath + "/mwg-kw:Keyword";

    auto childKeywordIt = xmpData.findKey(Exiv2::XmpKey(childKeywordKey));
    if (childKeywordIt == xmpData.end()) {
      break;
    }

    KeywordStruct child = KeywordStruct::fromXmp(xmpData, childPath);
    children.push_back(child);
    childIndex++;
  }

  return KeywordInfoModel::KeywordStruct(keywordValue, children, appliedValue);
}

void KeywordInfoModel::KeywordStruct::toXmp(Exiv2::XmpData& xmpData, const std::string& basePath) const {
  xmpData[basePath + "/mwg-kw:Keyword"] = Keyword;

  if (Applied) {
    xmpData[basePath + "/mwg-kw:Applied"] = *Applied ? "True" : "False";
  }

  writeChildrenToXmp(xmpData, basePath);
}

void KeywordInfoModel::KeywordStruct::writeChildrenToXmp(Exiv2::XmpData& xmpData, const std::string& basePath) const {
  if (!Children.empty()) {
    xmpData[basePath + "/mwg-kw:Children"] = "";
    for (size_t i = 0; i < Children.size(); ++i) {
      std::string childPath = basePath + "/mwg-kw:Children[" + std::to_string(i + 1) + "]";
      Children[i].toXmp(xmpData, childPath);
    }
  }
}

std::string KeywordInfoModel::KeywordStruct::to_string() const {
  std::string repr = "KeywordStruct(Keyword='" + Keyword + "'";

  if (Applied.has_value()) {
    repr += ", Applied=";
    repr += (Applied.value() ? "True" : "False");
  }

  if (!Children.empty()) {
    repr += ", Children=[";
    for (size_t i = 0; i < Children.size(); ++i) {
      if (i > 0) {
        repr += ", ";
      }
      repr += Children[i].to_string();
    }
    repr += "]";
  }

  repr += ")";
  return repr;
}

// KeywordInfoModel Implementation
KeywordInfoModel::KeywordInfoModel(const std::vector<KeywordStruct>& hierarchy) : Hierarchy(hierarchy) {
}

KeywordInfoModel::KeywordInfoModel(const std::vector<std::string>& delimitedStrings, char delimiter) {
  std::vector<KeywordStruct> rootNodes;
  for (const std::string& delimitedString : delimitedStrings) {
    std::vector<std::string> pathTokens = XmpUtils::splitString(delimitedString, delimiter);
    if (pathTokens.empty()) {
      continue;
    }
    // Start at root level
    std::vector<KeywordStruct>* currentLevel = &rootNodes;
    KeywordStruct* node = nullptr;
    for (const std::string& token : pathTokens) {
      node = KeywordInfoModel::findOrCreateChild(*currentLevel, token);
      currentLevel = &(node->Children);
    }

    if (node) {
      node->Applied = true;
    }
  }
  Hierarchy = std::move(rootNodes);
  sortKeywordVector(Hierarchy);
}

KeywordInfoModel KeywordInfoModel::fromXmp(const Exiv2::XmpData& xmpData) {
  std::vector<KeywordStruct> hierarchy;
  std::string basePath = "Xmp.mwg-kw.Keywords/mwg-kw:Hierarchy";
  int index = 1;

  while (true) {
    std::string itemPath = basePath + "[" + std::to_string(index) + "]";
    std::string keywordKey = itemPath + "/mwg-kw:Keyword";

    auto keywordIt = xmpData.findKey(Exiv2::XmpKey(keywordKey));
    if (keywordIt == xmpData.end()) {
      break;
    }

    KeywordStruct keywordStruct = KeywordStruct::fromXmp(xmpData, itemPath);
    hierarchy.push_back(keywordStruct);
    index++;
  }

  // Check for digiKam tags
  auto digiKamIt = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::DigiKamTagsList));
  if (digiKamIt != xmpData.end()) {
    auto parsed = parseDelimitedPaths(digiKamIt->toString(), '/', ',');
    hierarchy = mergeKeywordVectors(hierarchy, parsed);
  }

  // Check for Lightroom hierarchical
  auto lrIt = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::LightroomHierarchicalSubject));
  if (lrIt != xmpData.end()) {
    auto parsed = parseDelimitedPaths(lrIt->toString(), '|', ',');
    hierarchy = mergeKeywordVectors(hierarchy, parsed);
  }

  // Check for Microsoft keywords
  auto msIt = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::MicrosoftLastKeywordXMP));
  if (msIt != xmpData.end()) {
    auto parsed = parseDelimitedPaths(msIt->toString(), '/', ',');
    hierarchy = mergeKeywordVectors(hierarchy, parsed);
  }

  // Check for Iview MediaPro Catalog Sets
  auto mpcsIt = xmpData.findKey(Exiv2::XmpKey(MetadataKeys::Xmp::MediaProCatalogSets));
  if (mpcsIt != xmpData.end()) {
    auto parsed = parseDelimitedPaths(mpcsIt->toString(), '|', ',');
    hierarchy = mergeKeywordVectors(hierarchy, parsed);
  }

  // Check for ACDSee categories
  // TODO

  sortKeywordVector(hierarchy);

  return KeywordInfoModel(hierarchy);
}

void KeywordInfoModel::toXmp(Exiv2::XmpData& xmpData) const {
  InternalLogger::debug("Writing MWG Keywords hierarchy");

  XmpUtils::clearXmpKey(xmpData, "Xmp.mwg-kw.Keywords");

  if (Hierarchy.empty()) {
    return;
  }

  const std::string basePath = "Xmp.mwg-kw.Keywords/mwg-kw:Hierarchy";
  xmpData[basePath] = "";

  for (size_t i = 0; i < Hierarchy.size(); ++i) {
    std::string itemPath = basePath + "[" + std::to_string(i + 1) + "]";
    Hierarchy[i].toXmp(xmpData, itemPath);
  }

  InternalLogger::debug("Wrote " + std::to_string(Hierarchy.size()) + " top-level keyword hierarchy items");

  // Write DigiKam tags
  std::string digiKamTags = buildDelimitedPaths('/');
  if (!digiKamTags.empty()) {
    XmpUtils::clearXmpKey(xmpData, MetadataKeys::Xmp::DigiKamTagsList);
    xmpData[MetadataKeys::Xmp::DigiKamTagsList] = digiKamTags;
  }

  // Write Lightroom hierarchical
  std::string lrHierarchical = buildDelimitedPaths('|');
  if (!lrHierarchical.empty()) {
    XmpUtils::clearXmpKey(xmpData, MetadataKeys::Xmp::LightroomHierarchicalSubject);
    xmpData[MetadataKeys::Xmp::LightroomHierarchicalSubject] = lrHierarchical;
  }

  // Write Microsoft keywords (same format as DigiKam)
  if (!digiKamTags.empty()) {
    XmpUtils::clearXmpKey(xmpData, MetadataKeys::Xmp::MicrosoftLastKeywordXMP);
    xmpData[MetadataKeys::Xmp::MicrosoftLastKeywordXMP] = digiKamTags;
  }

  // Write MediaPro keywords (same format as Lightroom)
  if (!lrHierarchical.empty()) {
    XmpUtils::clearXmpKey(xmpData, MetadataKeys::Xmp::MediaProCatalogSets);
    xmpData[MetadataKeys::Xmp::MediaProCatalogSets] = lrHierarchical;
  }

  // Write ACDSee categories
  // TODO
}

KeywordInfoModel::KeywordStruct*
KeywordInfoModel::findOrCreateChild(std::vector<KeywordInfoModel::KeywordStruct>& children,
                                    const std::string& keyword) {
  // Use std::find_if to search for the keyword
  auto it = std::find_if(children.begin(), children.end(),
                         [&](const KeywordInfoModel::KeywordStruct& child) { return child.Keyword == keyword; });

  // If found, return a pointer to the existing child
  if (it != children.end()) {
    return &(*it); // Dereference iterator to get reference, then take address
  }

  // If not found, create a new child using emplace_back
  // This constructs the KeywordStruct directly in the vector
  children.emplace_back(keyword, std::vector<KeywordInfoModel::KeywordStruct>{}, std::nullopt);

  // Return a pointer to the newly created child
  return &children.back();
}

std::optional<bool> KeywordInfoModel::mergeApplied(const std::optional<bool>& a, const std::optional<bool>& b) {
  if (!a.has_value()) {
    return b;
  }
  if (!b.has_value()) {
    return a;
  }
  return a.value() || b.value();
}

std::vector<KeywordInfoModel::KeywordStruct>
KeywordInfoModel::mergeKeywordVectors(const std::vector<KeywordStruct>& vec1, const std::vector<KeywordStruct>& vec2) {

  std::vector<KeywordStruct> result;

  // Add all keywords from vec1
  for (const auto& keyword1 : vec1) {
    // Look for matching keyword in vec2
    auto it = std::find_if(vec2.begin(), vec2.end(),
                           [&keyword1](const KeywordStruct& k) { return k.Keyword == keyword1.Keyword; });

    if (it != vec2.end()) {
      // Found matching keyword - merge them
      std::vector<KeywordStruct> mergedChildren = mergeKeywordVectors(keyword1.Children, it->Children);

      result.emplace_back(keyword1.Keyword, mergedChildren, mergeApplied(keyword1.Applied, it->Applied));
    } else {
      // No match found - add keyword1 as-is
      result.push_back(keyword1);
    }
  }

  // Add keywords from vec2 that weren't already processed
  for (const auto& keyword2 : vec2) {
    auto it = std::find_if(vec1.begin(), vec1.end(),
                           [&keyword2](const KeywordStruct& k) { return k.Keyword == keyword2.Keyword; });

    if (it == vec1.end()) {
      // keyword2 not found in vec1 - add it
      result.push_back(keyword2);
    }
  }

  return result;
}

std::string KeywordInfoModel::to_string() const {
  std::string repr = "KeywordInfoModel(Hierarchy=[";

  for (size_t i = 0; i < Hierarchy.size(); ++i) {
    if (i > 0) {
      repr += ", ";
    }
    repr += Hierarchy[i].to_string();
  }

  repr += "])";
  return repr;
}

KeywordInfoModel& KeywordInfoModel::operator|=(const KeywordInfoModel& other) {
  this->Hierarchy = mergeKeywordVectors(this->Hierarchy, other.Hierarchy);
  return *this;
}

KeywordInfoModel KeywordInfoModel::operator|(const KeywordInfoModel& other) const {
  KeywordInfoModel result = *this;
  result |= other;
  return result;
}

// Helper implementations
std::vector<KeywordInfoModel::KeywordStruct> KeywordInfoModel::parseDelimitedPaths(const std::string& data,
                                                                                   char pathDelim, char listDelim) {
  std::vector<KeywordStruct> result;

  std::vector<std::string> items = XmpUtils::splitString(data, listDelim);

  for (const auto& item : items) {
    std::string trimmed = XmpUtils::trimWhitespace(item);
    if (!trimmed.empty()) {
      KeywordStruct keyword = parseHierarchicalPath(trimmed, pathDelim);
      mergeKeywordIntoHierarchy(result, keyword);
    }
  }
  return result;
}

KeywordInfoModel::KeywordStruct KeywordInfoModel::parseHierarchicalPath(const std::string& path, char delimiter,
                                                                        bool leafApplied) {
  if (path.empty()) {
    return KeywordStruct(""); // Handle empty input gracefully
  }

  std::vector<std::string> pathTokens = XmpUtils::splitString(path, delimiter);
  if (pathTokens.empty()) {
    return KeywordStruct("");
  }

  KeywordStruct result(XmpUtils::trimWhitespace(pathTokens[0]));
  KeywordStruct* current = &result;
  for (size_t i = 1; i < pathTokens.size(); ++i) {
    current->Children.emplace_back(XmpUtils::trimWhitespace(pathTokens[i]));
    current = &current->Children.back();
  }
  if (leafApplied) {
    current->Applied = true;
  }
  return result;
}

void KeywordInfoModel::mergeKeywordIntoHierarchy(std::vector<KeywordStruct>& hierarchy, const KeywordStruct& keyword) {
  auto it = std::find_if(hierarchy.begin(), hierarchy.end(),
                         [&keyword](const KeywordStruct& k) { return k.Keyword == keyword.Keyword; });
  if (it != hierarchy.end()) {
    for (const auto& child : keyword.Children) {
      mergeKeywordIntoHierarchy(it->Children, child);
    }
    it->Applied = mergeApplied(it->Applied, keyword.Applied);
  } else {
    hierarchy.push_back(keyword);
  }
}

std::string KeywordInfoModel::buildDelimitedPaths(char delimiter) const {
  std::vector<std::string> paths;
  for (const auto& keyword : Hierarchy) {
    writeHierarchicalPaths(paths, keyword, "", delimiter);
  }
  return XmpUtils::joinStrings(paths, ',');
}

void KeywordInfoModel::writeHierarchicalPaths(std::vector<std::string>& paths, const KeywordStruct& keyword,
                                              const std::string& currentPath, char delimiter) const {
  const std::string newPath = currentPath.empty() ? keyword.Keyword : currentPath + delimiter + keyword.Keyword;
  if (keyword.Applied.value_or(false) || keyword.Children.empty()) {
    paths.push_back(newPath);
  }
  for (const auto& child : keyword.Children) {
    writeHierarchicalPaths(paths, child, newPath, delimiter);
  }
}

void KeywordInfoModel::sortKeywordVector(std::vector<KeywordStruct>& keywords) {
  std::sort(keywords.begin(), keywords.end());
  for (auto& keyword : keywords) {
    sortKeywordVector(keyword.Children);
  }
}
