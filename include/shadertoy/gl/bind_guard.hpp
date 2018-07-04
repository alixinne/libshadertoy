#ifndef _SHADERTOY_GL_BIND_GUARD_HPP_
#define _SHADERTOY_GL_BIND_GUARD_HPP_

#include <tuple>

namespace shadertoy
{
namespace gl
{
	/**
	 * @brief Guard objects that handles binding and unbinding various
	 *        OpenGL objects automatically when it goes out of scope.
	 */
	template<typename resource_type, typename... bind_args>
	class bind_guard
	{
		const resource_type &resource_;
		std::tuple<bind_args...> args_;

		template<size_t... I>
		void unbind_resource(std::index_sequence<I...>)
		{
			resource_.unbind(std::get<I>(args_)...);
		}

	public:
		/**
		 * @brief Constructs the bind_guard. Binds \p resource using the given \p args.
		 *
		 * @param resource Resource to bind
		 * @param args     Resource bind arguments
		 */
		bind_guard(const resource_type &resource, bind_args&&... args)
			: resource_(resource), args_(args...)
		{
			resource_.bind(args...);
		}

		~bind_guard()
		{
			unbind_resource(std::make_index_sequence<sizeof...(bind_args)>());
		}
	};

	template<typename resource_type, typename... bind_args>
	auto get_bind_guard(const resource_type &resource, bind_args&&... args)
	{
		return bind_guard<resource_type, bind_args...>(resource, std::forward<bind_args>(args)...);
	}
}
}

#endif /* _SHADERTOY_GL_BIND_GUARD_HPP_ */
