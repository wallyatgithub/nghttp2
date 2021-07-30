#ifndef H2SERVER_MESSAGE_H
#define H2SERVER_MESSAGE_H

#include <rapidjson/pointer.h>
#include <rapidjson/document.h>
#include <vector>
#include <list>
#include <map>
#include <nghttp2/asio_http2_server.h>

#include "h2server_Config_Schema.h"


using rapidjson;

class H2Server_Request_Message {
public:
  std::string path;
  std::multimap<std::string, std::string> headers;
  rapidjson::Document::Document json_payload;
  H2Server_Request_Message(const request &req)
  {
      path = req.uri().path;
      json_payload.Parse(req.payload().c_str());
  }
};

#endif

