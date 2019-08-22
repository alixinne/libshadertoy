#ifndef _API_HPP_
#define _API_HPP_

#include <shadertoy.hpp>

#include <curl/curl.h>
#include <json/json.h>

int load_remote(shadertoy::render_context &context, shadertoy::swap_chain &chain,
				const shadertoy::rsize &size, const std::string &shaderId, const std::string &shaderApiKey);

Json::Value json_get(CURL *curl, const std::string &url);

#endif /* _API_HPP_ */
