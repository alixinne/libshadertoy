#ifndef _SHADERTOY_OPENGL_RESOURCE_HPP_
#define _SHADERTOY_OPENGL_RESOURCE_HPP_

#include "shadertoy/pre.hpp"
#include "shadertoy/OpenGL/Caller.hpp"

namespace shadertoy
{
namespace OpenGL
{
	typedef GLuint (*SingleResourceCreator)();
	typedef void (*MultiResourceCreator)(GLsizei, GLuint*);

	typedef void (*SingleResourceDeleter)(GLuint);
	typedef void (*MultiResourceDeleter)(GLsizei, const GLuint*);

	template<SingleResourceCreator *CreateFunction, SingleResourceDeleter *DeleteFunction>
	class SingleAllocator
	{
	public:
		GLuint Create()
		{ return glCall(*CreateFunction); }

		void Delete(GLuint resource)
		{ glCall(*DeleteFunction, resource); }
	};

	template<MultiResourceCreator *CreateFunction, MultiResourceDeleter *DeleteFunction>
	class MultiAllocator
	{
	public:
		GLuint Create()
		{
			GLuint res;
			glCall(*CreateFunction, 1, &res);
			return res;
		}

		void Delete(GLuint resource)
		{ glCall(*DeleteFunction, 1, &resource); }
	};

	template<typename TFinal, typename TAllocator, typename TError>
	class Resource
	{
	public:
		typedef TAllocator Allocator;
		typedef TError ErrorType;

		/**
		 * Creates a resource using the given allocator.
		 */
		Resource()
			: hasRes(true),
			resId(TAllocator().Create())
		{
		}

		/**
		 * Destroys the object referenced by this resource.
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
		 */
		GLuint operator*() const throw(TError)
		{
			if (!hasRes)
			{
				throw TError();
			}

			return resId;
		}

		// No copy operator
		Resource(const Resource &) = delete;
		Resource &operator=(const Resource &) = delete;

		// Move operators
		Resource(Resource &&other)
		{
			(*this) = std::move(other);
		}

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
		explicit Resource(GLuint resId);

	private:
		/**
		 * Destroys the object referenced by this resource.
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
