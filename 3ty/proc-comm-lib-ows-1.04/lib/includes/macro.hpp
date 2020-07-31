#ifndef EOEPCAOWS_MACRO_HPP
#define EOEPCAOWS_MACRO_HPP

#define FOR(NEW, CURRENT) \
  for (xmlNode *NEW = CURRENT->children; NEW; NEW = NEW->next)
#define XMLNS_ATOM "http://www.w3.org/2005/Atom"
#define XMLNS_OWC "http://www.opengis.net/owc/1.0"
#define XMLNS_WPS2 "http://www.opengis.net/wps/2.0"
#define XMLNS_WPS1 "http://www.opengis.net/wps/1.0.0"
#define XMLNS_OWS "http://www.opengis.net/ows/1.1"
#define XMLNS_PURL "http://purl.org/dc/elements/1.1/"
#define XMLNS_STAC "http://www.me.net/stac/cwl/extension"
#define XMLNS_OPENSEARCH "http://a9.com/-/spec/opensearch/1.1/"

#define XML_DOCKER_TYPE "application/vnd.docker.distribution.manifest.v1+json"
#define XML_CWL_TYPE "application/cwl"

#define XML_COMPARE(X1, CHAR) (!xmlStrcmp(X1, (const xmlChar *)CHAR))
#define OFFERING_CODE "http://www.opengis.net/tb13/eoc/wpsProcessOffering"
#define OFFERING_CODE_CWL "http://www.opengis.net/eoc/applicationContext/cwl"
#define IS_CHECK(NODE, V, NS)                    \
  (!xmlStrcmp(NODE->name, (const xmlChar *)V) && \
   !xmlStrcmp(NODE->ns->href, (const xmlChar *)NS))
#define CHAR_BAD_CAST (char *)

#define CWL_STACF_ORMAT_DEFAULT "application/geo+json"

#define FNCMAP(N, T) #N, std::bind(&T, std::placeholders::_1)
#define FNCMAPS(N, T) \
  N, std::bind(&T, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

#define CWL_NAMESPACE \
  { XMLNS_STAC, XMLNS_WPS1 }

#define CWLTYPE_LIST                                                       \
  {                                                                        \
    "null", "boolean","integer", "int", "long", "float", "double", "string", "File", \
        "Directory"                                                        \
  }

#define IS_INPUT(NODE, NS, CHAR) \
  IS_CHECK(NODE, CHAR, NS) || XML_COMPARE(NODE->name, CHAR)

#define IS_FEED(D)                                    \
  if (!xmlStrcmp(D->name, (const xmlChar *)"feed") && \
      !xmlStrcmp(D->ns->href, (const xmlChar *)XMLNS_ATOM))
#define IS_OFFERING(D)                                    \
  if (!xmlStrcmp(D->name, (const xmlChar *)"offering") && \
      !xmlStrcmp(D->ns->href, (const xmlChar *)XMLNS_OWC))
#define IS_OFFERING_CODE(D) \
  if (D != NULL && !xmlStrcmp(D, (const xmlChar *)OFFERING_CODE))
#define IS_CONTENT(D)                                    \
  if (!xmlStrcmp(D->name, (const xmlChar *)"content") && \
      !xmlStrcmp(D->ns->href, (const xmlChar *)XMLNS_OWC))
#define IS_OPERATION(D)                                    \
  if (!xmlStrcmp(D->name, (const xmlChar *)"operation") && \
      !xmlStrcmp(D->ns->href, (const xmlChar *)XMLNS_OWC))
#define IS_PROCESSOFFERINGS(D)                                    \
  if (!xmlStrcmp(D->name, (const xmlChar *)"ProcessOfferings") && \
      !xmlStrcmp(D->ns->href, (const xmlChar *)XMLNS_WPS))
#define IS_PROCESSOFFERING(D)                                    \
  if (!xmlStrcmp(D->name, (const xmlChar *)"ProcessOffering") && \
      !xmlStrcmp(D->ns->href, (const xmlChar *)XMLNS_WPS))
#define IS_PROCESS(D)                                    \
  if (!xmlStrcmp(D->name, (const xmlChar *)"Process") && \
      !xmlStrcmp(D->ns->href, (const xmlChar *)XMLNS_WPS))

#define IS_IDENTIFIER(D)                                    \
  if (!xmlStrcmp(D->name, (const xmlChar *)"Identifier") && \
      !xmlStrcmp(D->ns->href, (const xmlChar *)XMLNS_OWS))
#define IS_ENTRY(D)                                    \
  if (!xmlStrcmp(D->name, (const xmlChar *)"entry") && \
      !xmlStrcmp(D->ns->href, (const xmlChar *)XMLNS_ATOM))
#define IS_ENTRYIDENTIFIER(D)                               \
  if (!xmlStrcmp(D->name, (const xmlChar *)"identifier") && \
      !xmlStrcmp(D->ns->href, (const xmlChar *)XMLNS_PURL))
#define IS_PARSERDESCRIPTOR(D)                                      \
  if (!xmlStrcmp(D->name, (const xmlChar *)"ProcessDescription") && \
      !xmlStrcmp(D->ns->href, (const xmlChar *)XMLNS_ATOM))

// if (IS_INPUT(input, XMLNS_WPS1, "LiteralData")) {
// param = parseLiteralData(input);
//} else if (IS_INPUT(input, XMLNS_WPS1, "BoundingBoxData")) {
// param = parseBoundingBoxData(input);
//} else if (IS_INPUT(input, XMLNS_WPS1, "ComplexData")) {
// param = parseComplexData(input);
//}

#endif  // EOEPCAOWS_MACRO_HPP