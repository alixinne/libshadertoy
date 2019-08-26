#ifndef _SHADERTOY_BACKENDS_GL4_PROGRAM_HPP_
#define _SHADERTOY_BACKENDS_GL4_PROGRAM_HPP_

#include "shadertoy/backends/gl4/pre.hpp"
#include "shadertoy/backends/gl4/resource.hpp"
#include "shadertoy/backends/gx/program.hpp"

#include "shadertoy/shadertoy_error.hpp"

#include <algorithm>
#include <vector>

SHADERTOY_BACKENDS_GL4_NAMESPACE_BEGIN
class program;

/**
 * @brief Error thrown when an attempt is made to dereference a null program object.
 */
class shadertoy_EXPORT null_program_error : public shadertoy::shadertoy_error
{
	public:
	/**
	 * @brief Initialize a new instance of the null_program_error class.
	 */
	explicit null_program_error();
};

/**
 * @brief Represents the location of an attribute.
 */
class shadertoy_EXPORT attrib_location : public gx::attrib_location
{
	public:
	/**
	 * @brief Initialize a new instance of the attrib_location class.
	 *
	 * @param location Location of the attrib
	 */
	attrib_location(GLint location);

	/**
	 * @brief glVertexAttribPointer
	 *
	 * @param size       Size
	 * @param type       Type
	 * @param normalized Normalized
	 * @param stride     Stride
	 * @param pointer    Pointer
	 */
	void vertex_pointer(GLint size, GLenum type, GLboolean normalized, GLsizei stride,
						const GLvoid *pointer) const override;

	/**
	 * @brief glEnableVertexAttribArray
	 */
	void enable_vertex_array() const override;

	private:
	/// Attrib location
	const GLint location_;
};

/**
 * @brief Represents the location of an uniform in a program.
 */
class shadertoy_EXPORT uniform_location : public gx::uniform_location
{
	template <typename Base, typename Ptr>
	inline bool set_program_value(void (*SetterFunction)(GLuint, GLint, GLsizei, const Base *),
								  size_t count, const Ptr *v) const
	{
		if (is_active())
		{
			gl_call(SetterFunction, program_, location_, count, reinterpret_cast<const Base *>(v));
			return true;
		}

		return false;
	}

	template <typename Base, typename Ptr>
	inline bool set_program_matrix(void (*SetterFunction)(GLuint, GLint, GLsizei, GLboolean, const Base *),
								   size_t count, const Ptr *v) const
	{
		if (is_active())
		{
			gl_call(SetterFunction, program_, location_, count, GL_FALSE, reinterpret_cast<const Base *>(v));
			return true;
		}

		return false;
	}

	public:
	/**
	 * @brief Initialize a new instance of the UniformLocation class.
	 *
	 * @param program  Program this location is defined in
	 * @param location Location of the uniform
	 */
	uniform_location(const gx::program &program, GLint location);

	/**
	 * @brief Return a value indicating if this uniform location is active in its
	 * program.
	 *
	 * @return true if the uniform is active, false otherwise.
	 */
	bool is_active() const override;

	/**
	 * @brief glProgramUniform1iv
	 *
	 * @param  count count
	 * @param  v0    v0
	 * @return       true if the value was set, false otherwise
	 */
	inline bool set_value(size_t count, const GLint *v0) const override
	{
		return set_program_value(glProgramUniform1iv, count, v0);
	}

	/**
	 * @brief glProgramUniform2iv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	inline bool set_value(size_t count, const glm::ivec2 *v) const override
	{
		return set_program_value(glProgramUniform2iv, count, v);
	}

	/**
	 * @brief glProgramUniform3iv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	inline bool set_value(size_t count, const glm::ivec3 *v) const override
	{
		return set_program_value(glProgramUniform3iv, count, v);
	}

	/**
	 * @brief glProgramUniform4iv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	inline bool set_value(size_t count, const glm::ivec4 *v) const override
	{
		return set_program_value(glProgramUniform4iv, count, v);
	}

	/**
	 * @brief glProgramUniform1iv
	 *
	 * @param  count count
	 * @param  v0    v0
	 * @return       true if the value was set, false otherwise
	 */
	inline bool set_value(size_t count, const bool *v0) const override
	{
		GLint val(*v0);
		return set_program_value(glProgramUniform1iv, count, &val);
	}

	/**
	 * @brief glProgramUniform2iv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	inline bool set_value(size_t count, const glm::bvec2 *v) const override
	{
		std::vector<glm::ivec2> vec;
		vec.reserve(count);
		std::transform(v, v + count, std::back_inserter(vec),
					   [](auto &item) { return glm::ivec2(item); });
		return set_program_value(glProgramUniform2iv, count, vec.data());
	}

	/**
	 * @brief glProgramUniform3iv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	inline bool set_value(size_t count, const glm::bvec3 *v) const override
	{
		std::vector<glm::ivec3> vec;
		vec.reserve(count);
		std::transform(v, v + count, std::back_inserter(vec),
					   [](auto &item) { return glm::ivec3(item); });
		return set_program_value(glProgramUniform3iv, count, vec.data());
	}

	/**
	 * @brief glProgramUniform4iv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	inline bool set_value(size_t count, const glm::bvec4 *v) const override
	{
		std::vector<glm::ivec4> vec;
		vec.reserve(count);
		std::transform(v, v + count, std::back_inserter(vec),
					   [](auto &item) { return glm::ivec4(item); });
		return set_program_value(glProgramUniform4iv, count, vec.data());
	}

	/**
	 * @brief glProgramUniform1fv
	 *
	 * @param  count count
	 * @param  v0    v0
	 * @return       true if the value was set, false otherwise
	 */
	inline bool set_value(size_t count, const GLfloat *v0) const override
	{
		return set_program_value(glProgramUniform1fv, count, v0);
	}

	/**
	 * @brief glProgramUniform2fv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	inline bool set_value(size_t count, const glm::vec2 *v) const override
	{
		return set_program_value(glProgramUniform2fv, count, v);
	}

	/**
	 * @brief glProgramUniform3fv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	inline bool set_value(size_t count, const glm::vec3 *v) const override
	{
		return set_program_value(glProgramUniform3fv, count, v);
	}

	/**
	 * @brief glProgramUniform4fv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	inline bool set_value(size_t count, const glm::vec4 *v) const override
	{
		return set_program_value(glProgramUniform4fv, count, v);
	}

	/**
	 * @brief glProgramUniform1uiv
	 *
	 * @param  count count
	 * @param  v0    v0
	 * @return       true if the value was set, false otherwise
	 */
	inline bool set_value(size_t count, const GLuint *v0) const override
	{
		return set_program_value(glProgramUniform1uiv, count, v0);
	}

	/**
	 * @brief glProgramUniform2uiv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	inline bool set_value(size_t count, const glm::uvec2 *v) const override
	{
		return set_program_value(glProgramUniform2uiv, count, v);
	}

	/**
	 * @brief glProgramUniform3uiv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	inline bool set_value(size_t count, const glm::uvec3 *v) const override
	{
		return set_program_value(glProgramUniform3uiv, count, v);
	}

	/**
	 * @brief glProgramUniform4uiv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	inline bool set_value(size_t count, const glm::uvec4 *v) const override
	{
		return set_program_value(glProgramUniform4uiv, count, v);
	}

	/**
	 * @brief glProgramUniformMatrix2fv
	 *
	 * @param count count
	 * @param v     v
	 * @return      true if the value was set, false otherwise
	 */
	inline bool set_value(size_t count, const glm::mat2 *v) const override
	{
		return set_program_matrix(glProgramUniformMatrix2fv, count, v);
	}

	/**
	 * @brief glProgramUniformMatrix3fv
	 *
	 * @param count count
	 * @param v     v
	 * @return      true if the value was set, false otherwise
	 */
	inline bool set_value(size_t count, const glm::mat3 *v) const override
	{
		return set_program_matrix(glProgramUniformMatrix3fv, count, v);
	}

	/**
	 * @brief glProgramUniformMatrix4fv
	 *
	 * @param count count
	 * @param v     v
	 * @return      true if the value was set, false otherwise
	 */
	inline bool set_value(size_t count, const glm::mat4 *v) const override
	{
		return set_program_matrix(glProgramUniformMatrix4fv, count, v);
	}

	/**
	 * @brief glProgramUniformMatrix2x3fv
	 *
	 * @param count count
	 * @param v     v
	 * @return      true if the value was set, false otherwise
	 */
	inline bool set_value(size_t count, const glm::mat2x3 *v) const override
	{
		return set_program_matrix(glProgramUniformMatrix2x3fv, count, v);
	}

	/**
	 * @brief glProgramUniformMatrix3x2fv
	 *
	 * @param count count
	 *
	 * @param v     v
	 * @return      true if the value was set, false otherwise
	 *
	 */
	inline bool set_value(size_t count, const glm::mat3x2 *v) const override
	{
		return set_program_matrix(glProgramUniformMatrix3x2fv, count, v);
	}

	/**
	 * @brief glProgramUniformMatrix2x4fv
	 *
	 * @param count count
	 *
	 * @param v     v
	 * @return      true if the value was set, false otherwise
	 *
	 */
	inline bool set_value(size_t count, const glm::mat2x4 *v) const override
	{
		return set_program_matrix(glProgramUniformMatrix2x4fv, count, v);
	}

	/**
	 * @brief glProgramUniformMatrix4x2fv
	 *
	 * @param count count
	 *
	 * @param v     v
	 * @return      true if the value was set, false otherwise
	 *
	 */
	inline bool set_value(size_t count, const glm::mat4x2 *v) const override
	{
		return set_program_matrix(glProgramUniformMatrix4x2fv, count, v);
	}

	/**
	 * @brief glProgramUniformMatrix3x4fv
	 *
	 * @param count count
	 *
	 * @param v     v
	 * @return      true if the value was set, false otherwise
	 *
	 */
	inline bool set_value(size_t count, const glm::mat3x4 *v) const override
	{
		return set_program_matrix(glProgramUniformMatrix3x4fv, count, v);
	}

	/**
	 * @brief glProgramUniformMatrix4x3fv
	 *
	 * @param count count
	 *
	 * @param v     v
	 * @return      true if the value was set, false otherwise
	 *
	 */
	inline bool set_value(size_t count, const glm::mat4x3 *v) const override
	{
		return set_program_matrix(glProgramUniformMatrix4x3fv, count, v);
	}

	private:
	/// Program id
	const GLuint program_;
	/// Uniform location
	const GLint location_;
};

/**
 * @brief Represents an OpenGL program.
 */
class shadertoy_EXPORT program
: public resource<program, single_allocator<&glCreateProgram, &glDeleteProgram>, null_program_error, gx::program>
{
	public:
	program() : resource() {}
	program(resource_type &&other) : resource(std::forward<resource_type &&>(other)) {}
	resource_type &operator=(resource_type &&other)
	{
		return assign_operator(std::forward<resource_type &&>(other));
	}

	/**
	 * @brief glLinkProgram
	 *
	 * @throws opengl_error
	 * @throws ProgramLinkError
	 */
	void link() const override;

	/**
	 * @brief glUseProgram
	 *
	 * @throws opengl_error
	 */
	void use() const override;

	/**
	 * @brief glValidateProgram
	 *
	 * @throws opengl_error
	 * @throws program_validate_error
	 */
	void validate() const override;

	/**
	 * @brief glGetUniformLocation
	 *
	 * @param  name Name of the uniform
	 * @return      Uniform location object that can be used to set the
	 *              value of this uniform.
	 *
	 * @throws opengl_error
	 */
	std::unique_ptr<gx::uniform_location> get_uniform_location(const GLchar *name) const override;

	/**
	 * @brief glGetAttribLocation
	 *
	 * @param name Name of the attribute
	 * @return     Attribute location object that can be used to set this
	 *             attribute's properties.
	 *
	 * @throws opengl_error
	 */
	std::unique_ptr<gx::attrib_location> get_attrib_location(const GLchar *name) const override;

	/**
	 * @brief glAttachShader
	 *
	 * @param shader Shader object to attach
	 *
	 * @throws opengl_error
	 * @throws null_shader_error
	 */
	void attach_shader(const gx::shader &shader) const override;

	/**
	 * @brief glDetachShader
	 *
	 * @param shader Shader object to detach
	 *
	 * @throws opengl_error
	 * @throws null_shader_error
	 */
	void detach_shader(const gx::shader &shader) const override;

	/**
	 * @brief glGetProgramInfoLog
	 *
	 * @return Contents of the information log
	 *
	 * @throws opengl_error
	 */
	std::string log() const override;

	/**
	 * @brief glGetProgramiv
	 *
	 * @param pname  Parameter name
	 * @param params Parameters
	 *
	 * @throws opengl_error
	 * @throws null_program_error
	 */
	void get(GLenum pname, GLint *params) const override;

	/**
	 * @brief glGetProgramBinary
	 *
	 * @param bufsize      Program binary buffer size
	 * @param length       Program binary length
	 * @param binaryFormat Program binary format
	 * @param binary       Program binary
	 *
	 * @throws opengl_error
	 * @throws null_program_error
	 */
	void get_binary(GLsizei bufsize, GLsizei *length, GLenum *binaryFormat, void *binary) const override;

	/**
	 * @brief glGetProgramInterfaceiv
	 *
	 * @param programInterface Name of program interface to query
	 * @param pname            Parameter name
	 * @param params           Parameter return value
	 *
	 * @throws opengl_error
	 * @throws null_program_error
	 */
	void get_program_interface(GLenum programInterface, GLenum pname, GLint *params) const override;

	/**
	 * @brief glGetProgramResourceiv
	 *
	 * @param programInterface
	 * @param index
	 * @param propCount
	 * @param props
	 * @param bufSize
	 * @param length
	 * @param params
	 *
	 * @throws opengl_error
	 * @throws null_program_error
	 */
	void get_program_resource(GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props,
							  GLsizei bufSize, GLsizei *length, GLint *params) const override;

	/**
	 * @brief glGetProgramResourceName
	 *
	 * @param programInterface
	 * @param index
	 * @param bufSize
	 * @param length
	 * @param name
	 *
	 * @throws opengl_error
	 * @throws null_program_error
	 */
	void get_program_resource_name(GLenum programInterface, GLuint index, GLsizei bufSize,
								   GLsizei *length, GLchar *name) const override;

	/**
	 * @brief glDispatchCompute
	 *
	 * @param num_groups
	 */
	void dispatch_compute(glm::uvec3 num_groups) const override;
};
SHADERTOY_BACKENDS_GL4_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GL4_PROGRAM_HPP_ */
