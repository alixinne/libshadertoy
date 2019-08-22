#include "shadertoy/backends/gx/backend.hpp"

#include "shadertoy/program_interface.hpp"

using namespace shadertoy;
using namespace shadertoy::backends::gx;

program_resource::program_resource(const backends::gx::program &program, GLenum program_interface, GLuint resource_index)
: program_interface(program_interface), resource_index(resource_index)
{
	// Prepare properties array
	const GLenum properties[] = { GL_NAME_LENGTH, GL_LOCATION, GL_TYPE, GL_ARRAY_SIZE };
	constexpr const size_t propCount = sizeof(properties) / sizeof(properties[0]);

	// Property values array
	GLint values[propCount];

	// Get properties
	program.get_program_resource(program_interface, resource_index, propCount,
								 std::begin(properties), propCount, nullptr, std::begin(values));

	// Move values into fields
	location = values[1];
	type = values[2];
	array_size = values[3];

	// Fetch name
	name = std::string(values[0] - 1, ' ');
	program.get_program_resource_name(program_interface, resource_index, values[0], nullptr, name.data());
}

program_resource::~program_resource() = default;

uniform_variant uniform_resource::make_variant() const
{
	switch (type)
	{
		case GL_FLOAT: return float{0.0f};
		case GL_FLOAT_VEC2: return glm::vec2(0.0f);
		case GL_FLOAT_VEC3: return glm::vec3(0.0f);
		case GL_FLOAT_VEC4: return glm::vec4(0.0f);
		case GL_INT: return int{0};
		case GL_INT_VEC2: return glm::ivec2(0);
		case GL_INT_VEC3: return glm::ivec3(0);
		case GL_INT_VEC4: return glm::ivec4(0);
		case GL_UNSIGNED_INT: return (unsigned int)(0);
		case GL_UNSIGNED_INT_VEC2: return glm::uvec2(0);
		case GL_UNSIGNED_INT_VEC3: return glm::uvec3(0);
		case GL_UNSIGNED_INT_VEC4: return glm::uvec4(0);
		case GL_BOOL: return false;
		case GL_BOOL_VEC2: return glm::bvec2(false);
		case GL_BOOL_VEC3: return glm::bvec3(false);
		case GL_BOOL_VEC4: return glm::bvec4(false);
		case GL_FLOAT_MAT2: return glm::mat2(0);
		case GL_FLOAT_MAT3: return glm::mat3(0);
		case GL_FLOAT_MAT4: return glm::mat4(0);
		case GL_FLOAT_MAT2x3: return glm::mat2x3(0);
		case GL_FLOAT_MAT2x4: return glm::mat2x4(0);
		case GL_FLOAT_MAT3x2: return glm::mat3x2(0);
		case GL_FLOAT_MAT4x2: return glm::mat4x2(0);
		case GL_FLOAT_MAT4x3: return glm::mat4x3(0);
		case GL_SAMPLER_1D:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
		case GL_SAMPLER_1D_SHADOW:
		case GL_SAMPLER_2D_SHADOW:
		case GL_SAMPLER_1D_ARRAY:
		case GL_SAMPLER_2D_ARRAY:
		case GL_SAMPLER_1D_ARRAY_SHADOW:
		case GL_SAMPLER_2D_ARRAY_SHADOW:
		case GL_SAMPLER_2D_MULTISAMPLE:
		case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
		case GL_SAMPLER_CUBE_SHADOW:
		case GL_SAMPLER_BUFFER:
		case GL_SAMPLER_2D_RECT:
		case GL_SAMPLER_2D_RECT_SHADOW:
		case GL_INT_SAMPLER_1D:
		case GL_INT_SAMPLER_2D:
		case GL_INT_SAMPLER_3D:
		case GL_INT_SAMPLER_CUBE:
		case GL_INT_SAMPLER_1D_ARRAY:
		case GL_INT_SAMPLER_2D_ARRAY:
		case GL_INT_SAMPLER_2D_MULTISAMPLE:
		case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
		case GL_INT_SAMPLER_BUFFER:
		case GL_INT_SAMPLER_2D_RECT:
		case GL_UNSIGNED_INT_SAMPLER_1D:
		case GL_UNSIGNED_INT_SAMPLER_2D:
		case GL_UNSIGNED_INT_SAMPLER_3D:
		case GL_UNSIGNED_INT_SAMPLER_CUBE:
		case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
		case GL_UNSIGNED_INT_SAMPLER_BUFFER:
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
			return int{0};
		default:
			throw shadertoy_error("Unsupported uniform type");
	}
}

std::unique_ptr<uniform_location> uniform_resource::get_location(const backends::gx::program &program) const
{
	return backends::current->make_uniform_location(program, location);
}

uniform_resource::uniform_resource(const backends::gx::program &program, GLuint resource_index)
: program_resource(program, INTERFACE_TYPE, resource_index)
{
}

input_resource::input_resource(const backends::gx::program &program, GLuint resource_index)
: program_resource(program, INTERFACE_TYPE, resource_index)
{
}

output_resource::output_resource(const backends::gx::program &program, GLuint resource_index)
: program_resource(program, INTERFACE_TYPE, resource_index)
{
}

program_interface::program_interface(const backends::gx::program &program)
	: program_(program),
	uniforms_(program_),
	inputs_(program_),
	outputs_(program_)
{}

