#ifndef _SHADERTOY_SWAP_CHAIN_HPP_
#define _SHADERTOY_SWAP_CHAIN_HPP_

#include "shadertoy/pre.hpp"

#include <deque>
#include <memory>
#include <set>

namespace shadertoy
{

/**
 * @brief This class represents a swap chain. A swap chain specifies how
 * buffers are rendered to obtain the final result.
 */
class swap_chain
{
	/// Ordered list of swap chain elements
	std::deque<std::shared_ptr<members::basic_member>> members_;

	/// Set of chain elements, used to check for the absence of duplicates
	std::set<std::shared_ptr<members::basic_member>> members_set_;

	/// Last member that has been rendered in this swap chain
	std::shared_ptr<members::basic_member> current_;

public:
	/**
	 * @brief Initializes a new instance of the swap_chain class.
	 */
	swap_chain();

	/**
	 * @brief Obtains the list of members of this swap_chain
	 *
	 * @return Reference to the list of members
	 */
	inline const std::deque<std::shared_ptr<members::basic_member>> &members() const
	{ return members_; }

	/**
	 * @brief Obtains the last member that has been rendered in this swap_chain
	 *
	 * @return Last member that has been rendered, or null
	 */
	inline const std::shared_ptr<members::basic_member> &current() const
	{ return current_; }

	/**
	 * @brief Obtains the member that occurs before \p member
	 *
	 * @param member Member that immediately follows the return value
	 *
	 * @return A pointer to the member that precedes\p member, or null if \p member
	 * is the first member
	 */
	std::shared_ptr<members::basic_member> before(members::basic_member *member) const;

	/**
	 * @brief Adds a member to the end of this swap chain
	 *
	 * @param member Member to add to the end of this swap chain
	 */
	void push_back(std::shared_ptr<members::basic_member> member);

	/**
	 * @brief Constructs a swap_chain member and adds it to the end of this swap chain
	 *
	 * @param args Arguments to forward to the swap chain member constructor
	 *
	 * @return Constructed member
	 */
	template<typename... Args>
	auto emplace_back(Args&&... args)
	{
		auto member(members::member_data(std::forward<Args>(args)...));
		push_back(member);
		return member;
	}

	/**
	 * @brief Find and return the first member of this chain that matches a given predicate
	 *
	 * @param  predicate  Predicate to find a match. Should accept a std::shared_ptr<MemberType> as a parameter and return
	 * a boolean.
	 * @tparam MemberType Type of the member to look for
	 * @tparam Callable   Type of the predicate
	 *
	 * @return Pointer to the member if found, or null otherwise
	 */
	template<typename MemberType, typename Callable>
	std::shared_ptr<MemberType> find_if(Callable predicate)
	{
		for (const auto &member : members_)
		{
			if (auto ptr_member = std::dynamic_pointer_cast<MemberType>(member))
				if (predicate(ptr_member))
					return ptr_member;
		}

		return {};
	}

	/**
	 * @brief Renders the swap chain using \p context up to the specified \p target
	 *
	 * All members from members().begin() up to \p target will be fully rendered.
	 *
	 * @param context Context used to render this swap chain
	 * @param target  Optional. Target member to render. If null, the last buffer in
	 * chain will be used as the rendering target.
	 *
	 * @return Pointer to the latest rendered member
	 */
	std::shared_ptr<members::basic_member> render(const render_context &context,
												  std::shared_ptr<members::basic_member> target 
													= std::shared_ptr<members::basic_member>());

	/**
	 * @brief Initializes the members of this swap chain
	 *
	 * @param context Context used for initialization
	 */
	void init(const render_context &context);

	/**
	 * @brief Allocates the textures for all the members of this swap chain
	 *
	 * @param context Context used to allocate textures
	 */
	void allocate_textures(const render_context &context);
};
}

#endif /* _SHADERTOY_SWAP_CHAIN_HPP_ */
