#ifndef H2SERVER_REQUEST_MATCH_H
#define H2SERVER_REQUEST_MATCH_H

#include <rapidjson/pointer.h>
#include <rapidjson/document.h>
#include <vector>
#include <list>
#include <map>
#include <set>
#include "h2server_Config_Schema.h"
#include "h2server_Resonse.h"
#include "h2server_Message.h"


using rapidjson;

class Match_Interface {
public:
  std::list<Match_Interface*> next_matches;
  H2Server_Response* response_to_return;
};

class Match_Rule: public Match_Interface{
public:
  Match_Type match_type;
  std::string header_name;
  std::string json_pointer;
  std::string object;
  Match_Rule(const Schema_Path_Match& path_match)
  {
      object = path_match.input;
      header_name = ":path";
      json_pointer = "";
      match_type = string_to_match_type[path_match.matchType];
  }
  Match_Rule(const Schema_Payload_Match& payload_match)
  {
      object = payload_match.input;
      header_name = "";
      json_pointer = payload_match.jsonPointer;
      match_type = string_to_match_type[path_match.matchType];
  }

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

  bool match(const std::string& subject)
  {
      return match(subject, match_type, object);
  }
  
  bool match(const rapidjson::Document::Document& d)
  {
      return match(getJsonPointerValue(d, json_pointer), match_type, object);
  }

  bool match(const H2Server_Request_Message& request)
  {
      return (header_name.size() ? match(request.path) : match(request.json_payload));
  }

  bool operator<(const Match_Header& rhs) const 
  {
     if (!header_name.empty() && rhs.header_name.empty())
     {
        return true;
     }
     else if (header_name.empty() && !rhs.header_name.empty())
     {
        return false;
     }

     if (match_type < rhs.match_type)
     {
        return true;
     }
     if (header_name < rhs.header_name)
     {
        return true;
     }
     if (json_pointer < rhs.json_pointer)
     {
        return true;
     }
     if (object < rhs.object)
     {
        return true;
     }
     return false;
  }

};

class H2Server_Request {
public:
  std::set<Match_Rule> match_rules;
  H2Server_Request(const Schema_Request_Match& request_match):
  {
      match_rules.emplace_back(Match_Rule(request_match.path_match));
      for (auto& schema_payload_match: request_match.payload_match)
      {
          match_rules.emplace(Match_Rule(schema_payload_match));
      }
  }
  bool match(const H2Server_Request_Message& request)
  {
      for (auto& match: match_rules)
      {
          if (!match.match(request))
          {
              return false;
          }
      }
  }
  bool operator<(const H2Server_Request& rhs) const 
  {
      if (match_rules.size() < rhs.rhs.size())
      {
          return true;
      }
      else if (match_rules.size() > rhs.match_rules.size())
      {
          return false;
      }
      else
      {
          auto& match = match_rules.begin();
          auto& other_match = rhs.match_rules.begin();
          while (match != match_rules.end() && other_match != rhs.match_rules.end())
          {
              if (*match < *other_match)
              {
                  return true;
              }
              else
              {
                  match++;
                  other_match++;
              }
          }
      }
      return false;
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

Match_Interface* find_or_insert_into_match_tree(std::vector<Match_Interface*>& tree, )
{
    
}


void build_match_tree(const H2Server_Config_Schema& config_schema)
{
    std::vector<Match_Interface*> tree;

    std::vector<H2Server_Service> services;

    for (auto& service_schema: config_schema.service)
    {
        services.emplace_back(H2Server_Service(service_schema));
    }

    for (auto& service: services)
    {
        
    }
}

#endif
