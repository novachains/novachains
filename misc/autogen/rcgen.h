#ifndef RCGen_H
#define RCGen_H

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

using namespace std;

// xml source file
#define RCXMLSRC "rclist.xml"
// output file path
#define CPPPATH "../../src/oss/ossErr.cpp"
#define CPATH "../../src/include/ossErr.h"
#define JSPATH "../../src/spt/error.js"
#define RC_MDPATH "../../doc/src/document/reference/nova_error_code.md"
// XML element
#define CONSLIST "rclist.conslist"
#define CODELIST "rclist.codelist"
#define NAME "name"
#define VALUE "value"
#define DESCRIPTION "description"
#define RESERVED_ERROR "SDB_ERROR_RESERVED"

struct ErrorCode
{
   string name ;
   string desc_en ;
   string desc_cn ;
   int    value ;
   bool   reserved ;

   ErrorCode():
    value(0), reserved(false)
   {
   }

   string getDesc(string lang) const
   {
      if ("cn" == lang)
      {
          return desc_cn ;
      }
      else if ("en" == lang)
      {
          return desc_en ;
      }
      else
      {
          return "invalid lang" ;
      }
   }
} ;

class RCGen
{
private:
    const char* language;
    std::vector<std::pair<std::string, int> > conslist;
    std::vector<ErrorCode> errcodes;
    int maxErrorNameWidth;
    void loadFromXML ();
    void genC ();
    void genCPP ();
    void genJS () ;

public:
    RCGen (const char* lang);
    void run ();
    void genDoc () ;

};
#endif

