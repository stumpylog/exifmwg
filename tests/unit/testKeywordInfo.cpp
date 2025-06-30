#include <catch2/catch_test_macros.hpp>

#include <exiv2/exiv2.hpp>

#include "KeywordInfoModel.hpp"

TEST_CASE("KeywordStruct equality", "[KeywordStruct]") {
  using KS = KeywordInfoModel::KeywordStruct;

  SECTION("Equal keywords with no children or Applied") {
    KS a("Bird", {}, std::nullopt);
    KS b("Bird", {}, std::nullopt);
    REQUIRE(a == b);
  }

  SECTION("Different keywords are not equal") {
    KS a("Bird", {}, std::nullopt);
    KS b("Fish", {}, std::nullopt);
    REQUIRE_FALSE(a == b);
  }

  SECTION("Different Applied values are not equal") {
    KS a("Tree", {}, true);
    KS b("Tree", {}, false);
    REQUIRE_FALSE(a == b);
  }

  SECTION("Equal with nested children") {
    KS child("Leaf", {}, false);
    KS a("Tree", {child}, true);
    KS b("Tree", {child}, true);
    REQUIRE(a == b);
  }
}

TEST_CASE("KeywordInfoModel delimited constructor builds hierarchy", "[KeywordInfoModel]") {
  using KI = KeywordInfoModel;

  SECTION("Single path builds root and child") {
    KI model({"Animal/Mammal/Dog"}, '/');
    REQUIRE(model.Hierarchy.size() == 1);
    REQUIRE(model.Hierarchy[0].Keyword == "Animal");
    REQUIRE(model.Hierarchy[0].Children[0].Keyword == "Mammal");
    REQUIRE(model.Hierarchy[0].Children[0].Children[0].Keyword == "Dog");
  }

  SECTION("Empty path is ignored") {
    KI model({""}, '/');
    REQUIRE(model.Hierarchy.empty());
  }

  SECTION("Multiple paths merge shared ancestors") {
    KI model({"A/B/C", "A/B/D", "A/E"}, '/');
    REQUIRE(model.Hierarchy.size() == 1);
    REQUIRE(model.Hierarchy[0].Keyword == "A");
    REQUIRE(model.Hierarchy[0].Children.size() == 2);
  }
}

TEST_CASE("KeywordInfoModel merging via operators", "[KeywordInfoModel]") {
  using KS = KeywordInfoModel::KeywordStruct;
  using KI = KeywordInfoModel;

  SECTION("Merging models with overlapping roots") {
    KI a({KS("Animal", {KS("Dog", {}, true)}, true)});
    KI b({KS("Animal", {KS("Dog", {}, false), KS("Cat", {}, true)}, false)});

    KI merged = a | b;
    REQUIRE(merged.Hierarchy.size() == 1);
    const KS& animal = merged.Hierarchy[0];
    REQUIRE(animal.Keyword == "Animal");
    REQUIRE(animal.Applied == true); // true | false = true
    REQUIRE(animal.Children.size() == 2);
  }

  SECTION("Operator |= modifies the left-hand model") {
    KI a({KS("X", {}, false)});
    KI b({KS("X", {}, true), KS("Y", {}, true)});

    a |= b;
    REQUIRE(a.Hierarchy.size() == 2);
    REQUIRE(a.Hierarchy[0].Keyword == "X");
    REQUIRE(a.Hierarchy[0].Applied == true);
  }

  SECTION("Merging with disjoint keywords combines both") {
    KI a({KS("Alpha", {}, true)});
    KI b({KS("Beta", {}, false)});

    KI merged = a | b;
    REQUIRE(merged.Hierarchy.size() == 2);
  }
}

TEST_CASE("KeywordInfoModel fromXmp and toXmp round trip", "[KeywordInfoModel][XMP]") {
  using KS = KeywordInfoModel::KeywordStruct;
  using KI = KeywordInfoModel;
  using namespace Exiv2;

  SECTION("Round trip with single keyword") {
    XmpData xmp;
    xmp["Xmp.mwg-kw.Keywords/mwg-kw:Hierarchy[1]/mwg-kw:Keyword"] = "Animals";
    xmp["Xmp.mwg-kw.Keywords/mwg-kw:Hierarchy[1]/mwg-kw:Applied"] = "True";

    KeywordInfoModel model = KeywordInfoModel::fromXmp(xmp);
    REQUIRE(model.Hierarchy.size() == 1);
    REQUIRE(model.Hierarchy[0].Keyword == "Animals");
    REQUIRE(model.Hierarchy[0].Applied == true);

    XmpData outXmp;
    model.toXmp(outXmp);

    // Re-parse it and confirm it's the same
    KeywordInfoModel model2 = KeywordInfoModel::fromXmp(outXmp);
    REQUIRE(model == model2);
  }

  SECTION("Round trip with nested children") {
    XmpData xmp;
    auto prefix = "Xmp.mwg-kw.Keywords/mwg-kw:Hierarchy[1]";
    xmp[prefix + std::string("/mwg-kw:Keyword")] = "Nature";
    xmp[prefix + std::string("/mwg-kw:Applied")] = "False";
    xmp[prefix + std::string("/mwg-kw:Children")] = "";

    xmp[prefix + std::string("/mwg-kw:Children[1]/mwg-kw:Keyword")] = "Trees";
    xmp[prefix + std::string("/mwg-kw:Children[1]/mwg-kw:Applied")] = "True";

    KeywordInfoModel model = KeywordInfoModel::fromXmp(xmp);
    REQUIRE(model.Hierarchy.size() == 1);
    REQUIRE(model.Hierarchy[0].Keyword == "Nature");
    REQUIRE(model.Hierarchy[0].Applied == false);
    REQUIRE(model.Hierarchy[0].Children.size() == 1);
    REQUIRE(model.Hierarchy[0].Children[0].Keyword == "Trees");
    REQUIRE(model.Hierarchy[0].Children[0].Applied == true);

    XmpData outXmp;
    model.toXmp(outXmp);
    KeywordInfoModel roundTrip = KeywordInfoModel::fromXmp(outXmp);
    REQUIRE(model == roundTrip);
  }

  SECTION("Empty hierarchy results in no output") {
    XmpData xmp;
    KeywordInfoModel model(std::vector<KeywordInfoModel::KeywordStruct>{});
    model.toXmp(xmp);
    REQUIRE(xmp.empty());
  }
}

TEST_CASE("KeywordInfoModel mergeApplied logic via operator|=", "[KeywordInfoModel][mergeApplied]") {
  using KS = KeywordInfoModel::KeywordStruct;
  using KI = KeywordInfoModel;

  auto makeModel = [](std::optional<bool> applied) { return KI({KS("A", {}, applied)}); };

  SECTION("nullopt | nullopt = nullopt") {
    KI a = makeModel(std::nullopt);
    KI b = makeModel(std::nullopt);

    KI merged = a | b;
    REQUIRE_FALSE(merged.Hierarchy[0].Applied.has_value());
  }

  SECTION("nullopt | false = false") {
    KI a = makeModel(std::nullopt);
    KI b = makeModel(false);

    KI merged = a | b;
    REQUIRE(merged.Hierarchy[0].Applied == false);
  }

  SECTION("nullopt | true = true") {
    KI a = makeModel(std::nullopt);
    KI b = makeModel(true);

    KI merged = a | b;
    REQUIRE(merged.Hierarchy[0].Applied == true);
  }

  SECTION("false | nullopt = false") {
    KI a = makeModel(false);
    KI b = makeModel(std::nullopt);

    KI merged = a | b;
    REQUIRE(merged.Hierarchy[0].Applied == false);
  }

  SECTION("true | nullopt = true") {
    KI a = makeModel(true);
    KI b = makeModel(std::nullopt);

    KI merged = a | b;
    REQUIRE(merged.Hierarchy[0].Applied == true);
  }
}

TEST_CASE("KeywordStruct sorting and comparison", "[keywords][unit]") {

  SECTION("Direct comparison of two KeywordStruct objects") {
    // Basic alphabetical comparison
    CHECK(KeywordInfoModel::KeywordStruct("Apple") < KeywordInfoModel::KeywordStruct("Banana"));
    CHECK(KeywordInfoModel::KeywordStruct("Cherry") > KeywordInfoModel::KeywordStruct("Banana"));
    CHECK(KeywordInfoModel::KeywordStruct("Date") == KeywordInfoModel::KeywordStruct("Date"));

    // Case sensitivity matters in default string comparison
    CHECK(KeywordInfoModel::KeywordStruct("apple") > KeywordInfoModel::KeywordStruct("Banana"));

    // Comparison when keywords are the same, but 'Applied' differs
    CHECK(KeywordInfoModel::KeywordStruct("Fruit", {}, true) > KeywordInfoModel::KeywordStruct("Fruit", {}, false));
    CHECK(KeywordInfoModel::KeywordStruct("Fruit", {}, false) < KeywordInfoModel::KeywordStruct("Fruit", {}, true));

    // 'Applied' has value vs. no value (a value is "greater")
    CHECK(KeywordInfoModel::KeywordStruct("Fruit", {}, true) > KeywordInfoModel::KeywordStruct("Fruit", {}));
    CHECK(KeywordInfoModel::KeywordStruct("Fruit", {}, false) > KeywordInfoModel::KeywordStruct("Fruit", {}));
    CHECK(KeywordInfoModel::KeywordStruct("Fruit", {}) < KeywordInfoModel::KeywordStruct("Fruit", {}, true));

    // Comparison with children (should not affect sorting, only equality)
    auto appleWithChild = KeywordInfoModel::KeywordStruct("Apple", {KeywordInfoModel::KeywordStruct("Granny Smith")});
    auto appleNoChild = KeywordInfoModel::KeywordStruct("Apple");
    // They are not equal
    CHECK_FALSE(appleWithChild == appleNoChild);
    // But for sorting purposes, they are equivalent if Keyword and Applied are the same
    CHECK_FALSE(appleWithChild < appleNoChild);
    CHECK_FALSE(appleWithChild > appleNoChild);
  }

  SECTION("Sorting a vector of KeywordStruct objects") {
    std::vector<KeywordInfoModel::KeywordStruct> keywords = {
        KeywordInfoModel::KeywordStruct("Zoo"),
        KeywordInfoModel::KeywordStruct("Animal", {}, true), // Same keyword, applied=true
        KeywordInfoModel::KeywordStruct("Plant", {}, false),
        KeywordInfoModel::KeywordStruct("Animal", {}),        // Same keyword, applied=nullopt
        KeywordInfoModel::KeywordStruct("Animal", {}, false), // Same keyword, applied=false
        KeywordInfoModel::KeywordStruct("Plant", {}, true),
        KeywordInfoModel::KeywordStruct("Mineral")};

    // Sort the vector using the overloaded <=>
    std::sort(keywords.begin(), keywords.end());

    // Define the expected order after sorting
    std::vector<KeywordInfoModel::KeywordStruct> expected = {
        KeywordInfoModel::KeywordStruct("Animal", {}),        // 1. nullopt
        KeywordInfoModel::KeywordStruct("Animal", {}, false), // 2. false
        KeywordInfoModel::KeywordStruct("Animal", {}, true),  // 3. true
        KeywordInfoModel::KeywordStruct("Mineral"),
        KeywordInfoModel::KeywordStruct("Plant", {}, false), // 1. false
        KeywordInfoModel::KeywordStruct("Plant", {}, true),  // 2. true
        KeywordInfoModel::KeywordStruct("Zoo")};

    REQUIRE(keywords.size() == expected.size());
    for (size_t i = 0; i < keywords.size(); ++i) {
      CHECK(keywords[i].Keyword == expected[i].Keyword);
      CHECK(keywords[i].Applied == expected[i].Applied);
    }

    // Also check the full vector equality for good measure
    CHECK(keywords == expected);
  }

  SECTION("Sorting nested children") {
    KeywordInfoModel model(
        std::vector<std::string>{"Animal/Vertebrate/Fish", "Animal/Vertebrate/Bird", "Animal/Invertebrate/Insect"},
        '/');

    // The constructor should automatically sort the hierarchy.
    // Let's verify the order.
    REQUIRE(model.Hierarchy.size() == 1);
    REQUIRE(model.Hierarchy[0].Keyword == "Animal");

    const auto& children = model.Hierarchy[0].Children;
    REQUIRE(children.size() == 2);

    // Check level 1 children
    CHECK(children[0].Keyword == "Invertebrate");
    CHECK(children[1].Keyword == "Vertebrate");

    // Check level 2 children (grandchildren of "Animal")
    const auto& grandchildren = children[1].Children; // Children of "Vertebrate"
    REQUIRE(grandchildren.size() == 2);
    CHECK(grandchildren[0].Keyword == "Bird");
    CHECK(grandchildren[1].Keyword == "Fish");
  }
}
