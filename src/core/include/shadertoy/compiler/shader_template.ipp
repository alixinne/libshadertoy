#ifndef _SHADERTOY_COMPILER_SHADER_TEMPLATE_IPP_
#define _SHADERTOY_COMPILER_SHADER_TEMPLATE_IPP_

#include <map>

namespace shadertoy
{
namespace compiler
{

template <typename PartCallback>
shader_template
shader_template::specify_parts(const std::vector<std::unique_ptr<basic_part>> &parts, PartCallback pc) const
{
	// Create a map of new parts
	std::map<std::string, const std::unique_ptr<basic_part> *> new_parts;
	for (auto &part : parts)
		new_parts.emplace(part->name(), &part);

	// New list of parts
	std::deque<std::unique_ptr<basic_part>> specified_parts;

	for (auto &current_part : parts_)
	{
		if (!current_part->is_specified())
		{
			// current_part is not specified yet
			auto new_part_it = new_parts.find(current_part->name());

			if (new_part_it != new_parts.end())
			{
				// We have a specification for this part
				specified_parts.emplace_back(std::move((*new_part_it->second)->clone()));
			}
			else
			{
				auto cb_part(pc(current_part->name()));
				if (!cb_part.empty())
				{
					// Add the callback part
					std::move(cb_part.begin(), cb_part.end(), std::back_inserter(specified_parts));
				}
				else
				{
					// Keep the unspecified part
					specified_parts.emplace_back(current_part->clone());
				}
			}
		}
		else
		{
			// This part has been specified already
			specified_parts.emplace_back(current_part->clone());
		}
	}

	return shader_template(std::move(specified_parts));
}
} // namespace compiler
} // namespace shadertoy

#endif /* _SHADERTOY_COMPILER_SHADER_TEMPLATE_IPP_ */
