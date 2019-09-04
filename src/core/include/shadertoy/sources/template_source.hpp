#ifndef _SHADERTOY_SOURCES_TEMPLATE_SOURCE_HPP_
#define _SHADERTOY_SOURCES_TEMPLATE_SOURCE_HPP_

#include "shadertoy/program_input.hpp"
#include "shadertoy/sources/basic_source.hpp"

#include "shadertoy/compiler/file_part.hpp"
#include "shadertoy/compiler/input_part.hpp"
#include "shadertoy/compiler/template_part.hpp"
#include "shadertoy/compiler/program_template.hpp"

#include <deque>
#include <memory>

namespace shadertoy
{
namespace sources
{
/**
 * @brief Represents shader sources based on a template
 */
class shadertoy_EXPORT template_source : public basic_source
{
	/// Pointer to the program template to use
	std::shared_ptr<compiler::program_template> template_;

	/// Map of parts to fully specify the template
	std::map<GLenum, std::vector<std::unique_ptr<compiler::basic_part>>> parts_;

	public:
	template_source(std::shared_ptr<compiler::program_template> prog_template,
					std::map<GLenum, std::vector<std::unique_ptr<compiler::basic_part>>> parts);

	template_source(const template_source &) = delete;
	template_source &operator=(const template_source &) = delete;

	std::vector<std::pair<std::string, std::string>> get_source(GLenum shader_type) const final;

	std::shared_ptr<backends::gx::shader> get_precompiled(GLenum shader_type) const final;

	bool has_source(GLenum shader_type) const final;
};

/**
 * @brief Load a source string as a part for a buffer, using a template
 *
 * @param buffer        Buffer to set the source on
 * @param prog_template Template to use for the buffer
 * @param stage         Shader stage of this source
 * @param source        Source code to use for the target buffer
 */
template <typename Buffer>
void set_source(Buffer &buffer, std::shared_ptr<compiler::program_template> prog_template,
					 GLenum stage, const std::string &source)
{
	std::vector<std::unique_ptr<compiler::basic_part>> stage_parts;
	stage_parts.emplace_back(std::make_unique<compiler::input_part>("buffer:inputs", buffer.inputs()));
	stage_parts.emplace_back(std::make_unique<compiler::template_part>("buffer:sources", source));

	std::map<GLenum, std::vector<std::unique_ptr<compiler::basic_part>>> parts;
	parts.emplace(stage, std::move(stage_parts));

	buffer.source(std::make_unique<sources::template_source>(
	std::forward<std::shared_ptr<compiler::program_template>>(prog_template), std::move(parts)));
}

/**
 * @brief Load a source file as a part for a buffer, using a template
 *
 * @param buffer        Buffer to set the source on
 * @param prog_template Template to use for the buffer
 * @param stage         Shader stage of this source
 * @param path          Path to the source file to load
 */
template <typename Buffer>
void set_source_file(Buffer &buffer, std::shared_ptr<compiler::program_template> prog_template,
					 GLenum stage, const std::string &path)
{
	std::vector<std::unique_ptr<compiler::basic_part>> stage_parts;
	stage_parts.emplace_back(std::make_unique<compiler::input_part>("buffer:inputs", buffer.inputs()));
	stage_parts.emplace_back(std::make_unique<compiler::file_part>("buffer:sources", path));

	std::map<GLenum, std::vector<std::unique_ptr<compiler::basic_part>>> parts;
	parts.emplace(stage, std::move(stage_parts));

	buffer.source(std::make_unique<sources::template_source>(
	std::forward<std::shared_ptr<compiler::program_template>>(prog_template), std::move(parts)));
}
} // namespace sources
} // namespace shadertoy

#endif /* _SHADERTOY_SOURCES_TEMPLATE_SOURCE_HPP_ */
