#include <cassert>
#include <sstream>

#include "shadertoy/compiler/define_part.hpp"

using namespace shadertoy::compiler;

preprocessor_defines::preprocessor_defines()
	: generated_source_(),
	source_ready_(true),
	definitions_()
{
}

const std::string &preprocessor_defines::source()
{
	if (!source_ready_)
	{
		std::ostringstream oss;
		for (auto define : definitions_)
		{
			oss << "#define " << define.first;
			if (!define.second.empty())
			{
				oss << " " << define.second;
			}
			oss << std::endl;
		}
		generated_source_ = oss.str();
		source_ready_ = true;
	}

	return generated_source_;
}

define_part::define_part(const std::string &name)
	: basic_part(name),
	definitions_(std::make_shared<preprocessor_defines>())
{
}

define_part::define_part(const std::string &name, std::shared_ptr<preprocessor_defines> defines)
	: basic_part(name),
	definitions_(defines)
{
	assert(defines);
}

define_part::operator bool() const
{
	return true;
}

std::vector<std::pair<std::string, std::string>> define_part::sources() const
{
	return { std::make_pair(name(), definitions_->source()) };
}
