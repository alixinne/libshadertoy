#ifndef _SHADERTOY_SOURCES_STACK_SOURCE_HPP_
#define _SHADERTOY_SOURCES_STACK_SOURCE_HPP_

#include "shadertoy/sources/basic_source.hpp"

#include <memory>
#include <vector>

namespace shadertoy
{
namespace sources
{
/**
 * @brief Uses a stack of sources to provide shader sources
 *
 * Elements higher in the stack override shader sources from lower
 * elements in the stack.
 */
class shadertoy_EXPORT stack_source : public basic_source
{
	/// List of sources
	std::vector<std::unique_ptr<basic_source>> sources_;

	public:
	/**
	 * @brief Create a new stack_source
	 *
	 * @param sources List of sources to wrap
	 */
	stack_source(std::vector<std::unique_ptr<basic_source>> sources);

	stack_source(const stack_source &) = delete;
	stack_source &operator=(const stack_source &) = delete;

	std::vector<std::pair<std::string, std::string>> get_source(GLenum shader_type) const final;

	std::shared_ptr<backends::gx::shader> get_precompiled(GLenum shader_type) const final;

	bool has_source(GLenum shader_type) const final;
};
} // namespace sources
} // namespace shadertoy

#endif /* _SHADERTOY_SOURCES_STACK_SOURCE_HPP_ */
