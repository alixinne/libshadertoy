#ifndef _SHADERTOY_PROGRAM_INTERFACE_HPP_
#define _SHADERTOY_PROGRAM_INTERFACE_HPP_

#include "shadertoy/pre.hpp"

#include "shadertoy/backends/gx/program.hpp"

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include <glm/glm.hpp>

namespace shadertoy
{

/**
 * @brief Representation of an OpenGL program interface resource
 */
struct program_resource
{
	/// Program interface this resource is part of
	GLenum program_interface;

	/// Index in the interface this resource is located at
	GLuint resource_index;

	/// Textual name of the represented resource
	std::string name;

	/// Location of the resource
	GLint location;

	/// OpenGL type of the resource
	GLint type;

	/// Size of the array, 0 if the resource is not an array
	GLint array_size;

	virtual ~program_resource();

	protected:
	/**
	 * @brief Build a new program resource from its interface
	 *
	 * @param program           Program to query the resource from
	 * @param program_interface Name of the interface this resource belongs to
	 * @param resource_index    Index of the target resource in the program interface
	 */
	program_resource(const backends::gx::program &program, GLenum program_interface, GLuint resource_index);
};

/**
 * @brief OpenGL uniform value variant type
 */
typedef std::variant<int, glm::ivec2, glm::ivec3, glm::ivec4, unsigned int, glm::uvec2, glm::uvec3, glm::uvec4,
					 float, glm::vec2, glm::vec3, glm::vec4, bool, glm::bvec2, glm::bvec3, glm::bvec4, glm::mat2,
					 glm::mat3, glm::mat4, glm::mat2x3, glm::mat2x4, glm::mat3x2, glm::mat3x4, glm::mat4x2, glm::mat4x3>
uniform_variant;

/**
 * @brief Represents a uniform interface resource
 */
struct uniform_resource : public program_resource
{
	constexpr static const GLenum INTERFACE_TYPE = GL_UNIFORM;

	/**
	 * @brief Get a variant corresponding to this uniform declaration
	 *
	 * @return An instance of uniform_variant which runtime type is what would be represented
	 *         in the associated uniform. Its value is the default for the corresponding type.
	 */
	uniform_variant make_variant() const;

	/**
	 * @brief Get a backends::gx::uniform_location object for this uniform.
	 *
	 * This is what can be used to set the value of the uniform. Note that if the
	 * given \p program is not the one this resource has been queried from, the
	 * behavior will be undefined.
	 *
	 * @return uniform_location object pointing to this uniform in the given program
	 */
	std::unique_ptr<backends::gx::uniform_location> get_location(const backends::gx::program &program) const;

	/**
	 * @brief Build a new uniform resource from its interface
	 *
	 * @param program           Program to query the resource from
	 * @param resource_index    Index of the target resource in the program interface
	 */
	uniform_resource(const backends::gx::program &program, GLuint resource_index);
};

/**
 * @brief Represents a program input resource
 */
struct input_resource : public program_resource
{
	constexpr static const GLenum INTERFACE_TYPE = GL_PROGRAM_INPUT;

	/**
	 * @brief Build a new program input resource from its interface
	 *
	 * @param program           Program to query the resource from
	 * @param resource_index    Index of the target resource in the program interface
	 */
	input_resource(const backends::gx::program &program, GLuint resource_index);
};

/**
 * @brief Represents a program output resource
 */
struct output_resource : public program_resource
{
	constexpr static const GLenum INTERFACE_TYPE = GL_PROGRAM_OUTPUT;

	/**
	 * @brief Build a new program output resource from its interface
	 *
	 * @param program           Program to query the resource from
	 * @param resource_index    Index of the target resource in the program interface
	 */
	output_resource(const backends::gx::program &program, GLuint resource_index);
};

/**
 * @brief Generic program interface representation
 *
 * This class represents a set of resources which are part of a program interface.
 * The resources can be queried efficiently by name or location.
 *
 * @tparam T Type of the resources to manage
 */
template <typename T> class resource_interface
{
	/// List of resource objects
	std::vector<T> resources_;

	/// Map for lookup of resources by name
	std::unordered_map<std::string, size_t> resource_names_;

	/// Map for lookup of resources by location
	std::unordered_map<GLint, size_t> resource_locations_;

	public:
	/**
	 * @brief Build a new resource_interface object
	 *
	 * @param program Program to query the resources from
	 */
	resource_interface(const backends::gx::program &program)
	{
		// Get the number of active resources
		GLint active_resources;
		program.get_program_interface(T::INTERFACE_TYPE, GL_ACTIVE_RESOURCES, &active_resources);

		// Fetch resource details
		resources_.reserve(active_resources);
		for (int resource = 0; resource < active_resources; ++resource)
		{
			resources_.emplace_back(program, resource);
		}

		// Build lookup maps
		for (auto it = resources_.begin(); it != resources_.end(); ++it)
		{
			size_t idx = it - resources_.begin();
			resource_names_.insert(std::make_pair(it->name, idx));
			resource_locations_.insert(std::make_pair(it->location, idx));
		}
	}

	/**
	 * @brief Get the list of resources in this interface.
	 *
	 * @return Reference to the vector of resource objects in this interface
	 */
	const std::vector<T> &resources() const { return resources_; }

	/**
	 * @brief Get a resource by its location.
	 *
	 * This method will fail (out of bounds) if the requested location is not active.
	 *
	 * @param location Location of the resource to get
	 *
	 * @return Reference to the resource object corresponding to this location.
	 */
	const T &operator[](GLint location) const
	{
		return resources_[resource_locations_.at(location)];
	}


	/**
	 * @brief Get a resource by its name.
	 *
	 * This method will fail (out of bounds) if the requested name is not active.
	 *
	 * @param name Name of the resource to get
	 *
	 * @return Reference to the resource object corresponding to this name.
	 */
	const T &operator[](const std::string &name) const
	{
		return resources_[resource_names_.at(name)];
	}

	/**
	 * @brief Get a resource by its location.
	 *
	 * This method will fail (nullptr) if the requested location is not active.
	 *
	 * @param location Location of the resource to get
	 *
	 * @return Pointer to the resource object corresponding to this location, or null
	 *         if it was not found.
	 */
	const T *try_get(GLint location) const
	{
		auto it = resource_locations_.find(location);
		if (it == resource_locations_.end())
			return nullptr;

		return &resources_[it->second];
	}

	/**
	 * @brief Get a resource by its name.
	 *
	 * This method will fail (nullptr) if the requested name is not active.
	 *
	 * @param name Name of the resource to get
	 *
	 * @return Pointer to the resource object corresponding to this name, or null
	 *         if it was not found.
	 */
	const T *try_get(const std::string &name) const
	{
		auto it = resource_names_.find(name);
		if (it == resource_names_.end())
			return nullptr;

		return &resources_[it->second];
	}
};

/// Uniform resource interface type
typedef resource_interface<uniform_resource> uniform_interface;
/// Program input resource interface type
typedef resource_interface<input_resource> input_interface;
/// Program output resource interface type
typedef resource_interface<output_resource> output_interface;

/**
 * @brief Represents the set of interfaces of an OpenGL program
 *
 * The referenced \c program must remain valid as long as this object is alive.
 */
class program_interface
{
	const backends::gx::program &program_;

	uniform_interface uniforms_;
	input_interface inputs_;
	output_interface outputs_;

	public:
	program_interface(const backends::gx::program &program);

	/**
	 * @brief Get the program corresponding to this interface
	 */
	inline const backends::gx::program &program() const { return program_; }

	/**
	 * @brief Get the uniform resource interface
	 */
	inline const uniform_interface &uniforms() const { return uniforms_; }

	/**
	 * @brief Get the location of a uniform in this interface
	 *
	 * @param identifier Name or location of the uniform to find. See resource_interface#operator[]
	 * @tparam TIndex    Type of the identifier for resource finding
	 *
	 * @return An instance of backends::gx::uniform_location corresponding to this
	 * uniform.
	 */
	template <typename TIndex>
	inline std::unique_ptr<backends::gx::uniform_location> get_uniform_location(const TIndex &identifier) const
	{
		return uniforms_[identifier].get_location(program_);
	}

	/**
	 * @brief Try to get the location of a uniform in this interface
	 *
	 * @param identifier Name or location of the uniform to find. See resource_interface#operator[]
	 * @tparam TIndex    Type of the identifier for resource finding
	 *
	 * @return \c std::nullopt if the uniform could not be found, otherwise
	 * returns an instance of backends::gx::uniform_location corresponding to this
	 * uniform.
	 */
	template <typename TIndex>
	inline std::unique_ptr<backends::gx::uniform_location> try_get_uniform_location(const TIndex &identifier) const
	{
		if (auto resource = uniforms_.try_get(identifier))
		{
			return resource->get_location(program_);
		}

		return {};
	}

	/**
	 * @brief Get the inputs resource interface
	 */
	inline const input_interface &inputs() const { return inputs_; }

	/**
	 * @brief Get the outputs resource interface
	 */
	inline const output_interface &outputs() const { return outputs_; }
};

} // namespace shadertoy

#endif /* _SHADERTOY_PROGRAM_INTERFACE_HPP_ */
