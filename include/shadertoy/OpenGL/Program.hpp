#ifndef _SHADERTOY_OPENGL_PROGRAM_HPP_
#define _SHADERTOY_OPENGL_PROGRAM_HPP_

#include "shadertoy/OpenGL/Resource.hpp"

namespace shadertoy
{
namespace OpenGL
{
	/**
	 * @brief Error thrown when an attempt is made to dereference a null program object.
	 */
	class shadertoy_EXPORT NullProgramError : public shadertoy::ShadertoyError
	{
	public:
		/**
		 * @brief Initializes a new instance of the NullProgramError class.
		 */
		explicit NullProgramError();
	};

	/**
	 * @brief Represents the location of an uniform in a program.
	 */
	class shadertoy_EXPORT UniformLocation
	{
	public:
		/**
		 * @brief Initializes a new instance of the UniformLocation class.
		 *
		 * @param program  Program this location is defined in
		 * @param location Location of the uniform
		 */
		UniformLocation(const Program &program, GLint location);

		/**
		 * @brief Returns a value indicating if this uniform location is active in its
		 * program.
		 *
		 * @return true if the uniform is active, false otherwise.
		 */
		bool IsActive() const;

		/**
		 * @brief glProgramUniform1i
		 *
		 * @param  v0 v0
		 * @return    true if the value was set, false otherwise
		 */
		bool SetValue(const GLint &v0);
		/**
		 * @brief glProgramUniform1f
		 *
		 * @param  v0 v0
		 * @return    true if the value was set, false otherwise
		 */
		bool SetValue(const GLfloat &v0);
		/**
		 * @brief glProgramUniform2f
		 *
		 * @param  v  v0 and v1 as a vec2
		 * @return    true if the value was set, false otherwise
		 */
		bool SetValue(const glm::vec2 &v);
		/**
		 * @brief glProgramUniform3f
		 *
		 * @param  v  v0 to v2 as a vec3
		 * @return    true if the value was set, false otherwise
		 */
		bool SetValue(const glm::vec3 &v);
		/**
		 * @brief glProgramUniform4f
		 *
		 * @param  v  v0 to v4 as a vec4
		 * @return    true if the value was set, false otherwise
		 */
		bool SetValue(const glm::vec4 &v);

		/**
		 * @brief glProgramUniform1iv
		 *
		 * @param  count count
		 * @param  v0    v0
		 * @return       true if the value was set, false otherwise
		 */
		bool SetValue(size_t count, const GLint *v0);
		/**
		 * @brief glProgramUniform1fv
		 *
		 * @param  count count
		 * @param  v0    v0
		 * @return       true if the value was set, false otherwise
		 */
		bool SetValue(size_t count, const GLfloat *v0);
		/**
		 * @brief glProgramUniform2fv
		 *
		 * @param  count count
		 * @param  v     v0, v1
		 * @return       true if the value was set, false otherwise
		 */
		bool SetValue(size_t count, const glm::vec2 *v);
		/**
		 * @brief glProgramUniform3fv
		 *
		 * @param  count count
		 * @param  v     v0, v1, v2
		 * @return       true if the value was set, false otherwise
		 */
		bool SetValue(size_t count, const glm::vec3 *v);
		/**
		 * @brief glProgramUniform4fv
		 *
		 * @param  count count
		 * @param  v     v0, v1, v2, v3
		 * @return       true if the value was set, false otherwise
		 */
		bool SetValue(size_t count, const glm::vec4 *v);

	private:
		/// Program id
		const GLuint _program;
		/// Uniform location
		const GLint _location;
	};

	/**
	 * @brief Error thrown when the linking step of a program fails.
	 */
	class shadertoy_EXPORT ProgramLinkError : public shadertoy::ShadertoyError
	{
	public:
		/**
		 * @brief Initializes a new instance of the ProgramLinkError class.
		 *
		 * @param  programId OpenGL resource id of the failed program
		 * @param  log       Contents of the link step log
		 */
		explicit ProgramLinkError(GLuint programId, const std::string &log);

		/**
		 * @brief Get the program id of the failed linking step.
		 *
		 * @return Id of the program that failed the linking step.
		 */
		GLuint programId() const
		{ return _programId; }

		/**
		 * @brief Get the log of the linking step.
		 *
		 * @return Contents of the program linking log.
		 */
		const std::string &log() const
		{ return _log; }

	private:
		/// Program id
		const GLuint _programId;
		/// Program link log
		const std::string _log;
	};

	/**
	 * @brief Error thrown when the validation step of a program fails.
	 */
	class shadertoy_EXPORT ProgramValidateError : public shadertoy::ShadertoyError
	{
	public:
		/**
		 * @brief Initializes a new instance of the ProgramValidateError class.
		 *
		 * @param  programId OpenGL resource id of the failed program
		 * @param  log       Contents of the validate step log
		 */
		explicit ProgramValidateError(GLuint programId, const std::string &log);

		/**
		 * @brief Get the program id of the failed validation step.
		 *
		 * @return Id of the program that failed the validation step.
		 */
		GLuint programId() const
		{ return _programId; }

		/**
		 * @brief Get the log of the validation step.
		 *
		 * @return Contents of the program validation log.
		 */
		const std::string &log() const
		{ return _log; }

	private:
		/// Program id
		const GLuint _programId;
		/// Program link log
		const std::string _log;
	};

	/**
	 * @brief Represents an OpenGL program.
	 */
	class shadertoy_EXPORT Program : public Resource<
		Program,
		SingleAllocator<&glCreateProgram, &glDeleteProgram>,
		NullProgramError>
	{
	public:
		/**
		 * @brief glLinkProgram
		 *
		 * @throws OpenGLError
		 * @throws ProgramLinkError
		 */
		void Link();

		/**
		 * @brief glUseProgram
		 *
		 * @throws OpenGLError
		 */
		void Use();

		/**
		 * @brief glValidateProgram
		 *
		 * @throws OpenGLError
		 * @throws ProgramValidateError
		 */
		void Validate();

		/**
		 * @brief glGetUniformLocation
		 *
		 * @param  name Name of the uniform
		 * @return      Uniform location object that can be used to set the
		 *              value of this uniform.
		 *
		 * @throws OpenGLError
		 */
		UniformLocation GetUniformLocation(const GLchar *name);

		/**
		 * @brief glAttachShader
		 *
		 * @param shader Shader object to attach
		 *
		 * @throws OpenGLError
		 * @throws NullShaderError
		 */
		void AttachShader(const Shader &shader);

		/**
		 * @brief glGetProgramInfoLog
		 *
		 * @return Contents of the information log
		 *
		 * @throws OpenGLError
		 */
		std::string Log();
	};
}
}

#endif /* _SHADERTOY_OPENGL_PROGRAM_HPP_ */
