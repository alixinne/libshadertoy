#include <sstream>

#include "shadertoy/compiler/define_part.hpp"

#include "shadertoy/utils/assert.hpp"

using namespace shadertoy::compiler;

using shadertoy::utils::error_assert;

preprocessor_defines::preprocessor_defines() : source_ready_(true) {}

const std::string &preprocessor_defines::source()
{
	if (!source_ready_)
	{
		std::ostringstream oss;
		for (const auto &define : definitions_)
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
	: cloneable_part(name),
	definitions_(std::make_shared<preprocessor_defines>())
{
}

define_part::define_part(const std::string &name, const std::shared_ptr<preprocessor_defines> &defines)
: cloneable_part(name), definitions_(defines)
{
	error_assert(defines.get() != nullptr, "The defines pointer cannot be null");
}

define_part::operator bool() const
{
	return true;
}

std::vector<std::pair<std::string, std::string>> define_part::sources() const
{
	return { std::make_pair(name(), definitions_->source()) };
}
