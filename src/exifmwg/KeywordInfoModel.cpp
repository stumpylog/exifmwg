#include <algorithm>
#include <utility>

#include "KeywordInfoModel.hpp"
#include "Logging.hpp"

#include "XmpUtils.hpp"

// KeywordStruct Implementation
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

  return {keywordValue, children, appliedValue};
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
    for (const std::string& token : pathTokens) {
      KeywordStruct* node = findOrCreateChild(*currentLevel, token);
      currentLevel = &(node->Children);
    }
  }
  Hierarchy = std::move(rootNodes);
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

  return {hierarchy};
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
}

static KeywordStruct* findOrCreateChild(std::vector<KeywordStruct>& children, const std::string& keyword) {
  // Use std::find_if to search for the keyword
  auto it = std::find_if(children.begin(), children.end(),
                         [&](const KeywordStruct& child) { return child.Keyword == keyword; });

  // If found, return a pointer to the existing child
  if (it != children.end()) {
    return &(*it); // Dereference iterator to get reference, then take address
  }

  // If not found, create a new child using emplace_back
  // This constructs the KeywordStruct directly in the vector
  children.emplace_back(keyword, std::vector<KeywordStruct>{}, std::nullopt);

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
