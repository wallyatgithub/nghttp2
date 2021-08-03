#ifndef H2SERVER_H
#define H2SERVER_H

#include <map>
#include "h2server_Request_Match.h"
#include "h2server_Message.h"

class H2Server_Service
{
public:
    H2Server_Request request;
    H2Server_Response response;
    H2Server_Service(const Schema_Service& service):
        request(service.request),
        response(service.response)
    {
    }
};

class H2Server {
public:
    std::map<H2Server_Request, H2Server_Response> services;
    
    void build_match_rule_unique_id(std::map<H2Server_Request, H2Server_Response>& services)
    {
        std::set<Match_Rule> all_match_rules;
        for (auto& each_service : services)
        {
            for (auto& match_rule : each_service.first.match_rules)
            {
                all_match_rules.insert(match_rule);
            }
        }
    
        std::map<H2Server_Request, H2Server_Response> new_services;
        for (auto each_service : services)
        {
            for (auto& match_rule : each_service.first.match_rules)
            {
                match_rule.unique_id = std::distance(all_match_rules.begin(), all_match_rules.find(match_rule));
            }
            new_services.insert(std::make_pair(each_service.first, each_service.second));
        }
        services.swap(new_services);
    }

    H2Server(const H2Server_Config_Schema& config_schema)
    {
        for (auto& service_in_config_schema: config_schema.service)
        {
            H2Server_Service service(service_in_config_schema);
            services.insert(std::make_pair(service.request, service.response));
        }
        build_match_rule_unique_id(services);
    }

    const H2Server_Response* get_response_to_return(H2Server_Request_Message& msg) const
    {
        for (auto iter = services.rbegin(); iter != services.rend(); iter++)
        {
            if (iter->first.match(msg))
            {
                return &iter->second;
            }
        }
        return nullptr;
    }

};


#endif

