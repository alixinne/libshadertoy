#ifndef _SHADERTOY_OPENGL_RESOURCE_HPP_
#define _SHADERTOY_OPENGL_RESOURCE_HPP_

#include "shadertoy/pre.hpp"
#include "shadertoy/OpenGL/Caller.hpp"

namespace shadertoy
{
namespace OpenGL
{
	/// Allocation function that creates single objects
	typedef GLuint (*SingleResourceCreator)();
	/// Allocation function that creates multiple objects
	typedef void (*MultiResourceCreator)(GLsizei, GLuint*);

	/// De-allocation function that frees a single object
	typedef void (*SingleResourceDeleter)(GLuint);
	//// De-allocation function that frees multiple objects
	typedef void (*MultiResourceDeleter)(GLsizei, const GLuint*);

	/**
	 * Represents a resource that is allocated one by one.
	 */
	template<SingleResourceCreator *CreateFunction, SingleResourceDeleter *DeleteFunction>
	class shadertoy_EXPORT SingleAllocator
	{
	public:
		/**
		 * Creates the associated resource.
		 * @return The id of the created resource
		 * @throws OpenGLError
		 */
		GLuint Create()
		{ return glCall(*CreateFunction); }

		/**
		 * Deletes the given resource.
		 * @param resource Resource to delete
		 * @throws OpenGLError
		 */
		void Delete(GLuint resource)
		{ glCall(*DeleteFunction, resource); }
	};

	/**
	 * Represents a resource that is allocated in batches.
	 */
	template<MultiResourceCreator *CreateFunction, MultiResourceDeleter *DeleteFunction>
	class shadertoy_EXPORT MultiAllocator
	{
	public:
		/**
		 * Creates the associated resource.
		 * @return The id of the created resource
		 * @throws OpenGLError
		 */
		GLuint Create()
		{
			GLuint res;
			glCall(*CreateFunction, 1, &res);
			return res;
		}

		/**
		 * Deletes the given resource.
		 * @param resource Resource to delete
		 * @throws OpenGLError
		 */
		void Delete(GLuint resource)
		{ glCall(*DeleteFunction, 1, &resource); }
	};

	/**
	 * Represents an OpenGL resource and manages its lifecycle. Uses move
	 * semantics, so only one Resource object can be the owner of a
	 * corresponding OpenGL resource, such as a texture, program, shader, etc.
	 */
	template<typename TFinal, typename TAllocator, typename TError>
	class shadertoy_EXPORT Resource
	{
	public:
		/// Type of the allocator object
		typedef TAllocator Allocator;
		/// Type of the null reference error
		typedef TError ErrorType;

		/**
		 * Creates a resource using the given allocator.
		 *
		 * @throws OpenGLError
		 */
		Resource()
			: hasRes(true),
			resId(TAllocator().Create())
		{
		}

		/**
		 * Destroys the object referenced by this resource.
		 *
		 * @throws OpenGLError
		 */
		~Resource()
		{
			Free();
		}

		/**
		 * Returns true if this resource object holds a reference to a resource.
		 *
		 * @return true if this resource object holds a reference to a resource,
		 *         false otherwise.
		 */
		operator bool() const
		{ return hasRes; }

		/**
		 * Returns the underlying texture identifier referenced by this texture
		 * object. Throws an exception if this object does not hold a reference.
		 *
		 * @throws TError
		 */
		operator GLuint() const throw(TError)
		{
			if (!hasRes)
			{
				throw TError();
			}

			return resId;
		}

		/// Deleted copy constructor
		Resource(const Resource &) = delete;
		/// Deleted copy operator
		Resource &operator=(const Resource &) = delete;

		/**
		 * Move constructor
		 * @param other Instance to move
		 *
		 * @throws OpenGLError
		 */
		Resource(Resource &&other)
		{
			this->hasRes = other.hasRes;
			this->resId = other.resId;

			other.hasRes = false;
		}

		/**
		 * Move operator
		 * @param other Instance to move
		 *
		 * @throws OpenGLError
		 */
		TFinal &operator=(TFinal &&other)
		{
			if (this != &other)
			{
				this->Free();

				this->hasRes = other.hasRes;
				this->resId = other.resId;

				other.hasRes = false;
			}

			return (*this);
		}

	protected:
		/**
		 * Creates a new resource object from an existing resourceID.
		 *
		 * @param  texId Resource id to reference
		 */
		explicit Resource(GLuint resId)
			: hasRes(true),
			resId(resId)
		{}

	private:
		/**
		 * Destroys the object referenced by this resource.
		 *
		 * @throws OpenGLError
		 */
		void Free()
		{
			if (hasRes)
			{
				TAllocator().Delete(resId);
				hasRes = false;
			}
		}

		bool hasRes;
		GLuint resId;
	};
}
}

#endif /* _SHADERTOY_OPENGL_RESOURCE_HPP_ */
