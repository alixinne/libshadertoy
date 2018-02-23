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
typedef std::function<std::shared_ptr<OpenGL::Texture>(
	const InputConfig &inputConfig,
	bool &skipTextureOptions,
	bool &skipCache,
	bool &framebufferSized)> InputHandler;

/**
 * @brief      Represents the engine responsible for loading input textures for
 *             the current rendering context.
 */
class shadertoy_EXPORT TextureEngine
{
	/// Context configuration reference
	ContextConfig &config;

	/// Input texture state
	std::map<std::string, std::tuple<std::shared_ptr<OpenGL::Texture>, bool> > inputTextures;

	/// The empty texture
	std::shared_ptr<OpenGL::Texture> emptyTexture;

	/// Registered texture handlers
	std::map<std::string, InputHandler> handlers;

	// Default texture handlers
	std::shared_ptr<OpenGL::Texture> SOILTextureHandler(const InputConfig &inputConfig,
		bool &skipTextureOptions,
		bool &skipCache,
		bool &framebufferSized);
	std::shared_ptr<OpenGL::Texture> NoiseTextureHandler(const InputConfig &inputConfig,
		bool &skipTextureOptions,
		bool &skipCache,
		bool &framebufferSized);
	std::shared_ptr<OpenGL::Texture> CheckerTextureHandler(const InputConfig &inputConfig,
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
	void ApplyTextureOptions(const InputConfig &inputConfig,
							 OpenGL::Texture &texture);

public:
	/**
	 * @brief      Initialize a new instance of the texture engine class.
	 *
	 * @param      config  Context configuration to use for this engine.
	 */
	TextureEngine(ContextConfig &config);

	/**
	 * @brief      Initialize this texture engine
	 */
	void Initialize();

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
	void ClearState(bool framebufferSizeChange = false);

	/**
	 * @brief      Get the texture for the given input
	 *
	 * @param[in]  inputConfig  The input configuration
	 *
	 * @return     A texture instance to be used for rendering. If the actual
	 *             input configuration is invalid, an empty texture is returned.
	 */
	OpenGL::Texture &GetInputTexture(const InputConfig &inputConfig);

	/**
	 * @brief      Registers a texture type handler with the given name.
	 *
	 * @param[in]  name     Name (type ID) of the texture handler
	 * @param[in]  handler  Handler function. See InputHandler for more details.
	 */
	void RegisterHandler(const std::string &name, InputHandler handler);
};

}

#endif /* _SHADERTOY_TEXTURE_ENGINE_HPP_ */
