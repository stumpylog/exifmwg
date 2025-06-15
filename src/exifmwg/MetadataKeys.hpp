#pragma once

namespace MetadataKeys {

namespace Exif {
// Standard Exif Tags
// https://exiv2.org/tags.html
constexpr const char* Orientation = "Exif.Image.Orientation";
constexpr const char* ImageDescription = "Exif.Image.ImageDescription";
} // namespace Exif

namespace Iptc {
// IPTC
// https://exiv2.org/iptc.html
constexpr const char* Caption = "Iptc.Application2.Caption";
constexpr const char* CountryName = "Iptc.Application2.CountryName";
constexpr const char* City = "Iptc.Application2.City";
constexpr const char* ProvinceState = "Iptc.Application2.ProvinceState";
constexpr const char* SubLocation = "Iptc.Application2.SubLocation";
} // namespace Iptc

namespace Xmp {
// Dublin Core
// https://exiv2.org/tags-xmp-dc.html
constexpr const char* Title = "Xmp.dc.title";
constexpr const char* Description = "Xmp.dc.description";

// IPTC Core (mapped to XMP)
// https://exiv2.org/tags-xmp-iptcExt.html
constexpr const char* IptcCountryName = "Xmp.iptc.CountryName";
constexpr const char* IptcLocation = "Xmp.iptc.Location";

// Photoshop
// https://exiv2.org/tags-xmp-photoshop.html
constexpr const char* PhotoshopCity = "Xmp.photoshop.City";
constexpr const char* PhotoshopState = "Xmp.photoshop.State";

// MWG (Metadata Working Group) Regions
// https://exiv2.org/tags-xmp-mwg-rs.html
constexpr const char* Regions = "Xmp.mwg-rs.Regions";

// MWG (Metadata Working Group) Keywords
// https://exiv2.org/tags-xmp-mwg-kw.html
constexpr const char* Keywords = "Xmp.mwg-kw.Keywords";
constexpr const char* KeywordInfo = "Xmp.mwg-kw.KeywordInfo";

// Vendor Specific / Other
// https://exiv2.org/tags-xmp-acdsee.html
constexpr const char* AcdseeCategories = "Xmp.acdsee.Categories";
// https://exiv2.org/tags-xmp-MicrosoftPhoto.html
constexpr const char* MicrosoftLastKeywordXMP = "Xmp.MicrosoftPhoto.LastKeywordXMP";
// https://exiv2.org/tags-xmp-digiKam.html
constexpr const char* DigiKamTagsList = "Xmp.digiKam.TagsList";
// https://exiv2.org/tags-xmp-lr.html
constexpr const char* LightroomHierarchicalSubject = "Xmp.lr.hierarchicalSubject";
// https://exiv2.org/tags-xmp-mediapro.html
constexpr const char* MediaProCatalogSets = "Xmp.mediapro.CatalogSets";
} // namespace Xmp

} // namespace MetadataKeys
