#ifndef H2SERVER_REQUEST_MATCH_H
#define H2SERVER_REQUEST_MATCH_H

#include <rapidjson/pointer.h>
#include <rapidjson/document.h>
#include <vector>
#include <list>
#include <map>
#include "h2server_Config_Schema.h"
#include "h2server_Resonse.h"
#include "h2server_Message.h"


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
  H2Server_Response* response_to_return;
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

class H2Server_Request {
public:
  Match_Header match_header;
  std::vector<Match_Payload> match_payload;
  H2Server_Request(const Schema_Request_Match& request_match):
  match_header(request_match.path_match)
  {
      for (auto& schema_payload_match: request_match.payload_match)
      match_payload.emplace_back(Match_Payload(schema_payload_match));
  }
  bool match(const H2Server_Request_Message& request)
  {
      if (!match_header.match(request.path))
      {
          return false;
      }
      for (auto& payload_match: match_payload)
      {
          if (!payload_match.match(request.json_payload))
          {
              return false;
          }
      }
  }
};


class H2Server_Service {
public:
  H2Server_Request request;
  H2Server_Response response;
  H2Server_Service(const Schema_Service& service):
  request(service.request),
  response(service.response)
  {
  }
};


#endif
