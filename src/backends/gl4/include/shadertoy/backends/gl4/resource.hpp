#ifndef _SHADERTOY_BACKENDS_GL4_RESOURCE_HPP_
#define _SHADERTOY_BACKENDS_GL4_RESOURCE_HPP_

#include "shadertoy/backends/gl4/pre.hpp"
#include "shadertoy/backends/gx/resource.hpp"

#include "shadertoy/backends/gl4/caller.hpp"

SHADERTOY_BACKENDS_GL4_NAMESPACE_BEGIN
/// Allocation function that creates single objects
typedef GLuint (*single_resource_creator)();
/// Allocation function that creates multiple objects
typedef void (*multi_resource_creator)(GLsizei, GLuint *);

/// De-allocation function that frees a single object
typedef void (*single_resource_deleter)(GLuint);
/// De-allocation function that frees multiple objects
typedef void (*multi_resource_deleter)(GLsizei, const GLuint *);

/**
 * @brief Represents a resource that is allocated one by one.
 */
template <single_resource_creator *CreateFunction, single_resource_deleter *DestroyFunction>
class stbackend_gl4_EXPORT single_allocator
{
	public:
	/**
	 * @brief Create the associated resource.
	 * @return The id of the created resource
	 * @throws OpenGLError
	 */
	GLuint create() { return gl_call(*CreateFunction); }

	/**
	 * @brief Delete the given resource.
	 * @param resource Resource to delete
	 * @throws OpenGLError
	 */
	void destroy(GLuint resource) { gl_call(*DestroyFunction, resource); }
};

/**
 * @brief Represents a resource that is allocated in batches.
 */
template <multi_resource_creator *CreateFunction, multi_resource_deleter *DestroyFunction>
class stbackend_gl4_EXPORT multi_allocator
{
	public:
	/**
	 * @brief Create the associated resource.
	 * @return The id of the created resource
	 * @throws OpenGLError
	 */
	GLuint create()
	{
		GLuint res;
		gl_call(*CreateFunction, 1, &res);
		return res;
	}

	/**
	 * @brief Delete the given resource.
	 * @param resource Resource to delete
	 * @throws OpenGLError
	 */
	void destroy(GLuint resource) { gl_call(*DestroyFunction, 1, &resource); }
};

/**
 * @brief Represents an OpenGL resource and manages its lifecycle. Uses move
 * semantics, so only one Resource object can be the owner of a
 * corresponding OpenGL resource, such as a texture, program, shader, etc.
 */
template <typename Final, typename Allocator, typename Error, typename BaseResource>
class stbackend_gl4_EXPORT resource : public BaseResource
{
	public:
	/// Type of the allocator object
	typedef Allocator allocator_type;
	/// Type of the null reference error
	typedef Error error_type;
	/// Type of the subclassed resource type
	typedef Final resource_type;
	/// Type of self
	typedef resource<Final, Allocator, Error, BaseResource> base_resource_type;

	/**
	 * @brief Create a resource using the given allocator.
	 *
	 * @throws opengl_error
	 */
	resource() : has_res_(true), res_id_(allocator_type().create()) {}

	/**
	 * @brief Destroy the object referenced by this resource.
	 *
	 * @throws opengl_error
	 */
	~resource() { free(); }

	/**
	 * @brief Return true if this resource object holds a reference to a resource.
	 *
	 * @return true if this resource object holds a reference to a resource,
	 *         false otherwise.
	 */
	operator bool() const override { return has_res_; }

	/**
	 * @brief Return the underlying texture identifier referenced by this texture
	 * object. Throws an exception if this object does not hold a reference.
	 *
	 * @throws error_type
	 */
	operator GLuint() const override
	{
		if (!has_res_)
		{
			throw error_type();
		}

		return res_id_;
	}

	/// Deleted copy constructor
	resource(const resource &) = delete;
	/// Deleted copy operator
	resource &operator=(const resource &) = delete;

	/**
	 * @brief Move constructor
	 * @param other Instance to move
	 *
	 * @throws OpenGLError
	 */
	resource(resource &&other)
	{
		this->has_res_ = other.has_res_;
		this->res_id_ = other.res_id_;

		other.has_res_ = false;
	}

	protected:
	/**
	 * @brief Create a new resource object from an existing resourceID.
	 *
	 * @param resId Resource id to reference
	 */
	explicit resource(GLuint resId) : has_res_(true), res_id_(resId) {}

	/**
	 * @brief Move operator
	 * @param other Instance to move
	 *
	 * @return Assigned resource
	 *
	 * @throws OpenGLError
	 */
	resource_type &assign_operator(resource_type &&other)
	{
		if (this != &other)
		{
			this->free();

			this->has_res_ = other.has_res_;
			this->res_id_ = other.res_id_;

			other.has_res_ = false;
		}

		return static_cast<resource_type &>(*this);
	}

	private:
	/**
	 * @brief Destroy the object referenced by this resource.
	 *
	 * @throws OpenGLError
	 */
	void free()
	{
		if (has_res_)
		{
			allocator_type().destroy(res_id_);
			has_res_ = false;
		}
	}

	bool has_res_;
	GLuint res_id_;
};
SHADERTOY_BACKENDS_GL4_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GL4_RESOURCE_HPP_ */
