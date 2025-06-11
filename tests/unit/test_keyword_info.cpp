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
