#ifndef _SHADERTOY_BACKENDS_GX_PROGRAM_HPP_
#define _SHADERTOY_BACKENDS_GX_PROGRAM_HPP_

#include "shadertoy/backends/gx/pre.hpp"
#include "shadertoy/backends/gx/resource.hpp"

#include "shadertoy/shadertoy_error.hpp"

#include "shadertoy/backends/gx/shader.hpp"

#include <glm/mat2x2.hpp>
#include <glm/mat2x3.hpp>
#include <glm/mat2x4.hpp>
#include <glm/mat3x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat3x4.hpp>
#include <glm/mat4x2.hpp>
#include <glm/mat4x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <memory>

SHADERTOY_BACKENDS_GX_NAMESPACE_BEGIN
class program;

/**
 * @brief Error thrown when the linking step of a program fails.
 */
class shadertoy_EXPORT program_link_error : public shadertoy::shadertoy_error
{
	public:
	/**
	 * @brief Initialize a new instance of the program_link_error class.
	 *
	 * @param  programId OpenGL resource id of the failed program
	 * @param  log       Contents of the link step log
	 */
	explicit program_link_error(GLuint programId, std::string log);

	/**
	 * @brief Get the program id of the failed linking step.
	 *
	 * @return Id of the program that failed the linking step.
	 */
	GLuint program_id() const { return program_id_; }

	/**
	 * @brief Get the log of the linking step.
	 *
	 * @return Contents of the program linking log.
	 */
	const std::string &log() const { return log_; }

	private:
	/// Program id
	const GLuint program_id_;
	/// Program link log
	const std::string log_;
};

/**
 * @brief Error thrown when the validation step of a program fails.
 */
class shadertoy_EXPORT program_validate_error : public shadertoy::shadertoy_error
{
	public:
	/**
	 * @brief Initialize a new instance of the program_validate_error class.
	 *
	 * @param  programId OpenGL resource id of the failed program
	 * @param  log       Contents of the validate step log
	 */
	explicit program_validate_error(GLuint programId, std::string log);

	/**
	 * @brief Get the program id of the failed validation step.
	 *
	 * @return Id of the program that failed the validation step.
	 */
	GLuint program_id() const { return program_id_; }

	/**
	 * @brief Get the log of the validation step.
	 *
	 * @return Contents of the program validation log.
	 */
	const std::string &log() const { return log_; }

	private:
	/// Program id
	const GLuint program_id_;
	/// Program link log
	const std::string log_;
};

/**
 * @brief Represents the location of an attribute.
 */
class shadertoy_EXPORT attrib_location
{
	public:
	virtual ~attrib_location();

	/**
	 * @brief glVertexAttribPointer
	 *
	 * @param size       Size
	 * @param type       Type
	 * @param normalized Normalized
	 * @param stride     Stride
	 * @param pointer    Pointer
	 */
	virtual void vertex_pointer(GLint size, GLenum type, GLboolean normalized, GLsizei stride,
								const GLvoid *pointer) const = 0;

	/**
	 * @brief glEnableVertexAttribArray
	 */
	virtual void enable_vertex_array() const = 0;
};

/**
 * @brief Represents the location of an uniform in a program.
 */
class shadertoy_EXPORT uniform_location
{
	public:
	virtual ~uniform_location();

	/**
	 * @brief Return a value indicating if this uniform location is active in its
	 * program.
	 *
	 * @return true if the uniform is active, false otherwise.
	 */
	virtual bool is_active() const = 0;

	/**
	 * @brief glProgramUniformNT
	 *
	 * Implements glProgramUniform for the generic type T with N components
	 *
	 * @param  v Value to set
	 * @return true if the value was set, false otherwise
	 */
	template <typename T> bool set_value(const T &v) const { return set_value(1, &v); }

	/**
	 * @brief glProgramUniform1iv
	 *
	 * @param  count count
	 * @param  v0    v0
	 * @return       true if the value was set, false otherwise
	 */
	virtual bool set_value(size_t count, const int *v0) const = 0;

	/**
	 * @brief glProgramUniform2iv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	virtual bool set_value(size_t count, const glm::ivec2 *v) const = 0;

	/**
	 * @brief glProgramUniform3iv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	virtual bool set_value(size_t count, const glm::ivec3 *v) const = 0;

	/**
	 * @brief glProgramUniform4iv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	virtual bool set_value(size_t count, const glm::ivec4 *v) const = 0;

	/**
	 * @brief glProgramUniform1iv
	 *
	 * @param  count count
	 * @param  v0    v0
	 * @return       true if the value was set, false otherwise
	 */
	virtual bool set_value(size_t count, const bool *v0) const = 0;

	/**
	 * @brief glProgramUniform2iv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	virtual bool set_value(size_t count, const glm::bvec2 *v) const = 0;

	/**
	 * @brief glProgramUniform3iv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	virtual bool set_value(size_t count, const glm::bvec3 *v) const = 0;

	/**
	 * @brief glProgramUniform4iv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	virtual bool set_value(size_t count, const glm::bvec4 *v) const = 0;

	/**
	 * @brief glProgramUniform1fv
	 *
	 * @param  count count
	 * @param  v0    v0
	 * @return       true if the value was set, false otherwise
	 */
	virtual bool set_value(size_t count, const float *v0) const = 0;

	/**
	 * @brief glProgramUniform2fv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	virtual bool set_value(size_t count, const glm::vec2 *v) const = 0;

	/**
	 * @brief glProgramUniform3fv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	virtual bool set_value(size_t count, const glm::vec3 *v) const = 0;

	/**
	 * @brief glProgramUniform4fv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	virtual bool set_value(size_t count, const glm::vec4 *v) const = 0;

	/**
	 * @brief glProgramUniform1uiv
	 *
	 * @param  count count
	 * @param  v0    v0
	 * @return       true if the value was set, false otherwise
	 */
	virtual bool set_value(size_t count, const unsigned int *v0) const = 0;

	/**
	 * @brief glProgramUniform2uiv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	virtual bool set_value(size_t count, const glm::uvec2 *v) const = 0;

	/**
	 * @brief glProgramUniform3uiv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	virtual bool set_value(size_t count, const glm::uvec3 *v) const = 0;

	/**
	 * @brief glProgramUniform4uiv
	 *
	 * @param  count count
	 * @param  v     v
	 * @return       true if the value was set, false otherwise
	 */
	virtual bool set_value(size_t count, const glm::uvec4 *v) const = 0;

	/**
	 * @brief glProgramUniformMatrix2fv
	 *
	 * @param count count
	 * @param v     v
	 * @return      true if the value was set, false otherwise
	 */
	virtual bool set_value(size_t count, const glm::mat2 *v) const = 0;

	/**
	 * @brief glProgramUniformMatrix3fv
	 *
	 * @param count count
	 * @param v     v
	 * @return      true if the value was set, false otherwise
	 */
	virtual bool set_value(size_t count, const glm::mat3 *v) const = 0;

	/**
	 * @brief glProgramUniformMatrix4fv
	 *
	 * @param count count
	 * @param v     v
	 * @return      true if the value was set, false otherwise
	 */
	virtual bool set_value(size_t count, const glm::mat4 *v) const = 0;

	/**
	 * @brief glProgramUniformMatrix2x3fv
	 *
	 * @param count count
	 * @param v     v
	 * @return      true if the value was set, false otherwise
	 */
	virtual bool set_value(size_t count, const glm::mat2x3 *v) const = 0;

	/**
	 * @brief glProgramUniformMatrix3x2fv
	 *
	 * @param count count
	 *
	 * @param v     v
	 * @return      true if the value was set, false otherwise
	 *
	 */
	virtual bool set_value(size_t count, const glm::mat3x2 *v) const = 0;

	/**
	 * @brief glProgramUniformMatrix2x4fv
	 *
	 * @param count count
	 *
	 * @param v     v
	 * @return      true if the value was set, false otherwise
	 *
	 */
	virtual bool set_value(size_t count, const glm::mat2x4 *v) const = 0;

	/**
	 * @brief glProgramUniformMatrix4x2fv
	 *
	 * @param count count
	 *
	 * @param v     v
	 * @return      true if the value was set, false otherwise
	 *
	 */
	virtual bool set_value(size_t count, const glm::mat4x2 *v) const = 0;

	/**
	 * @brief glProgramUniformMatrix3x4fv
	 *
	 * @param count count
	 *
	 * @param v     v
	 * @return      true if the value was set, false otherwise
	 *
	 */
	virtual bool set_value(size_t count, const glm::mat3x4 *v) const = 0;

	/**
	 * @brief glProgramUniformMatrix4x3fv
	 *
	 * @param count count
	 *
	 * @param v     v
	 * @return      true if the value was set, false otherwise
	 *
	 */
	virtual bool set_value(size_t count, const glm::mat4x3 *v) const = 0;
};

/**
 * @brief Represents an OpenGL program.
 */
class shadertoy_EXPORT program : public resource
{
	public:
	/**
	 * @brief glLinkProgram
	 *
	 * @throws opengl_error
	 * @throws ProgramLinkError
	 */
	virtual void link() const = 0;

	/**
	 * @brief glUseProgram
	 *
	 * @throws opengl_error
	 */
	virtual void use() const = 0;

	/**
	 * @brief glValidateProgram
	 *
	 * @throws opengl_error
	 * @throws program_validate_error
	 */
	virtual void validate() const = 0;

	/**
	 * @brief glGetUniformLocation
	 *
	 * @param  name Name of the uniform
	 * @return      Uniform location object that can be used to set the
	 *              value of this uniform.
	 *
	 * @throws opengl_error
	 */
	virtual std::unique_ptr<uniform_location> get_uniform_location(const GLchar *name) const = 0;

	/**
	 * @brief glGetAttribLocation
	 *
	 * @param name Name of the attribute
	 * @return     Attribute location object that can be used to set this
	 *             attribute's properties.
	 *
	 * @throws opengl_error
	 */
	virtual std::unique_ptr<attrib_location> get_attrib_location(const GLchar *name) const = 0;

	/**
	 * @brief glAttachShader
	 *
	 * @param shader Shader object to attach
	 *
	 * @throws opengl_error
	 * @throws null_shader_error
	 */
	virtual void attach_shader(const shader &shader) const = 0;

	/**
	 * @brief glDetachShader
	 *
	 * @param shader Shader object to detach
	 *
	 * @throws opengl_error
	 * @throws null_shader_error
	 */
	virtual void detach_shader(const shader &shader) const = 0;

	/**
	 * @brief glGetProgramInfoLog
	 *
	 * @return Contents of the information log
	 *
	 * @throws opengl_error
	 */
	virtual std::string log() const = 0;

	/**
	 * @brief glGetProgramiv
	 *
	 * @param pname  Parameter name
	 * @param params Parameters
	 *
	 * @throws opengl_error
	 * @throws null_program_error
	 */
	virtual void get(GLenum pname, GLint *params) const = 0;

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
	virtual void get_binary(GLsizei bufsize, GLsizei *length, GLenum *binaryFormat, void *binary) const = 0;

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
	virtual void get_program_interface(GLenum programInterface, GLenum pname, GLint *params) const = 0;

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
	virtual void get_program_resource(GLenum programInterface, GLuint index, GLsizei propCount,
									  const GLenum *props, GLsizei bufSize, GLsizei *length,
									  GLint *params) const = 0;

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
	virtual void get_program_resource_name(GLenum programInterface, GLuint index, GLsizei bufSize,
										   GLsizei *length, GLchar *name) const = 0;

	/**
	 * @brief glDispatchCompute
	 *
	 * TODO: Specify that it uses the program before dispatching.
	 *
	 * @param num_groups
	 */
	virtual void dispatch_compute(glm::uvec3 num_groups) const = 0;
};
SHADERTOY_BACKENDS_GX_NAMESPACE_END

#endif /* _SHADERTOY_BACKENDS_GX_PROGRAM_HPP_ */
