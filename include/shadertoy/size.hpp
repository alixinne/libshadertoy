#ifndef _SHADERTOY_SIZE_HPP_
#define _SHADERTOY_SIZE_HPP_

#include "shadertoy/shadertoy_error.hpp"

#include <functional>

namespace shadertoy
{

enum size_type
{
	size_explicit,
	size_automatic
};

class shadertoy_EXPORT size_error : public shadertoy_error
{
public:
	/**
	 * @brief       Initializes a new instance of the size_error class.
	 *
	 * @param what  Error message
	 */
	explicit size_error(const std::string &what);
};

template <typename T> class basic_size
{
	size_type type_;
	T width_;
	T height_;

	static void size_assert(bool type_result, const std::string &what)
	{
		if (!type_result)
			throw size_error(what);
	}

public:
	basic_size(T width, T height) : type_(size_explicit), width_(width), height_(height) {}

	basic_size() : type_(size_automatic), width_(0), height_(0) {}

	T width() const
	{
		size_assert(type_ == size_explicit,
					"A size reference must be given for automatic size calculation");

		return width_;
	}

	T height() const
	{
		size_assert(type_ == size_explicit,
					"A size reference must be given for automatic size calculation");

		return height_;
	}

	T width(const basic_size<T> &ref) const
	{
		if (type_ == size_automatic)
			return ref.width();

		// else type_ == size_explicit
		return width_;
	}

	T height(const basic_size<T> &ref) const
	{
		if (type_ == size_automatic)
			return ref.height();

		// else type_ == size_explicit
		return height_;
	}

	basic_size<T> resolve(const basic_size<T> &ref) const
	{
		return basic_size<T>(width(ref), height(ref));
	}
};

template <typename T> class basic_size_ref
{
	bool has_size_;
	basic_size<T> size_;
	std::function<basic_size<T>(void)> size_getter_;

public:
	basic_size_ref() : has_size_(false), size_(), size_getter_() {}

	basic_size_ref(const basic_size<T> &size) : has_size_(true), size_(size), size_getter_() {}

	basic_size_ref(const basic_size<T> &size, std::function<basic_size<T>(void)> size_getter)
	: has_size_(true), size_(size), size_getter_(size_getter)
	{
	}

	basic_size_ref(std::function<basic_size<T>(void)> size_getter)
	: has_size_(true), size_(), size_getter_(size_getter)
	{
	}

	void set(const basic_size<T> &size)
	{
		has_size_ = true;
		size_ = size;
	}

	void set_getter(std::function<basic_size<T>(void)> size_getter) { size_getter_ = size_getter; }

	operator basic_size<T>() const
	{
		if (!has_size_)
			throw size_error("This size object is undefined");
		if (size_getter_)
			return size_.resolve(size_getter_());
		return size_;
	}

	basic_size<T> resolve(basic_size<T> fallback) const
	{
		if (has_size_)
		{
			if (size_getter_)
				return size_.resolve(size_getter_().resolve(fallback));
			return size_.resolve(fallback);
		}
		else
		{
			if (size_getter_)
				return fallback.resolve(size_getter_());
			return fallback;
		}
	}
};

typedef basic_size<size_t> rsize;
typedef basic_size_ref<size_t> rsize_ref;
}

#endif /* _SHADERTOY_SIZE_HPP_ */
