#ifndef _SHADERTOY_GL_PROGRAM_HPP_
#define _SHADERTOY_GL_PROGRAM_HPP_

#include "shadertoy/gl/resource.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace shadertoy
{
namespace gl
{
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
	 * @brief Represents the location of an attribute in a program.
	 */
	class shadertoy_EXPORT attrib_location
	{
	public:
		/**
		 * @brief Initialize a new instance of the attrib_location class.
		 *
		 * @param program  Program this location is defined in
		 * @param location Location of the attrib
		 */
		attrib_location(const program &program, GLint location);

		/**
		 * @brief glVertexAttribPointer
		 *
		 * @param size       Size
		 * @param type       Type
		 * @param normalized Normalized
		 * @param stride     Stride
		 * @param pointer    Pointer
		 */
		void vertex_pointer(GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer) const;

		/**
		 * @brief glEnableVertexAttribArray
		 */
		void enable_vertex_array() const;

	private:
		/// Program id
		const GLuint program_;
		/// Attrib location
		const GLint location_;
	};

	/**
	 * @brief Represents the location of an uniform in a program.
	 */
	class shadertoy_EXPORT uniform_location
	{
	public:
		/**
		 * @brief Initialize a new instance of the UniformLocation class.
		 *
		 * @param program  Program this location is defined in
		 * @param location Location of the uniform
		 */
		uniform_location(const program &program, GLint location);

		/**
		 * @brief Return a value indicating if this uniform location is active in its
		 * program.
		 *
		 * @return true if the uniform is active, false otherwise.
		 */
		bool is_active() const;

		/**
		 * @brief glProgramUniform1i
		 *
		 * @param  v0 v0
		 * @return    true if the value was set, false otherwise
		 */
		bool set_value(const GLint &v0) const;
		/**
		 * @brief glProgramUniform1f
		 *
		 * @param  v0 v0
		 * @return    true if the value was set, false otherwise
		 */
		bool set_value(const GLfloat &v0) const;
		/**
		 * @brief glProgramUniform2f
		 *
		 * @param  v  v0 and v1 as a vec2
		 * @return    true if the value was set, false otherwise
		 */
		bool set_value(const glm::vec2 &v) const;
		/**
		 * @brief glProgramUniform3f
		 *
		 * @param  v  v0 to v2 as a vec3
		 * @return    true if the value was set, false otherwise
		 */
		bool set_value(const glm::vec3 &v) const;
		/**
		 * @brief glProgramUniform4f
		 *
		 * @param  v  v0 to v4 as a vec4
		 * @return    true if the value was set, false otherwise
		 */
		bool set_value(const glm::vec4 &v) const;

		/**
		 * @brief glProgramUniform1iv
		 *
		 * @param  count count
		 * @param  v0    v0
		 * @return       true if the value was set, false otherwise
		 */
		bool set_value(size_t count, const GLint *v0) const;
		/**
		 * @brief glProgramUniform1fv
		 *
		 * @param  count count
		 * @param  v0    v0
		 * @return       true if the value was set, false otherwise
		 */
		bool set_value(size_t count, const GLfloat *v0) const;
		/**
		 * @brief glProgramUniform2fv
		 *
		 * @param  count count
		 * @param  v     v0, v1
		 * @return       true if the value was set, false otherwise
		 */
		bool set_value(size_t count, const glm::vec2 *v) const;
		/**
		 * @brief glProgramUniform3fv
		 *
		 * @param  count count
		 * @param  v     v0, v1, v2
		 * @return       true if the value was set, false otherwise
		 */
		bool set_value(size_t count, const glm::vec3 *v) const;
		/**
		 * @brief glProgramUniform4fv
		 *
		 * @param  count count
		 * @param  v     v0, v1, v2, v3
		 * @return       true if the value was set, false otherwise
		 */
		bool set_value(size_t count, const glm::vec4 *v) const;

	private:
		/// Program id
		const GLuint program_;
		/// Uniform location
		const GLint location_;
	};

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
		explicit program_link_error(GLuint programId, const std::string &log);

		/**
		 * @brief Get the program id of the failed linking step.
		 *
		 * @return Id of the program that failed the linking step.
		 */
		GLuint program_id() const
		{ return program_id_; }

		/**
		 * @brief Get the log of the linking step.
		 *
		 * @return Contents of the program linking log.
		 */
		const std::string &log() const
		{ return log_; }

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
		explicit program_validate_error(GLuint programId, const std::string &log);

		/**
		 * @brief Get the program id of the failed validation step.
		 *
		 * @return Id of the program that failed the validation step.
		 */
		GLuint program_id() const
		{ return program_id_; }

		/**
		 * @brief Get the log of the validation step.
		 *
		 * @return Contents of the program validation log.
		 */
		const std::string &log() const
		{ return log_; }

	private:
		/// Program id
		const GLuint program_id_;
		/// Program link log
		const std::string log_;
	};

	/**
	 * @brief Represents an OpenGL program.
	 */
	class shadertoy_EXPORT program : public resource<
		program,
		single_allocator<&glCreateProgram, &glDeleteProgram>,
		null_program_error>
	{
	public:
		using base_resource_type::operator=;

		/**
		 * @brief glLinkProgram
		 *
		 * @throws opengl_error
		 * @throws ProgramLinkError
		 */
		void link() const;

		/**
		 * @brief glUseProgram
		 *
		 * @throws opengl_error
		 */
		void use() const;

		/**
		 * @brief glValidateProgram
		 *
		 * @throws opengl_error
		 * @throws program_validate_error
		 */
		void validate() const;

		/**
		 * @brief glGetUniformLocation
		 *
		 * @param  name Name of the uniform
		 * @return      Uniform location object that can be used to set the
		 *              value of this uniform.
		 *
		 * @throws opengl_error
		 */
		uniform_location get_uniform_location(const GLchar *name) const;

		/**
		 * @brief glGetAttribLocation
		 *
		 * @param name Name of the attribute
		 * @return     Attribute location object that can be used to set this
		 *             attribute's properties.
		 *
		 * @throws opengl_error
		 */
		attrib_location get_attrib_location(const GLchar *name) const;

		/**
		 * @brief glAttachShader
		 *
		 * @param shader Shader object to attach
		 *
		 * @throws opengl_error
		 * @throws null_shader_error
		 */
		void attach_shader(const shader &shader) const;

		/**
		 * @brief glDetachShader
		 *
		 * @param shader Shader object to detach
		 *
		 * @throws opengl_error
		 * @throws null_shader_error
		 */
		void detach_shader(const shader &shader) const;

		/**
		 * @brief glGetProgramInfoLog
		 *
		 * @return Contents of the information log
		 *
		 * @throws opengl_error
		 */
		std::string log() const;

		/**
		 * @brief glGetProgramiv
		 *
		 * @param pname  Parameter name
		 * @param params Parameters
		 *
		 * @throws opengl_error
		 * @throws null_program_error
		 */
		void get(GLenum pname, GLint *params) const;

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
		void get_binary(GLsizei bufsize, GLsizei *length, GLenum *binaryFormat, void *binary) const;
	};
}
}

#endif /* _SHADERTOY_GL_PROGRAM_HPP_ */
