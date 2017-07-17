#ifndef _SHADERTOY_MISC_HPP_
#define _SHADERTOY_MISC_HPP_

#include "shadertoy/pre.hpp"

namespace boost {
namespace serialization {
template<class Archive>
void shadertoy_EXPORT serialize(Archive &ar, oglplus::Vec3f &vec, const unsigned int version) {
	ar & make_nvp("x", vec[0]);
	ar & make_nvp("y", vec[1]);
	ar & make_nvp("z", vec[2]);
}

template<class Archive>
void shadertoy_EXPORT serialize(Archive &ar, oglplus::Vec4f &vec, const unsigned int version) {
	ar & make_nvp("x", vec[0]);
	ar & make_nvp("y", vec[1]);
	ar & make_nvp("z", vec[2]);
	ar & make_nvp("w", vec[3]);
}
}
}

#endif /* _SHADERTOY_MISC_HPP_ */
