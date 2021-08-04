#ifndef H2SERVER_RESPONSE_H
#define H2SERVER_RESPONSE_H

#include <rapidjson/pointer.h>
#include <rapidjson/document.h>
#include <vector>
#include <list>
#include <map>
#include "h2server_Config_Schema.h"
#include "h2server_Message.h"

using namespace rapidjson;

std::string getJsonPointerValue(const rapidjson::Document& d, const std::string& json_pointer)
{
    rapidjson::Pointer ptr(json_pointer.c_str());
    auto value = ptr.Get(d);
    if (value)
    {
        if (value->IsString())
        {
            return value->GetString();
        }
        else if (value->IsBool())
        {
            return value->GetBool() ? "true" : "false";
        }
        else if (value->IsUint64())
        {
            return std::to_string(value->GetUint64());
        }
        else if (value->IsDouble())
        {
            return std::to_string(value->GetDouble());
        }
    }
    return "";
}

std::vector<std::string> tokenize_string(const std::string& source, const std::string& delimeter)
{
    std::vector<std::string> retVec;
    size_t start = 0;
    size_t delimeter_len = delimeter.length();
    if (!delimeter.empty())
    {
        size_t pos = source.find(delimeter, start);
        while (pos != std::string::npos)
        {
            retVec.emplace_back(source.substr(start, (pos - start)));
            start = pos + delimeter_len;
            pos = source.find(delimeter, start);
        }
        retVec.emplace_back(source.substr(start, std::string::npos));
    }
    else
    {
        retVec.emplace_back(source);
    }
    return retVec;
}

class Payload_Argument
{
public:
    std::string json_pointer;
    uint64_t index;
    uint64_t substring_start;
    uint64_t substring_end;
    std::string header_name;
    Payload_Argument(const Schema_Payload_Argument& payload_argument)
    {
        if (payload_argument.type_of_value == "JsonPointer")
        {
            json_pointer = payload_argument.value_identifier;
            index = 0;
        }
        else if (payload_argument.type_of_value == "TokenInPath")
        {
            index = std::stoi(payload_argument.value_identifier);
            json_pointer = "";
            header_name = ":path";
        }
        substring_start = payload_argument.substring_start;
        substring_end = payload_argument.substring_end;
    }
    std::string getValue(const H2Server_Request_Message& msg) const
    {
        std::string str;
        if (json_pointer.size())
        {
            str = getJsonPointerValue(msg.json_payload, json_pointer);
        }
        else if (msg.headers.count(header_name))
        {
            auto tokens = tokenize_string(msg.headers.find(header_name)->second, "/");
            if (index < tokens.size())
            {
                str = tokens[index];
            }
        }

        if (substring_start > 0 && substring_end > substring_start &&
            str.size() > substring_start && str.size() >= substring_end)
        {
            return str.substr(substring_start, substring_end);
        }
        else
        {
            return str;
        }
    }
};

class H2Server_Response
{
public:
    uint32_t status_code;
    std::map<std::string, std::string> additonalHeaders;
    std::vector<std::string> tokenizedPayload;
    std::vector<Payload_Argument> payload_arguments;

    H2Server_Response(const Schema_Response_To_Return& resp)
    {
        status_code = resp.status_code;
        tokenizedPayload = tokenize_string(resp.payload.msg_payload, resp.payload.placeholder);
        for (auto& arg : resp.payload.payload_argument)
        {
            payload_arguments.emplace_back(Payload_Argument(arg));
        }
        if (tokenizedPayload.size() - payload_arguments.size() != 1)
        {
            std::cerr << "number of placeholders does not match number of arguments:" << staticjson::to_pretty_json_string(
                          resp) << std::endl;
            abort();
        }
        for (auto& header_with_value : resp.additonalHeaders)
        {
            size_t t = header_with_value.find(":", 1);
            if ((t == std::string::npos) ||
                (header_with_value[0] == ':' && 1 == t))
            {
                std::cerr << "invalid header, no name: " << header_with_value << std::endl;
                continue;
            }
            std::string header_name = header_with_value.substr(0, t);
            std::string header_value = header_with_value.substr(t + 1);
            /*
            header_value.erase(header_value.begin(), std::find_if(header_value.begin(), header_value.end(),
                                                                  [](unsigned char ch)
            {
                return !std::isspace(ch);
            }));
            */

            if (header_value.empty())
            {
                std::cerr << "invalid header - no value: " << header_with_value
                          << std::endl;
                continue;
            }
            additonalHeaders[header_name] = header_value;
        }
    }

    std::string produce_payload(const H2Server_Request_Message& msg) const
    {
        std::string payload;
        for (size_t index = 0; index < tokenizedPayload.size(); index++)
        {
            payload.append(tokenizedPayload[index]);
            if (index < payload_arguments.size())
            {
                payload.append(payload_arguments[index].getValue(msg));
            }
        }
        return payload;
    }
};


#endif

