#ifndef PROC_COMM_ZOO_SVC_SHA1_HPP
#define PROC_COMM_ZOO_SVC_SHA1_HPP

#include <string>

class sha1 {
 public:
  sha1(std::string val);
  virtual ~sha1();

  std::string parse();
  int parseFile(std::string& buffer);

  static std::string parseString(std::string str);

 public:
  void setBuffer(unsigned int bufferLen);

 private:
  std::string mVal{""};
  sha1() = default;
  unsigned int bufferLen{0};
};



#endif
