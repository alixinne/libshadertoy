#ifndef _SHADERTOY_CONTEXT_CONFIG_HPP_
#define _SHADERTOY_CONTEXT_CONFIG_HPP_

#include "shadertoy/pre.hpp"

namespace shadertoy
{

/**
 * @brief      Holds the configuration for a ShaderToy rendering context.
 */
struct shadertoy_EXPORT context_config
{
	/// Size of the rendering
	rsize render_size;

	/// Target framerate for the rendering (in frames per second)
	double target_framerate;

	/// List of pre-processor definitions to be compiled with the programs
	std::vector<std::pair<std::string, std::string>> preprocessor_defines;

	/// List of configured buffers for this context
	std::vector<std::pair<std::string, buffer_config>> buffer_configs;
};
}

#endif /* _SHADERTOY_CONTEXT_CONFIG_HPP_ */
