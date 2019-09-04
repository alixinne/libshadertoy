#ifndef _SHADERTOY_SOURCES_FILE_SOURCE_HPP_
#define _SHADERTOY_SOURCES_FILE_SOURCE_HPP_

#include "shadertoy/sources/basic_source.hpp"

#include <map>

namespace shadertoy
{
namespace sources
{
/**
 * @brief Loads raw files as shader sources
 */
class shadertoy_EXPORT file_source : public basic_source
{
	/// Map of filenames
	std::map<GLenum, std::string> files_;

	public:
	/**
	 * @brief Create a new file source
	 *
	 * @param files List of (type, filename) pairs describing files to be loaded
	 */
	file_source(std::map<GLenum, std::string> files);

	std::vector<std::pair<std::string, std::string>> get_source(GLenum shader_type) const final;

	bool has_source(GLenum shader_type) const final;
};
} // namespace sources
} // namespace shadertoy

#endif
