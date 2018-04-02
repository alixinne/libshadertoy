#ifndef _API_HPP_
#define _API_HPP_

#include <shadertoy.hpp>

#include <curl/curl.h>
#include <json/json.h>

int load_remote(shadertoy::context_config &ctx_config, const std::string &shaderId, const std::string &shaderApiKey);

Json::Value json_get(CURL *curl, const std::string &url);

#endif /* _API_HPP_ */
