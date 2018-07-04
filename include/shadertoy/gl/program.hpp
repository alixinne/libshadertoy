#ifndef _SHADERTOY_GL_PROGRAM_HPP_
#define _SHADERTOY_GL_PROGRAM_HPP_

#include "shadertoy/gl/resource.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/mat2x3.hpp>
#include <glm/mat3x2.hpp>
#include <glm/mat2x4.hpp>
#include <glm/mat4x2.hpp>
#include <glm/mat3x4.hpp>
#include <glm/mat4x3.hpp>

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
	 * @brief Represents the location of an attribute.
	 */
	class shadertoy_EXPORT attrib_location
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
		void vertex_pointer(GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer) const;

		/**
		 * @brief glEnableVertexAttribArray
		 */
		void enable_vertex_array() const;

	private:
		/// Attrib location
		const GLint location_;
	};

	/**
	 * @brief Represents the location of an uniform in a program.
	 */
	class shadertoy_EXPORT uniform_location
	{
		template<typename Base, typename Ptr>
		inline bool set_program_value(void (*SetterFunction)(GLuint, GLint, GLsizei, const Base *), size_t count, const Ptr *v) const
		{
			if (is_active())
			{
				gl_call(SetterFunction, program_, location_, count, reinterpret_cast<const Base *>(v));
				return true;
			}

			return false;
		}

		template<typename Base, typename Ptr>
		inline bool set_program_matrix(void (*SetterFunction)(GLuint, GLint, GLsizei, GLboolean, const Base *), size_t count, const Ptr *v) const
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
		uniform_location(const program &program, GLint location);

		/**
		 * @brief Return a value indicating if this uniform location is active in its
		 * program.
		 *
		 * @return true if the uniform is active, false otherwise.
		 */
		bool is_active() const;

		/**
		 * @brief glProgramUniformNT
		 *
		 * Implements glProgramUniform for the generic type T with N components
		 *
		 * @param  v Value to set
		 * @return true if the value was set, false otherwise
		 */
		template<typename T>
		bool set_value(const T &v) const
		{
			return set_value(1, &v);
		}

		/**
		 * @brief glProgramUniform1iv
		 *
		 * @param  count count
		 * @param  v0    v0
		 * @return       true if the value was set, false otherwise
		 */
		inline bool set_value(size_t count, const GLint *v0) const
		{ return set_program_value(glProgramUniform1iv, count, v0); }

		/**
		 * @brief glProgramUniform2iv
		 *
		 * @param  count count
		 * @param  v     v
		 * @return       true if the value was set, false otherwise
		 */
		inline bool set_value(size_t count, const glm::ivec2 *v) const
		{ return set_program_value(glProgramUniform2iv, count, v); }

		/**
		 * @brief glProgramUniform3iv
		 *
		 * @param  count count
		 * @param  v     v
		 * @return       true if the value was set, false otherwise
		 */
		inline bool set_value(size_t count, const glm::ivec3 *v) const
		{ return set_program_value(glProgramUniform3iv, count, v); }

		/**
		 * @brief glProgramUniform4iv
		 *
		 * @param  count count
		 * @param  v     v
		 * @return       true if the value was set, false otherwise
		 */
		inline bool set_value(size_t count, const glm::ivec4 *v) const
		{ return set_program_value(glProgramUniform4iv, count, v); }

		/**
		 * @brief glProgramUniform1fv
		 *
		 * @param  count count
		 * @param  v0    v0
		 * @return       true if the value was set, false otherwise
		 */
		inline bool set_value(size_t count, const GLfloat *v0) const
		{ return set_program_value(glProgramUniform1fv, count, v0); }

		/**
		 * @brief glProgramUniform2fv
		 *
		 * @param  count count
		 * @param  v     v
		 * @return       true if the value was set, false otherwise
		 */
		inline bool set_value(size_t count, const glm::vec2 *v) const
		{ return set_program_value(glProgramUniform2fv, count, v); }

		/**
		 * @brief glProgramUniform3fv
		 *
		 * @param  count count
		 * @param  v     v
		 * @return       true if the value was set, false otherwise
		 */
		inline bool set_value(size_t count, const glm::vec3 *v) const
		{ return set_program_value(glProgramUniform3fv, count, v); }

		/**
		 * @brief glProgramUniform4fv
		 *
		 * @param  count count
		 * @param  v     v
		 * @return       true if the value was set, false otherwise
		 */
		inline bool set_value(size_t count, const glm::vec4 *v) const
		{ return set_program_value(glProgramUniform4fv, count, v); }

		/**
		 * @brief glProgramUniform1uiv
		 *
		 * @param  count count
		 * @param  v0    v0
		 * @return       true if the value was set, false otherwise
		 */
		inline bool set_value(size_t count, const GLuint *v0) const
		{ return set_program_value(glProgramUniform1uiv, count, v0); }

		/**
		 * @brief glProgramUniform2uiv
		 *
		 * @param  count count
		 * @param  v     v
		 * @return       true if the value was set, false otherwise
		 */
		inline bool set_value(size_t count, const glm::uvec2 *v) const
		{ return set_program_value(glProgramUniform2uiv, count, v); }

		/**
		 * @brief glProgramUniform3uiv
		 *
		 * @param  count count
		 * @param  v     v
		 * @return       true if the value was set, false otherwise
		 */
		inline bool set_value(size_t count, const glm::uvec3 *v) const
		{ return set_program_value(glProgramUniform3uiv, count, v); }

		/**
		 * @brief glProgramUniform4uiv
		 *
		 * @param  count count
		 * @param  v     v
		 * @return       true if the value was set, false otherwise
		 */
		inline bool set_value(size_t count, const glm::uvec4 *v) const
		{ return set_program_value(glProgramUniform4uiv, count, v); }

		/**
		 * @brief glProgramUniformMatrix2fv
		 *
		 * @param count count
		 * @param v     v
		 * @return      true if the value was set, false otherwise
		 */
		inline bool set_value(size_t count, const glm::mat2 *v) const
		{ return set_program_matrix(glProgramUniformMatrix2fv, count, v); }

		/**
		 * @brief glProgramUniformMatrix3fv
		 *
		 * @param count count
		 * @param v     v
		 * @return      true if the value was set, false otherwise
		 */
		inline bool set_value(size_t count, const glm::mat3 *v) const
		{ return set_program_matrix(glProgramUniformMatrix3fv, count, v); }

		/**
		 * @brief glProgramUniformMatrix4fv
		 *
		 * @param count count
		 * @param v     v
		 * @return      true if the value was set, false otherwise
		 */
		inline bool set_value(size_t count, const glm::mat4 *v) const
		{ return set_program_matrix(glProgramUniformMatrix4fv, count, v); }

		/**
		 * @brief glProgramUniformMatrix2x3fv
		 *
		 * @param count count
		 * @param v     v
		 * @return      true if the value was set, false otherwise
		 */
		inline bool set_value(size_t count, const glm::mat2x3 *v) const
		{ return set_program_matrix(glProgramUniformMatrix2x3fv, count, v); }

		/**
		 * @brief glProgramUniformMatrix3x2fv
		 *
		 * @param count count
		 *
		 * @param v     v
		 * @return      true if the value was set, false otherwise
		 *
		 */
		inline bool set_value(size_t count, const glm::mat3x2 *v) const
		{ return set_program_matrix(glProgramUniformMatrix3x2fv, count, v); }

		/**
		 * @brief glProgramUniformMatrix2x4fv
		 *
		 * @param count count
		 *
		 * @param v     v
		 * @return      true if the value was set, false otherwise
		 *
		 */
		inline bool set_value(size_t count, const glm::mat2x4 *v) const
		{ return set_program_matrix(glProgramUniformMatrix2x4fv, count, v); }

		/**
		 * @brief glProgramUniformMatrix4x2fv
		 *
		 * @param count count
		 *
		 * @param v     v
		 * @return      true if the value was set, false otherwise
		 *
		 */
		inline bool set_value(size_t count, const glm::mat4x2 *v) const
		{ return set_program_matrix(glProgramUniformMatrix4x2fv, count, v); }

		/**
		 * @brief glProgramUniformMatrix3x4fv
		 *
		 * @param count count
		 *
		 * @param v     v
		 * @return      true if the value was set, false otherwise
		 *
		 */
		inline bool set_value(size_t count, const glm::mat3x4 *v) const
		{ return set_program_matrix(glProgramUniformMatrix3x4fv, count, v); }

		/**
		 * @brief glProgramUniformMatrix4x3fv
		 *
		 * @param count count
		 *
		 * @param v     v
		 * @return      true if the value was set, false otherwise
		 *
		 */
		inline bool set_value(size_t count, const glm::mat4x3 *v) const
		{ return set_program_matrix(glProgramUniformMatrix4x3fv, count, v); }

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
		program() : resource() {}
		program(resource_type &&other) : resource(std::forward<resource_type &&>(other)) {}
		resource_type &operator=(resource_type &&other) { return assign_operator(std::forward<resource_type &&>(other)); }

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
