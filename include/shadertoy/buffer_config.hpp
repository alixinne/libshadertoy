#ifndef _SHADERTOY_BUFFER_CONFIG_HPP_
#define _SHADERTOY_BUFFER_CONFIG_HPP_

#include "shadertoy/pre.hpp"

namespace shadertoy
{

/**
 * @brief      Represents the configuration of a ToyBuffer channel input
 */
struct shadertoy_EXPORT InputConfig {
  /// Input identifier, in the form <buffer>.<input-id>
  std::string id;

  /// Type of the actual input
  std::string type;

  /// Source for this input (texture path, or procedural parameters)
  std::string source;

  /// true to flip vertically when loading texture
  bool vflip;

  /// Texture wrap mode
  GLint wrap;

  /// Texture minification filtering mode
  GLint minFilter;

  /// Texture magnification filtering mode
  GLint magFilter;

  /**
   * @brief      Returns a value indicating if this input is enabled
   *
   * @return     true if this input is enabled
   */
  bool enabled() const;

  /**
   * @brief      Initialize an empty input config
   */
  InputConfig();
};

/**
 * @brief      Represents the configuration of a ToyBuffer
 */
struct shadertoy_EXPORT buffer_config
{
	/// Name of the current buffer
	std::string name;

	/// Path to the shader files to load
        std::vector<boost::filesystem::path> shaderFiles;

        /// Specification of inputs for this buffer
        InputConfig inputConfig[4];
};

}

#endif /* _SHADERTOY_BUFFER_CONFIG_HPP_ */
