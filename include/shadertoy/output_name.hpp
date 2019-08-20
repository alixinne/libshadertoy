#ifndef _SHADERTOY_OUTPUT_NAME_HPP_
#define _SHADERTOY_OUTPUT_NAME_HPP_

#include "shadertoy/pre.hpp"

#include <string>
#include <tuple>
#include <variant>

namespace shadertoy
{

/**
 * @brief Name specification of a given buffer output
 *
 * This type designates a buffer output either by name or by integer
 * location in the associated program.
 */
typedef std::variant<std::string, GLint> output_name_t;

/**
 * @brief Name information of a buffer output
 *
 * This type represents both the name and integer location of an
 * output in a program.
 */
typedef std::tuple<std::string, GLint> output_name_info_t;

/**
 * @brief Compares a name information object with a name specification
 *
 * This is a partial equality relation where \p name_info == \p name if the value
 * currently contained in the \p name variant matches the corresponding field in
 * \p name_info.
 *
 * @param name_info Name and location to test
 * @param name      Name specification (name or location) to test
 *
 * @return true if \p name could be a specification for \p name_info, false otherwise
 */
bool operator==(const output_name_info_t &name_info, const output_name_t &name);

/**
 * @brief Compares a name information object with a name specification
 *
 * This is a partial equality relation where \p name_info == \p name if the value
 * currently contained in the \p name variant matches the corresponding field in
 * \p name_info. The result of this operator is the the negated result of this
 * equality relation.
 *
 * @param name_info Name and location to test
 * @param name      Name specification (name or location) to test
 *
 * @return false if \p name could be a specification for \p name_info, true otherwise
 */
bool operator!=(const output_name_info_t &name_info, const output_name_t &name);
} // namespace shadertoy

#endif /* _SHADERTOY_OUTPUT_NAME_HPP_ */
