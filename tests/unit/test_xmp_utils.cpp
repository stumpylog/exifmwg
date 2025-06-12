#include <catch2/catch_test_macros.hpp>

#include <exiv2/exiv2.hpp>

#include "XmpUtils.hpp"

TEST_CASE("trim-whitespace", "xmp-utils") {
  SECTION("Removing basic string whitespace") {
    REQUIRE(XmpUtils::trimWhitespace("  test string  ") == "test string");
  }
  SECTION("Trimming a string with no whitespace") {
    REQUIRE(XmpUtils::trimWhitespace("string") == "string");
  }
  SECTION("Trimming a string with all whitespace") {
    REQUIRE(XmpUtils::trimWhitespace("\t  \n") == "");
  }
  SECTION("Trimming a string with only leading whitespace") {
    REQUIRE(XmpUtils::trimWhitespace("\tThis is a test") == "This is a test");
  }
  SECTION("Trimming a string with only trailing") {
    REQUIRE(XmpUtils::trimWhitespace("super neat stuff\n") == "super neat stuff");
  }
  SECTION("Trimming an empty string") {
    REQUIRE(XmpUtils::trimWhitespace("") == "");
  }
}

TEST_CASE("clearXmpKey", "xmp-utils") {
  Exiv2::XmpData xmpData;
  SECTION("Clearing existing keys") {
    xmpData["Xmp.dc.creator"] = "John Doe";
    xmpData["Xmp.dc.title"] = "My Title";
    xmpData["Xmp.photoshop.AuthorsPosition"] = "Developer";

    XmpUtils::clearXmpKey(xmpData, "dc");
    REQUIRE(xmpData.findKey(Exiv2::XmpKey("Xmp.dc.creator")) == xmpData.end());
    REQUIRE(xmpData.findKey(Exiv2::XmpKey("Xmp.dc.title")) == xmpData.end());
    REQUIRE(xmpData.findKey(Exiv2::XmpKey("Xmp.photoshop.AuthorsPosition")) != xmpData.end());
  }

  SECTION("Clearing non-existent key") {
    xmpData["Xmp.dc.creator"] = "John Doe";
    XmpUtils::clearXmpKey(xmpData, "nonExistentKey");
    REQUIRE(xmpData.findKey(Exiv2::XmpKey("Xmp.dc.creator")) != xmpData.end());
  }

  SECTION("Clearing from empty XmpData") {
    XmpUtils::clearXmpKey(xmpData, "someKey");
    REQUIRE(xmpData.empty());
  }

  SECTION("Clearing all keys") {
    xmpData["Xmp.dc.creator"] = "John Doe";
    xmpData["Xmp.dc.title"] = "My Title";
    XmpUtils::clearXmpKey(xmpData, "Xmp.");
    REQUIRE(xmpData.empty());
  }
}

TEST_CASE("doubleToStringWithPrecision", "xmp-utils") {
  SECTION("Positive double with various precision") {
    REQUIRE(XmpUtils::doubleToStringWithPrecision(123.45678, 2) == "123.46");
    REQUIRE(XmpUtils::doubleToStringWithPrecision(123.45678, 0) == "123");
    REQUIRE(XmpUtils::doubleToStringWithPrecision(123.0, 3) == "123.0");
  }

  SECTION("Negative double") {
    REQUIRE(XmpUtils::doubleToStringWithPrecision(-98.765, 1) == "-98.8");
  }

  SECTION("Zero") {
    REQUIRE(XmpUtils::doubleToStringWithPrecision(0.0, 5) == "0.0");
  }

  SECTION("Large double") {
    REQUIRE(XmpUtils::doubleToStringWithPrecision(123456789.123456789, 2) == "123456789.12");
  }

  SECTION("Precision equal to or greater than actual decimals") {
    REQUIRE(XmpUtils::doubleToStringWithPrecision(1.23, 2) == "1.23");
    REQUIRE(XmpUtils::doubleToStringWithPrecision(1.23000, 5) == "1.23");
  }
}

TEST_CASE("splitString", "xmp-utils") {
  SECTION("Splitting with a common delimiter") {
    std::vector<std::string> expected = {"one", "two", "three"};
    REQUIRE(XmpUtils::splitString("one,two,three", ',') == expected);
  }

  SECTION("Splitting with leading/trailing delimiters") {
    std::vector<std::string> expected = {"one", "two"};
    REQUIRE(XmpUtils::splitString(",one,two,", ',') == expected);
  }

  SECTION("Splitting with multiple consecutive delimiters") {
    std::vector<std::string> expected = {"one", "two"};
    REQUIRE(XmpUtils::splitString("one,,two", ',') == expected);
  }

  SECTION("Splitting an empty string") {
    std::vector<std::string> expected = {};
    REQUIRE(XmpUtils::splitString("", ',') == expected);
  }

  SECTION("Splitting a string with no delimiter") {
    std::vector<std::string> expected = {"singleString"};
    REQUIRE(XmpUtils::splitString("singleString", ',') == expected);
  }

  SECTION("Splitting with space delimiter") {
    std::vector<std::string> expected = {"hello", "world"};
    REQUIRE(XmpUtils::splitString("hello world", ' ') == expected);
  }
}

TEST_CASE("cleanXmpText", "xmp-utils") {
  SECTION("Cleaning a standard localized text value") {
    REQUIRE(XmpUtils::cleanXmpText("lang=\"x-default\" This is the content") == "This is the content");
  }

  SECTION("Cleaning with different language tag") {
    REQUIRE(XmpUtils::cleanXmpText("lang=\"en-US\" English content here") == "English content here");
  }

  SECTION("Cleaning with no language tag") {
    REQUIRE(XmpUtils::cleanXmpText("Just a regular string") == "Just a regular string");
  }

  SECTION("Cleaning with only language tag and no content") {
    REQUIRE(XmpUtils::cleanXmpText("lang=\"x-default\"") == "");
  }

  SECTION("Cleaning an empty string") {
    REQUIRE(XmpUtils::cleanXmpText("") == "");
  }

  SECTION("Cleaning with language tag and leading/trailing whitespace after tag") {
    REQUIRE(XmpUtils::cleanXmpText("lang=\"x-default\"   Some text with space ") == "Some text with space ");
  }

  SECTION("Cleaning a string that looks like a tag but isn't") {
    REQUIRE(XmpUtils::cleanXmpText("This string has lang=\" but no closing quote") ==
            "This string has lang=\" but no closing quote");
  }
}

TEST_CASE("parseDelimitedString", "xmp-utils") {
  Exiv2::XmpData xmpData;

  SECTION("Parsing a comma-delimited string") {
    xmpData["Xmp.dc.subject"] = "tag1, tag2, tag3";
    std::vector<std::string> expected = {"tag1", "tag2", "tag3"};
    REQUIRE(XmpUtils::parseDelimitedString(xmpData, "Xmp.dc.subject", ',') == expected);
  }

  SECTION("Parsing with leading/trailing spaces around tokens") {
    xmpData["Xmp.dc.keywords"] = "  keywordA ,keywordB, keywordC  ";
    std::vector<std::string> expected = {"keywordA", "keywordB", "keywordC"};
    REQUIRE(XmpUtils::parseDelimitedString(xmpData, "Xmp.dc.keywords", ',') == expected);
  }

  SECTION("Parsing an empty XMP value") {
    xmpData["Xmp.aux.empty"] = "";
    std::vector<std::string> expected = {};
    REQUIRE(XmpUtils::parseDelimitedString(xmpData, "Xmp.aux.empty", ';') == expected);
  }

  SECTION("Parsing an XMP value with only delimiters") {
    xmpData["Xmp.aux.delimitersOnly"] = ", , ,";
    std::vector<std::string> expected = {};
    REQUIRE(XmpUtils::parseDelimitedString(xmpData, "Xmp.aux.delimitersOnly", ',') == expected);
  }

  SECTION("Parsing a non-existent XMP key") {
    std::vector<std::string> expected = {};
    REQUIRE(XmpUtils::parseDelimitedString(xmpData, "Xmp.aux.nonexistentkey", ',') == expected);
  }

  SECTION("Parsing a single token with no delimiter") {
    xmpData["Xmp.dc.coverage"] = "singleItem";
    std::vector<std::string> expected = {"singleItem"};
    REQUIRE(XmpUtils::parseDelimitedString(xmpData, "Xmp.dc.coverage", ',') == expected);
  }

  SECTION("Parsing with a different delimiter (semicolon)") {
    xmpData["Xmp.dc.description"] = "item1; item2;item3";
    std::vector<std::string> expected = {"lang=\"x-default\" item1", "item2", "item3"};
    REQUIRE(XmpUtils::parseDelimitedString(xmpData, "Xmp.dc.description", ';') == expected);
  }
}
