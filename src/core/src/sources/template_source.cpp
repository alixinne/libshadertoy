#include "shadertoy/sources/template_source.hpp"

using namespace shadertoy;
using namespace shadertoy::sources;

template_source::template_source(std::shared_ptr<compiler::program_template> prog_template,
								 std::map<GLenum, std::vector<std::unique_ptr<compiler::basic_part>>> parts)
: template_(prog_template), parts_(std::move(parts))
{
}

std::vector<std::pair<std::string, std::string>> template_source::get_source(GLenum shader_type) const
{
	auto it = parts_.find(shader_type);
	if (it == parts_.end())
		return template_->get_source(shader_type, {});
	return template_->get_source(shader_type, it->second);
}

std::shared_ptr<backends::gx::shader> template_source::get_precompiled(GLenum shader_type) const
{
	return template_->get_precompiled(shader_type);
}

bool template_source::has_source(GLenum shader_type) const
{
	return template_->has_shader(shader_type);
}
