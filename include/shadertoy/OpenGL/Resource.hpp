#ifndef _SHADERTOY_OPENGL_RESOURCE_HPP_
#define _SHADERTOY_OPENGL_RESOURCE_HPP_

#include "shadertoy/pre.hpp"

namespace shadertoy
{
namespace OpenGL
{
	typedef void (*SingleResourceDeleter)(GLuint);
	typedef void (*MultiResourceDeleter)(GLsizei, const GLuint*);

	template<SingleResourceDeleter *DeleteFunction>
	class SingleDeleter
	{
	public:
		void operator()(GLuint resource)
		{ (*DeleteFunction)(resource); }
	};

	template<MultiResourceDeleter *DeleteFunction>
	class MultiDeleter
	{
	public:
		void operator()(GLuint resource)
		{ (*DeleteFunction)(1, &resource); }
	};

	template<MultiResourceDeleter DeleteFunction>
	class MultiStaticDeleter
	{
	public:
		void operator()(GLuint resource)
		{ DeleteFunction(1, &resource); }
	};

	template<typename TFinal, typename TDeleter, typename TError>
	class Resource
	{
	public:
		typedef TError ErrorType;

		/**
		 * Creates an empty resource.
		 */
		Resource()
			: hasRes(false)
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

	private:
		/**
		 * Creates a new resource object from an existing resourceID.
		 *
		 * @param  texId Resource id to reference
		 */
		explicit Resource(GLuint resId);

		/**
		 * Destroys the object referenced by this resource.
		 */
		void Free()
		{
			if (hasRes)
			{
				TDeleter()(resId);
				hasRes = false;
			}
		}

		bool hasRes;
		GLuint resId;
	};

	template<typename TResource>
	class WeakRef
	{
		static constexpr const TResource NullResource = TResource();
	public:

		WeakRef()
		: ref(TResource::NullValue) { }
		WeakRef(const TResource &resource)
		: ref(resource) { }

		operator bool() const
		{ return &ref != &TResource::NullValue && ref; }

		GLuint operator*() const throw(typename TResource::ErrorType)
		{ return *ref; }

	private:
		const TResource &ref;
	};
}
}

#endif /* _SHADERTOY_OPENGL_RESOURCE_HPP_ */
