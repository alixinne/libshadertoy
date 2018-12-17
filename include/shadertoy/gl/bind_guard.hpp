#ifndef _SHADERTOY_GL_BIND_GUARD_HPP_
#define _SHADERTOY_GL_BIND_GUARD_HPP_

#include <tuple>

#include "shadertoy/utils/assert.hpp"

namespace shadertoy
{
namespace gl
{
	/**
	 * @brief Class that defines operations that are possible on a particular
	 *        bound object.
	 */
	template<typename resource_type>
	class bound_ops_base {
		const resource_type &resource_;
		bool has_resource_;

	protected:
		bound_ops_base(const resource_type &resource)
			: resource_(resource), has_resource_(true)
		{}

		bound_ops_base(bound_ops_base &&other)
			: resource_(other.resource_), has_resource_(true)
		{
			shadertoy::utils::error_assert(other.has_resource_, "Trying to move out of empty bound_ops");
			other.has_resource_ = false;
		}

		/**
		 * @brief Drops the reference to the resource, meaning
		 *        it will not be unbound by bind_guards.
		 */
		void drop()
		{
			has_resource_ = false;
		}

	public:
		/**
		 * @brief  Get the bound resource
		 *
		 * @return Reference to the bound resource object
		 */
		inline const resource_type &resource() const { return resource_; }

		/**
		 * @brief  Returns a boolean indicating if this object holds a resource or not
		 *
		 *         Note that moved-out bind guards do not hold a resource.
		 */
		inline bool has_resource() const { return has_resource_; }

		// Non copy-able
		bound_ops_base(const bound_ops_base<resource_type> &) = delete;
		bound_ops_base<resource_type> &operator=(const bound_ops_base<resource_type> &) = delete;
	};

	template<typename resource_type>
	class bound_ops : public bound_ops_base<resource_type> {
	protected:
		bound_ops(const resource_type &resource)
			: bound_ops_base<resource_type>(resource)
		{}
	};

	/**
	 * @brief Guard objects that handles binding and unbinding various
	 *        OpenGL objects automatically when it goes out of scope.
	 */
	template<typename resource_type, typename... bind_args>
	class bind_guard : public bound_ops<resource_type>
	{
		std::tuple<bind_args...> args_;

		template<size_t... I>
		void unbind_resource(std::index_sequence<I...>)
		{
			if (this->has_resource())
				this->resource().unbind(std::get<I>(args_)...);
		}

	public:
		/**
		 * @brief Constructs the bind_guard. Binds \p resource using the given \p args.
		 *
		 * @param resource Resource to bind
		 * @param args     Resource bind arguments
		 */
		bind_guard(const resource_type &resource, bind_args&&... args)
			: bound_ops<resource_type>(resource), args_(args...)
		{
			this->resource().bind(args...);
		}

		bind_guard(bind_guard<resource_type, bind_args...> &&other)
			: bound_ops<resource_type>(std::forward<bound_ops<resource_type>&&>(other)), args_(std::move(other.args_))
		{
		}

		bind_guard<resource_type> &operator=(bind_guard<resource_type, bind_args...> &&other)
		{
			if (this != &other)
			{
				bound_ops_base<resource_type>::operator=(other);
				args_ = std::move(other.args_);
			}

			return (*this);
		}

		~bind_guard()
		{
			unbind_resource(std::make_index_sequence<sizeof...(bind_args)>());
		}

		template<typename resource_type_, typename... bind_args_>
		friend void drop_bind_guard(bind_guard<resource_type_, bind_args_...> &&guard);
	};

	template<typename resource_type, typename... bind_args>
	auto get_bind_guard(const resource_type &resource, bind_args&&... args)
	{
		return bind_guard<resource_type, bind_args...>(resource, std::forward<bind_args>(args)...);
	}

	template<typename resource_type, typename... bind_args>
	void drop_bind_guard(bind_guard<resource_type, bind_args...> &&guard)
	{
		guard.drop();
	}
}
}

#endif /* _SHADERTOY_GL_BIND_GUARD_HPP_ */
