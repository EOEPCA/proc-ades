
#include "includes/parser.hpp"

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <optional>

#include <fstream>//TOREMOVE

#include "includes/httpfuntions.hpp"
#include "includes/macro.hpp"
#include "includes/yaml/yaml.hpp"
#define echo std::cout

namespace EOEPCA {

void dumpCWLMODEL(const TOOLS::Object* m, int level) {
  level++;
  std::string tab = std::string((level * 2), ' ');
  echo << tab << "id: \"" << m->getQ() << "\" val: \"" << m->getF() << "\""
       << "\n";
  for (auto& i : m->getChildren()) {
    dumpCWLMODEL(i.get(), level);
  }
}

class TypeCWL {
  std::string typeBase_{""};
  std::string typeParsed_{""};
  std::list<std::string> symbols_{};

  bool array_{false};
  bool optional_{false};

  bool good_{false};
  std::string error_{"value is empty"};

 private:
  void setError(std::string_view e) {
    if (e.empty()) {
      good_ = true;
      error_.clear();
    } else {
      good_ = false;
      error_ = e;
    }
  }

  void resetError() {
    good_ = false;
    error_ = "value is empty";
  }
  void resetVals() {
    typeBase_.clear();
    typeParsed_.clear();
    symbols_.clear();
    array_ = false;
    optional_ = false;
  }

  void setOtherValue() {
    auto nposBob = typeBase_.find("[]");

    if (!array_){
      array_ = (nposBob != std::string::npos);
    }

    auto nposQM = typeBase_.find("?");
    optional_ = (nposQM != std::string::npos);

    typeParsed_ = typeBase_;
    std::list<std::string> list{"[]", "?"};

    for (auto& s : list) {
      auto np = typeParsed_.find(s);
      if (np != std::string::npos) {
        typeParsed_.replace(np, s.size(), "");
      }
    }
  }

  void parseOther(const TOOLS::Object* obj) {
    if (obj){
      auto qf=obj->getQlf();
      if (qf.first=="type" && qf.second.empty()){

        TOOLS::Object a;
        a.setQlf("noBase","");
        for (auto& o : obj->getChildren()) {
            a.addChildren(o.get());
        }
        //=============================================
        //TODO: create operator=
        //=============================================
        auto typeCWL = std::make_unique<TypeCWL>( &a);
        if (empty() && typeCWL->isGood()) {
          typeBase_ = typeCWL->getTypeBase();
          typeParsed_ = typeCWL->getTypeParsed();
          array_ = typeCWL->isArray();
          optional_ = typeCWL->isOptional();
          for (auto& s : typeCWL->getSymbols()) symbols_.emplace_back(s);
        }
        //=============================================
      }
    }
  }

  void parseArray(const TOOLS::Object* obj) {
    if (obj) {
      bool forceOptional{false};

      for (auto& o : obj->getChildren()) {
        if (o->isQlfEmpty()) {
          forceOptional = true;
        } else {
          //=============================================
          //TODO: create operator=
          //=============================================
          auto typeCWL = std::make_unique<TypeCWL>(o.get());
          if (empty() && typeCWL->isGood()) {
            typeBase_ = typeCWL->getTypeBase();
            typeParsed_ = typeCWL->getTypeParsed();
            array_ = typeCWL->isArray();
            optional_ = typeCWL->isOptional();
            for (auto& s : typeCWL->getSymbols()) symbols_.emplace_back(s);
          }
          //=============================================
        }
      }
      if (forceOptional) {
        optional_ = true;
      }
    }
  }

 public:
  TypeCWL() = delete;
  explicit TypeCWL(const TOOLS::Object* obj) {
    resetError();
    resetVals();
    if (!obj->hasChildren() && !obj->isQlfEmpty()) {
      typeBase_ = obj->getF();
      setOtherValue();
    } else {
      auto theType = obj->find("type", "", false);
      if (theType) {
        auto theF = theType->getF();
        if (!theF.empty()) {
          if (theF == "array") {
            auto items = obj->find("items", "", false);
            if (items) {

              if (items->getF().empty()){
                parseArray(items);
              }else{
                typeBase_=items->getF();
              }
              bool myOpt=isOptional();
              setOtherValue();

              if(myOpt){
                this->optional_=true;
              }
              this->array_=true;

            }
          } else if (theF == "enum") {
            typeBase_ = "enum";
            auto symbols = obj->find("symbols", "");
            if(symbols)
              for (auto& s : symbols->getChildren()) {
                if (!s->getF().empty()) symbols_.emplace_back(s->getF());
              }
            setOtherValue();
          } else if (theF == "record") {
            setError("Type RecordSchema not supported yet");
            resetVals();
            return;
          } else {
            typeBase_ = theF;
            setOtherValue();
          }
        } else {

          if(theType->isArray()){
            parseArray(theType);
          }else{
            parseOther(theType);
          }
        }
      }
    }

    setError("");
  };
  TypeCWL(const TypeCWL&) = default;
  TypeCWL(TypeCWL&&) = default;
  ~TypeCWL() = default;

 public:
  const std::string& getTypeBase() const { return typeBase_; }
  const std::string& getTypeParsed() const { return typeParsed_; }
  bool isGood() const { return good_; }
  const std::string& getError() const { return error_; }
  bool isArray() const { return array_; }
  bool isOptional() const { return optional_; }
  const std::list<std::string>& getSymbols() const { return symbols_; }
  bool empty() { return typeBase_.empty(); }
};

class NamespaceCWL {
  std::string stac_{""};
  std::string opensearch_{""};
  std::string ows_{""};

 public:
  NamespaceCWL() = delete;
  NamespaceCWL(const NamespaceCWL&) = default;
  NamespaceCWL(NamespaceCWL&&) = default;

  explicit NamespaceCWL(const TOOLS::Object* obj) {
    if (obj) {
      for (auto& a : obj->getChildren()) {
        if (a->getF() == XMLNS_STAC) this->stac_ = a->getQ();
        if (a->getF() == XMLNS_OWS) this->ows_ = a->getQ();
        if (a->getF() == XMLNS_OPENSEARCH) this->opensearch_ = a->getQ();
      }
    }
  }

  ~NamespaceCWL() = default;

 public:
  const std::string& getStac() const { return stac_; }
  const std::string& getOpensearch() const { return opensearch_; }
  const std::string& getOws() const { return ows_; }
};

const std::string& Parser::getName() const { return name; }

enum class OBJECT_NODE { INPUT, OUTPUT };

using MAP_PARSER =
std::map<std::string,
    const std::function<std::unique_ptr<OWS::Param>(xmlNode*)>>;

auto getWithoutSquareBob = [](const std::string& type) -> std::string {
  auto npos = type.find("[]");
  if (npos != std::string::npos) {
    return type.substr(0, npos);
  }
  return type;
};

auto hasNamespace = [](const std::string& val) -> std::string {
  auto npos = val.find(":");
  if (npos != std::string::npos) {
    return val.substr(0, npos);
  }
  return std::string("");
};

using fnccwl = std::unique_ptr<OWS::Param>(const NamespaceCWL*,
                                           const TOOLS::Object*, TypeCWL*);
using tFnccwl = const std::function<fnccwl>;
using MAP_PARSER_CWL = std::map<std::string, EOEPCA::tFnccwl>;

std::unique_ptr<OWS::Format> getFormat(xmlNode* nodeComplexDataFormat) {
  auto format = std::make_unique<OWS::Format>();

  FOR(f, nodeComplexDataFormat) {
    if (XML_COMPARE(f->name, "MimeType")) {
      format->setMimeType(std::string(CHAR_BAD_CAST xmlNodeGetContent(f)));
    } else if (XML_COMPARE(f->name, "Encoding")) {
      format->setEncoding(std::string(CHAR_BAD_CAST xmlNodeGetContent(f)));
    } else if (XML_COMPARE(f->name, "Schema")) {
      format->setSchema(std::string(CHAR_BAD_CAST xmlNodeGetContent(f)));
    }
  }

  return format;
}

[[nodiscard]] std::unique_ptr<OWS::Param> parseComplexData(
    xmlNode* nodeComplexData) {
  // echo "PARSECOMPLEXDATA DATA\n";

  auto complexData = std::make_unique<OWS::ComplexData>();

  xmlChar* maximumMegabytes =
      xmlGetProp(nodeComplexData, (const xmlChar*)"maximumMegabytes");

  if (maximumMegabytes) {
    complexData->setMaximumMegabytes(
        std::string(CHAR_BAD_CAST maximumMegabytes));
  }

  FOR(box, nodeComplexData) {
    if (XML_COMPARE(box->name, "Default")) {
      FOR(format, box) {
        auto mFrmat = getFormat(format);
        complexData->moveDefaultSupported(mFrmat);
      }
    } else if (XML_COMPARE(box->name, "Supported")) {
      FOR(format, box) {
        auto mFrmat = getFormat(format);
        complexData->moveAddSupported(mFrmat);
      }
    }
  }

  return std::move(complexData);
}

[[nodiscard]] std::unique_ptr<OWS::Param> parseBoundingBoxData(
    xmlNode* nodeBoundingBoxData) {
  // echo "PARSEBOUNDINGBOXDATA DATA\n";

  auto boundingBoxData = std::make_unique<OWS::BoundingBoxData>();

  FOR(box, nodeBoundingBoxData) {
    if (XML_COMPARE(box->name, "Supported")) {
      FOR(crs, box) {
        if (XML_COMPARE(crs->name, "CRS")) {
          boundingBoxData->addSupportedValues(
              std::string(CHAR_BAD_CAST xmlNodeGetContent(crs)));
        }
      }
    } else if (XML_COMPARE(box->name, "Default")) {
      if (box->children &&
          !xmlStrcmp(box->children->name, (const xmlChar*)"CRS")) {
        boundingBoxData->setDefault(
            std::string(CHAR_BAD_CAST xmlNodeGetContent(box->children)));
      }
    }
  }

  return std::move(boundingBoxData);
}

[[nodiscard]] std::unique_ptr<OWS::Param> parseLiteralData(
    xmlNode* nodeLiteralData) {
  auto literData = std::make_unique<OWS::LiteralData>();

  // echo "LITERLA DATA\n";

  FOR(ldata, nodeLiteralData) {
    if (IS_CHECK(ldata, "AnyValue", XMLNS_WPS1)) {
    } else if (IS_CHECK(ldata, "AllowedValues", XMLNS_OWS)) {
      FOR(value, ldata) {
        if (IS_CHECK(value, "Value", XMLNS_OWS)) {
          literData->addAllowedValues(
              std::string(CHAR_BAD_CAST xmlNodeGetContent(value)));
        }
      }
    } else if (IS_CHECK(ldata, "DataType", XMLNS_OWS)) {
      literData->setDataType(
          std::string(CHAR_BAD_CAST xmlNodeGetContent(ldata)));
    } else if (IS_CHECK(ldata, "DefaultValue", XMLNS_OWS)) {
      literData->setDefault(
          std::string(CHAR_BAD_CAST xmlNodeGetContent(ldata)));
    }
  }

  return std::move(literData);
}

void parseObject(MAP_PARSER& mapParser, xmlNode* nodeObject,
                 OBJECT_NODE objectNode,
                 std::unique_ptr<OWS::OWSProcessDescription>& ptrParams) {
  std::unique_ptr<OWS::Param> param{nullptr};

  auto ptrOccurs = std::make_unique<OWS::Occurs>();
  auto ptrDescriptor = std::make_unique<OWS::Descriptor>();

  xmlChar* minOccurs = xmlGetProp(nodeObject, (const xmlChar*)"minOccurs");
  xmlChar* maxOccurs = xmlGetProp(nodeObject, (const xmlChar*)"maxOccurs");
  if (minOccurs) {
    ptrOccurs->setMinOccurs(std::string(CHAR_BAD_CAST minOccurs));
  }
  if (maxOccurs) {
    ptrOccurs->setMaxOccurs(std::string(CHAR_BAD_CAST maxOccurs));
  }

  FOR(input, nodeObject) {
    if (IS_CHECK(input, "Identifier", XMLNS_OWS)) {
      ptrDescriptor->setIdentifier(
          std::string(CHAR_BAD_CAST xmlNodeGetContent(input)));
    } else if (IS_CHECK(input, "Title", XMLNS_OWS)) {
      ptrDescriptor->setTitle(
          std::string(CHAR_BAD_CAST xmlNodeGetContent(input)));
    } else if (IS_CHECK(input, "Abstract", XMLNS_OWS)) {
      ptrDescriptor->setAbstract(
          std::string(CHAR_BAD_CAST xmlNodeGetContent(input)));
    }

    if (auto f = mapParser[std::string(CHAR_BAD_CAST input->name)]) {
      if (f) param = f(input);
    }
  }

  if (param) {
    *param << *ptrDescriptor << *ptrOccurs;
    switch (objectNode) {
      case OBJECT_NODE::INPUT: {
        ptrParams->addMoveInput(param);
        break;
      }
      case OBJECT_NODE::OUTPUT: {
        ptrParams->addMoveOutput(param);
        break;
      }
    }
  }
}

void parseOutput(xmlNode* nodeOutput) {}

void parseProcessDescription(
    xmlNode* processDescription,
    std::unique_ptr<OWS::OWSProcessDescription>& ptrParams) {
  MAP_PARSER mapParser{};
  mapParser.emplace(FNCMAP(LiteralData, parseLiteralData));
  mapParser.emplace(FNCMAP(LiteralOutput, parseLiteralData));
  mapParser.emplace(FNCMAP(BoundingBoxData, parseBoundingBoxData));
  mapParser.emplace(FNCMAP(BoundingBoxOutput, parseBoundingBoxData));
  mapParser.emplace(FNCMAP(ComplexData, parseComplexData));
  mapParser.emplace(FNCMAP(ComplexOutput, parseComplexData));

  xmlChar* processVersion =
      xmlGetProp(processDescription, (const xmlChar*)"processVersion");
  if (processVersion) {
    ptrParams->setVersion(std::string(CHAR_BAD_CAST processVersion));
  }

  FOR(inner_cur_node, processDescription) {
    if (IS_CHECK(inner_cur_node, "Identifier", XMLNS_OWS)) {
      ptrParams->setIdentifier(
          std::string(CHAR_BAD_CAST xmlNodeGetContent(inner_cur_node)));
    } else if (IS_CHECK(inner_cur_node, "Title", XMLNS_OWS)) {
      ptrParams->setTitle(
          std::string(CHAR_BAD_CAST xmlNodeGetContent(inner_cur_node)));
    } else if (IS_CHECK(inner_cur_node, "Abstract", XMLNS_OWS)) {
      ptrParams->setAbstract(
          std::string(CHAR_BAD_CAST xmlNodeGetContent(inner_cur_node)));
    } else if (IS_CHECK(inner_cur_node, "DataInputs", XMLNS_ATOM)) {
      FOR(input, inner_cur_node) {
        // parser inputs
        parseObject(mapParser, input, OBJECT_NODE::INPUT, ptrParams);
      }
    } else if (IS_CHECK(inner_cur_node, "ProcessOutputs", XMLNS_ATOM)) {
      // parser outputs
      FOR(I, inner_cur_node) {
        if (I->children && XML_COMPARE(I->name, "Output")) {
          parseObject(mapParser, I, OBJECT_NODE::OUTPUT, ptrParams);
        }
      }
    }
  }
}

void getCWLInputDescriptor(const NamespaceCWL* namespaces,
                           const TOOLS::Object* obj,
                           EOEPCA::OWS::Descriptor& descriptor) {
  std::string id, doc, label;

  if (!obj->hasChildren() && !obj->isQlfEmpty()) {
    id = obj->getQ();
  }

  if (id.empty()) {
    if (!obj->isQlfEmpty()) {
      id = obj->getQ();
    }
    if (id.empty()) {
      id = obj->findAndReturnF("id", "", false);
    }
  }

  label = obj->findAndReturnF("label", "", false);
  doc = obj->findAndReturnF("doc", "", false);

  if (label.empty()) label = id;
  if (doc.empty()) doc = id;

  descriptor.setIdentifier(id);
  descriptor.setTitle(label);
  descriptor.setAbstract(doc);
}

std::unique_ptr<OWS::Param> CWLTypeParserSpecialization(
    const NamespaceCWL* namespaces, const TOOLS::Object* obj,
    EOEPCA::OWS::Descriptor& descriptor, TypeCWL* typeCWL) {
  std::unique_ptr<OWS::Param> theReturnParam{nullptr};

  //  dumpCWLMODEL(obj, 0);

  if (!namespaces->getOws().empty()) {
    std::string mustBeIgnored{namespaces->getOws()};
    mustBeIgnored.append(":ignore");
    auto r = obj->find(mustBeIgnored, "", false);
    if (r) return theReturnParam;
  }

  std::list<std::unique_ptr<EOEPCA::OWS::Format>> formats;
  if (!namespaces->getStac().empty()) {
    std::string catalog{namespaces->getStac()};
    catalog.append(":catalog");

    auto pCatalog = obj->find(catalog, "", false);
    if (pCatalog) {
      formats.emplace_back(new EOEPCA::OWS::Format("application/opensearchdescription+xml"));
      formats.emplace_back(new EOEPCA::OWS::Format("application/atom+xml"));
      formats.emplace_back(new EOEPCA::OWS::Format("application/geo+json; profile=stac"));
    }
  }

  if (!namespaces->getOpensearch().empty()) {
    std::string opensearch{namespaces->getOpensearch()};
    opensearch.append(":url");
    auto pOpensearch = obj->find(opensearch, "", false);
    if (pOpensearch) {
      formats.emplace_back(new EOEPCA::OWS::Format("application/atom+xml"));
      formats.emplace_back(
          new EOEPCA::OWS::Format("application/opensearchdescription+xml"));
    }
  }

  if (!formats.empty()) {
    auto cd = std::make_unique<OWS::ComplexData>();
    std::unique_ptr<OWS::Format> formatDefault{nullptr};
    for (auto& f : formats) {
      if (!formatDefault) {
        formatDefault = std::make_unique<OWS::Format>();
        *formatDefault = *f;
      }
      cd->moveAddSupported(f);
    }
    if (formatDefault) {
      cd->moveDefaultSupported(formatDefault);
    }

    theReturnParam = std::move(cd);
  }

  if (!namespaces->getOws().empty()) {
    static std::string ows_BoundingBox{"ows:BoundingBox"};

    std::string csr{namespaces->getOws()};
    csr.append(":CRS");

    auto pCsr = obj->find(csr, "", false);
    if (pCsr && obj->findAndReturnF("format", "", false) == ows_BoundingBox) {
      auto boundingBoxData = std::make_unique<OWS::BoundingBoxData>();

      bool isSet = false;
      for (auto& s : obj->getChildren()) {
        boundingBoxData->addSupportedValues(s->getF());
        if (!isSet) {
          isSet = true;
          boundingBoxData->setDefault(s->getF());
        }
      }

      theReturnParam = std::move(boundingBoxData);
    }
  }

  if (!theReturnParam) {
    auto literData = std::make_unique<OWS::LiteralData>();

    if (typeCWL->getTypeParsed()=="File" || typeCWL->getTypeParsed()=="Directory"){
      literData->setDataType("string");
    }else{
      literData->setDataType(typeCWL->getTypeParsed());
    }
    literData->setDefault(obj->findAndReturnF("default", "", false));

    if (typeCWL->getTypeParsed() == "enum") {
      literData->setDataType("string");
      for (auto& a : typeCWL->getSymbols()) {
        literData->addAllowedValues(a);
        if (literData->getDefaultValue().empty()) {
          literData->setDefault(a);
        }
      }
    }

    theReturnParam = std::move(literData);
  }

  // generic
  if (theReturnParam) {
    // isArray
    theReturnParam->setIdentifier(descriptor.getIdentifier());
    theReturnParam->setTitle(descriptor.getTitle());
    theReturnParam->setAbstract(descriptor.getAbstract());
    theReturnParam->setMinOccurs(1);
    if (typeCWL->isArray()) {
      theReturnParam->setMaxOccurs(999);
    }

    if (typeCWL->isOptional()) {
      theReturnParam->setMinOccurs(0);
      if (theReturnParam->getMaxOccurs()==0){
        theReturnParam->setMaxOccurs(1);
      }
    }
  }

  return theReturnParam;
}

/***
 *
 * @param namespaces
 * @param obj
 * @return it type is null or the id is null --> returns null.
 */
std::unique_ptr<OWS::Param> CWLTypeParser(const NamespaceCWL* namespaces,
                                          const TOOLS::Object* obj,
                                          TypeCWL* typeCWL) {
  EOEPCA::OWS::Descriptor descriptor;
  descriptor.setTag(typeCWL->getTypeParsed());
  getCWLInputDescriptor(namespaces, obj, descriptor);

  if (descriptor.getIdentifier().empty()) {
    return nullptr;
  }

  return CWLTypeParserSpecialization(namespaces, obj, descriptor, typeCWL);
}

void parseCwlInputs(MAP_PARSER_CWL& mapParserCwl,
                    const NamespaceCWL* namespaces, const TOOLS::Object* obj,
                    OWS::OWSProcessDescription* processDescription,
                    std::string_view type) {
  // ptrToNull

  auto typeCWL = std::make_unique<TypeCWL>(obj);
  if (typeCWL->isGood()) {
    auto fnc = mapParserCwl[typeCWL->getTypeParsed()];
    if (fnc) {
      std::unique_ptr<OWS::Param> ptrParam(nullptr);
      ptrParam = fnc(namespaces, obj, typeCWL.get());
      if (type == "inputs")
        processDescription->addMoveInput(ptrParam);
      else if (type == "outputs")
        processDescription->addMoveOutput(ptrParam);
    }

  } else {
    std::string err("@parseCwlInputs ");
    err.append(typeCWL->getError());
    throw std::runtime_error(err);
  }
}

std::unique_ptr<OWS::Param> CWLTypeEnum(const NamespaceCWL* namespaces,
                                        const TOOLS::Object* obj,
                                        TypeCWL* typeCWL) {


//  dumpCWLMODEL(obj,0);

  EOEPCA::OWS::Descriptor descriptor;
  descriptor.setTag(typeCWL->getTypeParsed());
  getCWLInputDescriptor(namespaces, obj, descriptor);

  if (descriptor.getIdentifier().empty()) {
    descriptor.setIdentifier(obj->findAndReturnF("name", "", false));
  }
  if (descriptor.getIdentifier().empty()) {
    return nullptr;
  }

  return CWLTypeParserSpecialization(namespaces, obj, descriptor, typeCWL);
}

void parserOfferingCWL(std::unique_ptr<OWS::OWSOffering>& ptrOffering) {
  if (ptrOffering) {
    MAP_PARSER_CWL mapParser{};
    for (auto& s : CWLTYPE_LIST) mapParser.emplace(FNCMAPS(s, CWLTypeParser));
    mapParser.emplace(FNCMAPS("enum", CWLTypeEnum));

    for (auto& content : ptrOffering->getContents()) {
      if (!content.tag.empty()) {
        auto yamlCwl = std::make_unique<CWL::PARSER::YamlToObject>(
            content.tag.c_str(), content.tag.size());
        auto cwl = yamlCwl->parse();

        auto namespaceCWL =
            std::make_unique<NamespaceCWL>(cwl->find("$namespaces", ""));

        //        dumpCWLMODEL(namespaces,0);
        auto pWorkflow = cwl->find("class", "Workflow");
        if (pWorkflow) {
          auto processDescription =
              std::make_unique<OWS::OWSProcessDescription>();
          processDescription->setDescriber(content.tag);
          processDescription->setIdentifier(
              pWorkflow->findAndReturnF("id", "", false));
          processDescription->setTitle(
              pWorkflow->findAndReturnF("label", "", false));
          processDescription->setAbstract(
              pWorkflow->findAndReturnF("doc", "", false));
          if (processDescription->getTitle().empty()) {
            processDescription->setTitle("NotDefined");
          }

          if (!namespaceCWL->getOws().empty()) {
            std::string owsVersion{namespaceCWL->getOws()};
            owsVersion.append(":version");

            processDescription->setVersion(
                pWorkflow->findAndReturnF(owsVersion, "", false));
            if (processDescription->getVersion().empty()) {

//              dumpCWLMODEL(pWorkflow,0);

              std::string err{"Workflow version empty."};
              throw std::runtime_error(err);
            }
          }

          if (processDescription->getAbstract().empty()) {
            if (!processDescription->getTitle().empty()) {
              processDescription->setAbstract(processDescription->getTitle());
            } else {
              processDescription->setAbstract("NotDefined");
            }
          }

          std::list<const TOOLS::Object*> toParse{
              pWorkflow->find("inputs", ""), pWorkflow->find("outputs", "")};

          for (auto& p : toParse) {
            if (p) {
              if (p->hasChildren()) {
                for (auto& obj : p->getChildren()) {
                  parseCwlInputs(mapParser, namespaceCWL.get(), obj.get(),
                                 processDescription.get(), p->getQ());
                }
              }
            }
          }

          ptrOffering->moveAddProcessDescription(processDescription);

        } else {
          // NO WORKFLOW!!
        }
      }
    }
  }
}

void parseOffering(xmlNode* offering_node,
                   std::unique_ptr<OWS::OWSOffering>& ptrOffering) {
  xmlChar* code = xmlGetProp(offering_node, (const xmlChar*)"code");
  if (code) {
    ptrOffering->setCode(std::string(CHAR_BAD_CAST code));
  }

  FOR(inner_cur_node, offering_node) {
    if (IS_CHECK(inner_cur_node, "content", XMLNS_OWC)) {
      xmlChar* type = xmlGetProp(inner_cur_node, (const xmlChar*)"type");
      xmlChar* href = xmlGetProp(inner_cur_node, (const xmlChar*)"href");

      if (href){
        ptrOffering->addContent(std::string(CHAR_BAD_CAST type),
                                std::string(CHAR_BAD_CAST href));
      }{

//        std::ifstream ifs("myfile.txt");
//        std::string content( (std::istreambuf_iterator<char>(ifs) ),
//                             (std::istreambuf_iterator<char>()    ) );

        ptrOffering->addContentTag(
            std::string(CHAR_BAD_CAST type),
            std::string(CHAR_BAD_CAST xmlNodeGetContent(inner_cur_node))
            );
      }

    } else if (IS_CHECK(inner_cur_node, "operation", XMLNS_OWC)) {
      xmlChar* mCode = xmlGetProp(inner_cur_node, (const xmlChar*)"code");
      if (mCode) {
        if (XML_COMPARE(mCode, "DescribeProcess")) {
          if (inner_cur_node->children) {
            FOR(desc, inner_cur_node->children) {
              if (IS_CHECK(desc, "ProcessDescription", XMLNS_ATOM)) {
                auto ptrParams = std::make_unique<OWS::OWSProcessDescription>();
                parseProcessDescription(desc, ptrParams);
                ptrOffering->moveAddProcessDescription(ptrParams);
              }
            }
          }
        }
      }
    }
  }
}

void parseEntry(xmlNode* entry_node, std::unique_ptr<OWS::OWSEntry>& owsEntry) {
  FOR(inner_cur_node, entry_node) {
    if (inner_cur_node->type == XML_COMMENT_NODE) {
      continue;
    } else if (inner_cur_node->type == XML_ELEMENT_NODE) {
      if (IS_CHECK(inner_cur_node, "identifier", XMLNS_PURL)) {
        // set /entry/identifier/
        owsEntry->setPackageIdentifier(
            std::string((char*)xmlNodeGetContent(inner_cur_node)));

      } else if (IS_CHECK(inner_cur_node, "offering", XMLNS_OWC)) {
        auto ptrOffering = std::make_unique<OWS::OWSOffering>();

        parseOffering(inner_cur_node, ptrOffering);

        for (auto& content : ptrOffering->getContents()) {
          if (content.type == XML_CWL_TYPE) {
            if (!content.href.empty()){

              auto res = getFromWeb(content.tag, content.href.c_str());
              if (res != 200) {
                std::string err{"href: "};
                err.append(content.href).append(" can't be downloaded");
                throw std::runtime_error(err);
              }
            }

          }
        }

        if (ptrOffering->getCode() == OFFERING_CODE_CWL) {
          parserOfferingCWL(ptrOffering);
        }
        owsEntry->moveAddOffering(ptrOffering);
      }
    }
  }
}

OWS::OWSContext* Parser::parseXml(const char* bufferXml, int size) {
  int ret = 0;
  xmlDoc* doc = nullptr;
  xmlNode* root_element = nullptr;
  LIBXML_TEST_VERSION
  int option = XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NOBLANKS |
               XML_PARSE_NOBLANKS;

  std::unique_ptr<xmlDoc, decltype(&xmlFreeDoc)> pDoc{
      xmlReadMemory(bufferXml, size, nullptr, nullptr, option), &xmlFreeDoc};

  auto owsContext = std::make_unique<OWS::OWSContext>();

  try {
    if (pDoc == nullptr) {
      ret = 2;
    } else {
      root_element = xmlDocGetRootElement(pDoc.get());
      for (xmlNode* cur_node = root_element; cur_node;
           cur_node = cur_node->next) {
        if (cur_node->type == XML_COMMENT_NODE) {
          continue;
        }

        if (IS_CHECK(cur_node, "feed", XMLNS_ATOM)) {
          if (cur_node->children != nullptr) {
            FOR(inner_entry_node, cur_node) {
              if (inner_entry_node->type == XML_COMMENT_NODE) {
                continue;
              } else if (inner_entry_node->type == XML_ELEMENT_NODE) {
                if (IS_CHECK(inner_entry_node, "entry", XMLNS_ATOM)) {
                  auto owsEntry = std::make_unique<OWS::OWSEntry>();
                  parseEntry(inner_entry_node, owsEntry);
                  owsContext->moveAddEntry(owsEntry);
                }
              }
            }
          }
        }
      }
    }

  } catch (std::runtime_error& err) {
    std::cerr << err.what() << "\n";
    owsContext.reset(nullptr);
  } catch (...) {
    std::cerr << "CATCH!!!\n";
    owsContext.reset(nullptr);
  }

  xmlCleanupParser();

  return owsContext.release();
}

Parser::~Parser() {}

}  // namespace EOEPCA
