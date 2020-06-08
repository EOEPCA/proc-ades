
#include "global.hpp"
#include "gtest/gtest.h"

//https://github.com/google/googletest/blob/master/googletest/docs/primer.md
// run binary with flag --gtest_break_on_failure

TEST(eoepcaows_01, CheckAssets_ows1_xml) {
  auto util = std::make_unique<Util>();

  std::ifstream infile(util->getOws1());
  EXPECT_TRUE(infile.good());
}

TEST(eoepcaows_01, CheckAssets_libs_path) {
  auto util = std::make_unique<Util>();

  EXPECT_NE("", util->getLibName());
}

TEST(eoepcaows_01, CheckAssets_libs_valid_false) {
  auto lib = std::make_unique<EOEPCA::EOEPCAows>("./libeoepcaows.so");
  if (!lib->IsValid()) {
    // build mac
    lib = std::make_unique<EOEPCA::EOEPCAows>("./libeoepcaows.dylib");
  }

  EXPECT_TRUE(lib->IsValid());
}

TEST(eoepcaows_01, CheckAssets_libs_valid_true) {
  auto util = std::make_unique<Util>();

  std::string path = util->getLibName();
  EXPECT_TRUE(!path.empty());

  auto lib = std::make_unique<EOEPCA::EOEPCAows>(path);

  EXPECT_NE(0, lib->IsValid());
}

TEST(eoepcaows_01, GetParserNameAndVersion) {
  auto util = std::make_unique<Util>();

  std::string path{util->getLibName()};

  EXPECT_TRUE(!path.empty());

  auto lib = std::make_unique<EOEPCA::EOEPCAows>(path);

  EXPECT_TRUE(lib->IsValid());

  int maxLen = 1024;
  auto theName = std::make_unique<char[]>(maxLen);
  lib->getParserName(theName.get(), maxLen);

  std::string NAMECHK{theName.get()};

  EXPECT_EQ("EOEPCA OWS Parser", NAMECHK);
  EXPECT_EQ(1, lib->version());
}