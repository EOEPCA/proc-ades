#include "global.hpp"
#include "gtest/gtest.h"

TEST(eoepcaows_02, SimpleRun) {
  auto util = std::make_unique<Util>();
  std::string path{util->getLibName()};

  auto lib = std::make_unique<EOEPCA::EOEPCAows>(path);
  std::unique_ptr<EOEPCA::OWS::OWSContext,
                  std::function<void(EOEPCA::OWS::OWSContext*)>>
      ptrContext(lib->parseFromFile(util->getOws1()), lib->releaseParameter);

  EXPECT_NE(nullptr, ptrContext.get());
}

TEST(eoepcaows_02, Get_entry_identifier) {
  auto util = std::make_unique<Util>();
  std::string path{util->getLibName()};

  auto lib = std::make_unique<EOEPCA::EOEPCAows>(path);
  std::unique_ptr<EOEPCA::OWS::OWSContext,
                  std::function<void(EOEPCA::OWS::OWSContext*)>>
      ptrContext(lib->parseFromFile(util->getOws1()), lib->releaseParameter);

  EXPECT_NE(nullptr, ptrContext.get());
  if (ptrContext) {
    auto& entries = ptrContext->getEntries();
    EXPECT_EQ(1, entries.size());

    for (auto& entry : entries) {
      EXPECT_EQ("application_package_sample_app",
                entry->getPackageIdentifier());
    }
  }
}

TEST(eoepcaows_02, Get_offering_code) {
  auto util = std::make_unique<Util>();
  std::string path{util->getLibName()};

  auto lib = std::make_unique<EOEPCA::EOEPCAows>(path);
  std::unique_ptr<EOEPCA::OWS::OWSContext,
                  std::function<void(EOEPCA::OWS::OWSContext*)>>
      ptrContext(lib->parseFromFile(util->getOws1()), lib->releaseParameter);

  EXPECT_NE(nullptr, ptrContext.get());
  if (ptrContext) {
    auto& entries = ptrContext->getEntries();
    EXPECT_EQ(1, entries.size());

    for (auto& entry : entries) {
      for (auto& off : entry->getOfferings()) {
        std::cout << "OFFERING CODE: " << off->getCode() << "\n";
      }
    }
  }
}

TEST(eoepcaows_02, Get_content_And_CWL) {
  auto util = std::make_unique<Util>();
  std::string path{util->getLibName()};

  auto lib = std::make_unique<EOEPCA::EOEPCAows>(path);
  std::unique_ptr<EOEPCA::OWS::OWSContext,
                  std::function<void(EOEPCA::OWS::OWSContext*)>>
      ptrContext(lib->parseFromFile(util->getOws1()), lib->releaseParameter);

  EXPECT_NE(nullptr, ptrContext.get());
  if (ptrContext) {
    auto& entries = ptrContext->getEntries();
    EXPECT_EQ(1, entries.size());

    for (auto& entry : entries) {
      for (auto& off : entry->getOfferings()) {
        std::cout << "OFFERING CODE: " << off->getCode() << "\n";
        for (auto& y : off->getContents()) {
          std::cout << "\t" << y.type << " " << y.href << "\n";
          if (!y.tag.empty()) {
            std::cout << "tag:" << y.tag << "\n";
          }
        }
      }
    }
  }
}

TEST(eoepcaows_02, processDescription_parameters) {
  auto util = std::make_unique<Util>();
  std::string path{util->getLibName()};

  auto lib = std::make_unique<EOEPCA::EOEPCAows>(path);
  std::unique_ptr<EOEPCA::OWS::OWSContext,
                  std::function<void(EOEPCA::OWS::OWSContext*)>>
      ptrContext(lib->parseFromFile(util->getOws1()), lib->releaseParameter);

  EXPECT_NE(nullptr, ptrContext.get());
  if (ptrContext) {
    auto& entries = ptrContext->getEntries();
    EXPECT_EQ(1, entries.size());

    for (auto& entry : entries) {
      for (auto& off : entry->getOfferings()) {
        for (auto& processDescription : off->getProcessDescription()) {
          std::cout << "getIdentifier: " << processDescription->getIdentifier()
                    << "\n";
          std::cout << "getTitle: " << processDescription->getTitle() << "\n";
          std::cout << "getAbstract: " << processDescription->getAbstract()
                    << "\n";

          std::cout << "getVersion: " << processDescription->getVersion() << "\n";

          EXPECT_EQ(processDescription->getIdentifier(), "eo_metadata_generation");
          EXPECT_EQ(processDescription->getTitle(), "Earth Observation Metadata Generation");
          EXPECT_EQ(processDescription->getAbstract(),
                    "Earth Observation Metadata Generation");
          EXPECT_EQ(processDescription->getVersion(),
                    "1.0");
        }
      }
    }
  }
}

TEST(eoepcaows_02, inputs_list) {
  auto util = std::make_unique<Util>();
  std::string path{util->getLibName()};

  auto lib = std::make_unique<EOEPCA::EOEPCAows>(path);
  std::unique_ptr<EOEPCA::OWS::OWSContext,
                  std::function<void(EOEPCA::OWS::OWSContext*)>>
      ptrContext(lib->parseFromFile(util->getOws1()), lib->releaseParameter);

  EXPECT_NE(nullptr, ptrContext.get());
  if (ptrContext) {
    auto& entries = ptrContext->getEntries();
    EXPECT_EQ(1, entries.size());

    for (auto& entry : entries) {
      for (auto& off : entry->getOfferings()) {
        for (auto& processDescription : off->getProcessDescription()) {
          for (auto& input : processDescription->getInputs()) {
            std::cout << "--------------------------\n";
            std::cout << "getIdentifier: " << input->getIdentifier() << "\n";
            std::cout << "getType: " << input->getType() << "\n";
            std::cout << "--------------------------\n";
            std::cout << "getTitle: " << input->getTitle() << "\n";
            std::cout << "getAbstract: " << input->getAbstract() << "\n";

            if (input->getType() == "BoundingBoxData") {
              auto data =
                  dynamic_cast<EOEPCA::OWS::BoundingBoxData*>(input.get());

            } else if (input->getType() == "ComplexData") {
              auto data = dynamic_cast<EOEPCA::OWS::ComplexData*>(input.get());

              std::cout << "getMaximumMegabytes: "
                        << data->getMaximumMegabytes() << "\n";

              std::cout << "getSupported: \n";
              for (auto& supp : data->getSupported()) {
                std::cout << "\tgetEncoding: " << supp->getEncoding() << "\n";
                std::cout << "\tgetMimeType: " << supp->getMimeType() << "\n";
                std::cout << "\tgetSchema: " << supp->getSchema() << "\n";
              }

              auto& supp2 = data->getDefaultSupported();
              std::cout << "\tgetEncoding: " << supp2->getEncoding() << "\n";
              std::cout << "\tgetMimeType: " << supp2->getMimeType() << "\n";
              std::cout << "\tgetSchema: " << supp2->getSchema() << "\n";
            }
            if (input->getType() == "LiteralData") {
              auto data = dynamic_cast<EOEPCA::OWS::LiteralData*>(input.get());
              std::cout << "getDataType: " << data->getDataType() << "\n";
              std::cout << "getAllowedValuesSize: "
                        << data->getAllowedValues().size() << "\n";
              for (auto& a : data->getAllowedValues()) {
                std::cout << "AllowedValue: " << a << "\n";
              }
            }
            std::cout << "--------------------------\n";
          }
        }
      }
    }
  }
}
