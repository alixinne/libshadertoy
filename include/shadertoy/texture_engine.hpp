#ifndef _SHADERTOY_TEXTURE_ENGINE_HPP_
#define _SHADERTOY_TEXTURE_ENGINE_HPP_

#include "shadertoy/pre.hpp"

namespace shadertoy
{

/**
 * @brief Handler callback definition for the texture engine. Instances of this method
 * are called when a texture is required for an input in a rendering context.
 *
 * `inputConfig` is the configuration object for the given input.
 * `skipTextureOptions` should be set to true if no wrapping or filtering
 * options should be automatically applied. `skipCache` should be set to true if
 * the method should be invoked each time a texture for this input is requested.
 * `framebufferSized` should be set to true if any change in framebuffer
 * resolution should require generating a new instance of this texture.
 */
typedef std::function<std::shared_ptr<gl::texture>(
	const input_config &inputConfig,
	bool &skipTextureOptions,
	bool &skipCache,
	bool &framebufferSized)> input_handler;

/**
 * @brief      Represents the engine responsible for loading input textures for
 *             the current rendering context.
 */
class shadertoy_EXPORT texture_engine
{
	/// Context configuration reference
	context_config &config_;

	/// Input texture state
	std::map<std::string, std::tuple<std::shared_ptr<gl::texture>, bool> > input_textures_;

	/// The empty texture
	std::shared_ptr<gl::texture> empty_texture_;

	/// Registered texture handlers
	std::map<std::string, input_handler> handlers_;

	// Default texture handlers
	std::shared_ptr<gl::texture> soil_texture_handler(const input_config &inputConfig,
													  bool &skipTextureOptions,
													  bool &skipCache,
													  bool &framebufferSized);
	std::shared_ptr<gl::texture> noise_texture_handler(const input_config &inputConfig,
													   bool &skipTextureOptions,
													   bool &skipCache,
													   bool &framebufferSized);
	std::shared_ptr<gl::texture> checker_texture_handler(const input_config &inputConfig,
														 bool &skipTextureOptions,
														 bool &skipCache,
														 bool &framebufferSized);

protected:
	/**
	 * @brief      Applies filtering and wrapping options of a given input onto
	 *             an already allocated OpenGL texture object.
	 *
	 * @param[in]  inputConfig  The input configuration for this texture
	 * @param      texture      The texture to apply the options to
	 */
	void apply_texture_options(const input_config &inputConfig,
							   gl::texture &texture);

public:
	/**
	 * @brief      Initialize a new instance of the texture engine class.
	 *
	 * @param      config  Context configuration to use for this engine.
	 */
	texture_engine(context_config &config);

	/**
	 * @brief      Initialize this texture engine
	 */
	void init();

	/**
	 * @brief      Clears loaded textures.
	 *
	 * @param      framebufferSizeChange  true if this event is being triggered
	 *                                    by a change in framebuffer size, which
	 *                                    should then only clear the textures
	 *                                    whose size is the same as the
	 *                                    framebuffer. false if all textures
	 *                                    should be reloaded, including
	 *                                    framebuffer-independent textures.
	 */
	void clear(bool framebufferSizeChange = false);

	/**
	 * @brief      Get the texture for the given input
	 *
	 * @param[in]  inputConfig  The input configuration
	 *
	 * @return     A texture instance to be used for rendering. If the actual
	 *             input configuration is invalid, an empty texture is returned.
	 */
	gl::texture &input_texture(const input_config &inputConfig);

	/**
	 * @brief      Registers a texture type handler with the given name.
	 *
	 * @param[in]  name     Name (type ID) of the texture handler
	 * @param[in]  handler  Handler function. See InputHandler for more details.
	 */
	void register_handler(const std::string &name, input_handler handler);
};

}

#endif /* _SHADERTOY_TEXTURE_ENGINE_HPP_ */
