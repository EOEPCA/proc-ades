


//test2
#include <eoepca/owl/eoepcaows.hpp>
#include <eoepca/owl/owsparameter.hpp>

#include "../lib/includes/httpfuntions.hpp"

#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <optional>
#include <string>

int main(int argc, const char** argv) {


  if (argc == 1) {
    std::cerr << "arg1: ows catalog";
    return 1;
  }

  auto lib = std::make_unique<EOEPCA::EOEPCAows>("./libeoepcaows.so");
  if (!lib->IsValid()) {
    //build mac
    lib = std::make_unique<EOEPCA::EOEPCAows>("./libeoepcaows.dylib");
  }

  std::cout << "LIB version: " << lib->version() << "\n";
  int maxLen = 1024;
  auto theName = std::make_unique<char[]>(maxLen);
  lib->getParserName(theName.get(), maxLen);

  std::cout << "LIB name: " << theName.get() << "\n";
  std::cout << "Run: \n";

  std::string fileBuffer{""};
  getFromWeb(fileBuffer,argv[1]);

  std::cout << fileBuffer << "\n\n";

  std::unique_ptr<EOEPCA::OWS::OWSContext,
      std::function<void(EOEPCA::OWS::OWSContext*)>>
      ptrContext(lib->parseFromMemory(fileBuffer.c_str(),fileBuffer.size()), lib->releaseParameter);


  if (ptrContext) {
    auto& entries = ptrContext->getEntries();

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

            std::cout << "getMinOccurs: " << input->getMinOccurs() << "\n";
            std::cout << "getMaxOccurs: " << input->getMaxOccurs() << "\n";


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
                std::cout << "\t\n";
              }

              std::cout << "getDefaultSupported: \n";
              auto& supp2 = data->getDefaultSupported();
              std::cout << "\tgetEncoding: " << supp2->getEncoding() << "\n";
              std::cout << "\tgetMimeType: " << supp2->getMimeType() << "\n";
              std::cout << "\tgetSchema: " << supp2->getSchema() << "\n";
              std::cout << "\t\n";
            }
            if (input->getType() == "LiteralData") {
              auto data = dynamic_cast<EOEPCA::OWS::LiteralData*>(input.get());
              std::cout << "getDataType: " << data->getDataType() << "\n";
              std::cout << "getAllowedValuesSize: "
                        << data->getAllowedValues().size() << "\n";
              for (auto& a : data->getAllowedValues()) {
                std::cout << "AllowedValue: " << a << "\n";
              }

              std::cout << "getDefaultValue: " << data->getDefaultValue() <<"\n";

            }
            std::cout << "--------------------------\n";
          }
        }
      }
    }
  }
  return 0;
}
