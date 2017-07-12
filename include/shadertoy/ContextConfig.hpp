#ifndef _SHADERTOY_CONTEXT_CONFIG_HPP_
#define _SHADERTOY_CONTEXT_CONFIG_HPP_

#include "shadertoy/pre.hpp"

namespace shadertoy
{

/**
 * @brief      Holds the configuration for a ShaderToy rendering context.
 */
struct ContextConfig
{
	/// Width of the rendering
	int width;

	/// Height of the rendering
	int height;

	/// Target framerate for the rendering (in frames per second)
	double targetFramerate;

	/// List of pre-processor definitions to be compiled with the programs
	std::vector<std::pair<std::string, std::string>> preprocessorDefines;

	/// List of configured buffers for this context
	std::map<std::string, BufferConfig> bufferConfigs;
};
}

#endif /* _SHADERTOY_CONTEXT_CONFIG_HPP_ */
