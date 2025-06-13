#include "TestUtils.hpp"

const std::unordered_map<SampleImage, std::string> ImageTestFixture::sampleFiles_ = {
    {SampleImage::Sample1, "sample1.jpg"},  {SampleImage::Sample2, "sample2.jpg"},
    {SampleImage::Sample3, "sample3.jpg"},  {SampleImage::Sample4, "sample4.jpg"},
    {SampleImage::SamplePNG, "sample.png"}, {SampleImage::SampleWEBP, "sample.webp"}};
