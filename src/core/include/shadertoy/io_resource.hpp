#ifndef _SHADERTOY_IO_RESOURCE_HPP_
#define _SHADERTOY_IO_RESOURCE_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/member_swap_policy.hpp"
#include "shadertoy/output_name.hpp"
#include "shadertoy/size.hpp"

#include "shadertoy/backends/gx/texture.hpp"

#include <functional>

namespace shadertoy
{

/**
 * @brief Represents the properties of a texture to use as an output buffer
 */
struct shadertoy_EXPORT output_buffer_spec
{
	/// Size of the texture, in pixels
	rsize_ref render_size;

	/// Name of the output
	output_name_info_t name;

	/// Internal format of the texture
	GLint internal_format;

	/**
	 * @brief Create an empty output_buffer_spec
	 */
	output_buffer_spec() = default;

	/**
	 * @brief Create a new output_buffer_spec of the given size and format and location
	 *
	 * @param render_size     Initial size
	 * @param name            Name of the output
	 * @param internal_format Internal format, as defined by https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
	 */
	output_buffer_spec(rsize_ref render_size, output_name_info_t name, GLint internal_format = GL_RGBA32F);
};

/**
 * @brief Represents a set of textures used as input and outputs of a buffer
 */
class shadertoy_EXPORT io_resource
{
	struct output_buffer
	{
		/// Source texture
		std::unique_ptr<backends::gx::texture> source_tex;

		/// Target texture
		std::unique_ptr<backends::gx::texture> target_tex;

		/**
		 * @brief Allocate the texture for this resource object
		 */
		void allocate(const output_buffer_spec &spec, const io_resource *resource);

		/**
		 * @brief      Swap the input and output textures after rendering
		 */
		void swap(const output_buffer_spec &spec, const io_resource *resource);

		/**
		 * @brief      Get a reference to the source texture for this buffer
		 *
		 * @return     Source texture for this buffer.
		 */
		inline const std::unique_ptr<backends::gx::texture> &source_texture() const
		{
			return source_tex;
		}

		/**
		 * @brief      Get a reference to the current texture for this buffer
		 *
		 * @return     Target (current) texture for this buffer.
		 */
		inline const std::unique_ptr<backends::gx::texture> &target_texture() const
		{
			if (target_tex)
				return target_tex;
			return source_tex;
		}

		private:
		/// Allocates a texture for this object
		void init_render_texture(const output_buffer_spec &spec, rsize size,
								 std::unique_ptr<backends::gx::texture> &texptr);
	};

	/// List of output specifications for this resource
	std::vector<output_buffer_spec> output_specs_;

	/// List of registered outputs for this resource
	std::vector<output_buffer> outputs_;

	/// Swapping policy
	member_swap_policy swap_policy_;

	public:
	/**
	 * @brief Create a new io_resource of the given size and format.
	 *
	 * @param swap_policy     Texture swapping policy, controls how many textures are allocated
	 */
	io_resource(member_swap_policy swap_policy = member_swap_policy::double_buffer);

	/**
	 * @brief      Allocate the textures in this IO object
	 */
	void allocate();

	/**
	 * @brief      Swap the input and output textures after rendering
	 */
	void swap();

	/**
	 * @brief      get the list of output buffer specifications
	 *
	 * @return     reference to the list of output buffer specifications
	 */
	inline std::vector<output_buffer_spec> &output_specs() { return output_specs_; }

	/**
	 * @brief      get the list of output buffer specifications
	 *
	 * @return     reference to the list of output buffer specifications
	 */
	inline const std::vector<output_buffer_spec> &output_specs() const { return output_specs_; }

	/**
	 * @brief      Get the swapping policy for this IO object
	 *
	 * @return     Swapping policy of this IO object
	 */
	inline member_swap_policy swap_policy() const { return swap_policy_; }

	/**
	 * @brief      Set the swapping policy for this IO object
	 *
	 * This method does not reset the allocated textures. The allocate method
	 * should be called after.
	 *
	 * @param new_policy New swapping policy
	 */
	inline void swap_policy(member_swap_policy new_policy) { swap_policy_ = new_policy; }

	/**
	 * @brief        Get a reference to the source texture for this buffer
	 *
	 * @param target Target buffer index to obtain. Must be less than \c output_specs().size()
	 *
	 * @return       Source texture for this buffer.
	 */
	inline const std::unique_ptr<backends::gx::texture> &source_texture(size_t target) const
	{
		return outputs_.at(target).source_texture();
	}

	/**
	 * @brief        Get a reference to the current texture for this buffer
	 *
	 * @param target Target buffer index to obtain. Must be less than \c output_specs().size()
	 *
	 * @return       Target (current) texture for this buffer.
	 */
	inline const std::unique_ptr<backends::gx::texture> &target_texture(size_t target) const
	{
		return outputs_.at(target).target_texture();
	}
};
} // namespace shadertoy

#endif /* _SHADERTOY_IO_RESOURCE_HPP_ */
