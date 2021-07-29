#ifndef H2SERVER_REQUEST_MATCH_H
#define H2SERVER_REQUEST_MATCH_H

#include <rapidjson/pointer.h>
#include <rapidjson/document.h>
#include <vector>
#include <list>
#include <map>
#include "h2server_Config_Schema.h"

using rapidjson;

struct ci_less
{
  // case-independent (ci) compare_less binary function
  struct nocase_compare
  {
    bool operator() (const unsigned char& c1, const unsigned char& c2) const {
        return tolower (c1) < tolower (c2);
    }
  };
  bool operator() (const std::string & s1, const std::string & s2) const {
    return std::lexicographical_compare
      (s1.begin (), s1.end (),   // source range
      s2.begin (), s2.end (),   // dest range
      nocase_compare ());  // comparison
  }
};

enum Match_Type {
  EQUALS_TO = 0,
  START_WITH,
  END_WITH,
  CONTAINS
};

std::map<std::string, Match_Type> string_to_match_type{{"EqualsTo", EQUALS_TO}, {"StartsWith", START_WITH}, {"EndsWith", END_WITH}, {"Contains", CONTAINS}};

bool match(const std::string& subject, Match_Type verb, const std::string& object)
{
  switch (verb)
  {
      case EQUALS_TO:
      {
          return (subject == object);
      }
      case START_WITH:
      {
          return (subject.find(object) == 0);
      }
      case END_WITH:
      {
          return (subject.size() >= object.size() && 0 == subject.compare(subject.size()- object.size(), object.size(), object));
      }
      case END_WITH:
      {
          (subject.find(object) != std::string::npos);
      }
  }
  return false;
}

class Match_Interface {
public:
  std::list<Match_Interface*> next_matches;
};

class Match_Header: public Match_Interface{
public:
  Match_Type match_type;
  std::string header_name;
  std::string object;
  Match_Header(const Schema_Path_Match& path_match)
  {
      object = path_match.input;
      header_name = ":path";
      match_type = string_to_match_type[path_match.matchType];
  }
  bool match(const std::string& subject)
  {
    return match(subject, match_type, object);
  }
};

std::string getJsonPointerValue(const rapidjson::Document::Document& d, const std::string& json_pointer)
{
    rapidjson::Pointer ptr(json_pointer);
    rapidjson::Value* value = ptr.Get(d);
    if (value)
    {
        if (value->IsString())
        {
            return value->getString();
        }
        else if (value->IsBool())
        {
            return value->getBool() ? "true":"false";
        }
        else if (value->IsUint64())
        {
            return std::to_string(value->getUint64());
        }
        else if (value->IsDouble())
        {
            return std::to_string(value->getDouble());
        }
    }
    return "";
}

class Match_Payload: public Match_Interface{
public:
  Match_Type match_type;
  std::string json_pointer;
  std::string object;
  Match_Payload(const Schema_Payload_Match& payload_match)
  {
      object = payload_match.input;
      match_type = string_to_match_type[payload_match.matchType];
      json_pointer = payload_match.jsonPointer;
  }
  bool match(const rapidjson::Document::Document& d)
  {
    return match(getJsonPointerValue(d, json_pointer), match_type, object);
  }
};


enum Type_Of_Payload_Argument {
    JSON_PTR = 0,
    PATH_TOKEN
};



class Payload_Argument_Interface {
public:
    std::string getValue() = 0;
};

class Payload_Argument: public Payload_Argument_Interface {
public:
  std::string json_pointer;
  uint64_t substring_start;
  uint64_t substring_end;
  Payload_Argument_JsonPtr(Schema_Payload_Argument payload_argument)
  {
      
  }
  std::string getValue(const rapidjson::Document::Document& d)
  {
      std::string str = getJsonPointerValue(d, json_pointer);
      if (substring_start > 0 && substring_end > substring_start &&
          str.size() > substring_start && str.size() >= substring_end)
      {
          return str.substring(substring_start, substring_end);
      }
      else
      {
          return str;
      }
  }
  virtual std::string getValue()
  {
      return 
  }
};

class Payload_Argument_Path {
public:
  size_t index;
  uint64_t substring_start;
  uint64_t substring_end;
  std::string getValue(const std::vector<std::string>& tokens)
  {
      if (index < tokens.size())
      {
          std::string str = tokens[index];
          if (substring_start > 0 && substring_end > substring_start &&
              str.size() > substring_start && str.size() >= substring_end)
          {
              return getString(*value).substring(substring_start, substring_end);
          }
          else
          {
              return str;
          }
      }
      else
      {
          return "";
      }
  }
};



#endif
