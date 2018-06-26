#ifndef _SHADERTOY_IO_RESOURCE_HPP_
#define _SHADERTOY_IO_RESOURCE_HPP_

#include "shadertoy/pre.hpp"

namespace shadertoy
{

/**
 * @brief Represents a set of textures used as input and outputs of a buffer
 */
class io_resource
{
	/// Size of the rendering resources in this object
	rsize_ref render_size_;

	/// Texture format
	GLint internal_format_;

	/// Source texture
	std::shared_ptr<gl::texture> source_tex_;

	/// Target texture
	std::shared_ptr<gl::texture> target_tex_;

	/// Allocates a texture for this object
	void init_render_texture(rsize size, std::shared_ptr<gl::texture> &texptr);

public:
	/**
	 * @brief Create a new io_resource of the given size and format.
	 *
	 * @param render_size     Initial size
	 * @param internal_format Internal format, as defined by https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
	 */
	io_resource(rsize_ref &&render_size, GLint internal_format = GL_RGBA32F);

	/**
	 * @brief      Allocate the textures in this IO object
	 */
	void allocate();

	/**
	 * @brief      Swap the input and output textures after rendering
	 */
	void swap();

	/**
	 * @brief      Get the rendering size for this IO object
	 *
	 * @return     Rendering size reference object
	 */
	inline const rsize_ref &render_size() const { return render_size_; }

	/**
	 * @brief      Set the rendering size for this IO object
	 *
	 * This method does not reset the allocated textures. The allocate method
	 * should be called after.
	 *
	 * @param new_size New rendering size object
	 */
	inline void render_size(rsize_ref &&new_size)
	{ render_size_ = std::move(new_size); }

	/**
	 * @brief      Get the rendering format for this IO object
	 *
	 * @return     Internal format of the rendering texture
	 */
	inline GLint internal_format() const { return internal_format_; }

	/**
	 * @brief      Set the rendering format for this IO object
	 *
	 * This method does not reset the allocated textures. The allocate method
	 * should be called after.
	 *
	 * @param new_format New internal format for the rendering textures
	 */
	inline void internal_format(GLint new_format)
	{ internal_format_ = new_format; }

	/**
	 * @brief      Get a reference to the source texture for this buffer
	 *
	 * @return     Source texture for this buffer.
	 */
	inline const std::shared_ptr<gl::texture> &source_texture() const
	{ return source_tex_; }

	/**
	 * @brief      Get a reference to the current texture for this buffer
	 *
	 * @return     Target (current) texture for this buffer.
	 */
	inline const std::shared_ptr<gl::texture> &target_texture() const
	{ return target_tex_; }

};
}

#endif /* _SHADERTOY_IO_RESOURCE_HPP_ */
