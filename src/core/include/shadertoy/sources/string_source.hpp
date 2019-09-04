#ifndef _SHADERTOY_SOURCES_STRING_SOURCE_HPP_
#define _SHADERTOY_SOURCES_STRING_SOURCE_HPP_

#include "shadertoy/sources/basic_source.hpp"

#include <map>

namespace shadertoy
{
namespace sources
{
/**
 * @brief Loads raw strings as shader sources
 */
class shadertoy_EXPORT string_source : public basic_source
{
	/// Map of string and names
	std::map<GLenum, std::pair<std::string, std::string>> strings_;

	public:
	/**
	 * @brief Create a new string source
	 *
	 * @param strings List of (type, strings) pairs describing sources
	 */
	string_source(std::map<GLenum, std::string> strings);

	/**
	 * @brief Create a new string source
	 *
	 * @param strings List of (type, (name, strings)) pairs describing named sources
	 */
	string_source(std::map<GLenum, std::pair<std::string, std::string>> strings);

	std::vector<std::pair<std::string, std::string>> get_source(GLenum shader_type) const final;

	bool has_source(GLenum shader_type) const final;
};
} // namespace sources
} // namespace shadertoy

#endif /* _SHADERTOY_SOURCES_STRING_SOURCE_HPP_ */
