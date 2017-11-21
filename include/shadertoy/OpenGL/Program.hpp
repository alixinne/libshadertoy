#ifndef _SHADERTOY_OPENGL_PROGRAM_HPP_
#define _SHADERTOY_OPENGL_PROGRAM_HPP_

#include "shadertoy/OpenGL/Resource.hpp"

namespace shadertoy
{
namespace OpenGL
{
	class shadertoy_EXPORT NullProgramError : public shadertoy::ShadertoyError
	{
	public:
		explicit NullProgramError();
	};

	class shadertoy_EXPORT UniformLocation
	{
	public:
		UniformLocation(const Program &program, GLint location);

		bool IsActive() const;

		// glProgramUniform
		// return true if the value was set
		bool SetValue(const GLint &v0);
		bool SetValue(const GLfloat &v0);
		bool SetValue(const glm::vec2 &v);
		bool SetValue(const glm::vec3 &v);
		bool SetValue(const glm::vec4 &v);

		bool SetValue(size_t count, const GLint *v0);
		bool SetValue(size_t count, const GLfloat *v0);
		bool SetValue(size_t count, const glm::vec2 *v);
		bool SetValue(size_t count, const glm::vec3 *v);
		bool SetValue(size_t count, const glm::vec4 *v);

	private:
		const GLuint _program;
		const GLint _location;
	};

	class shadertoy_EXPORT ProgramLinkError : public shadertoy::ShadertoyError
	{
	public:
		explicit ProgramLinkError(GLuint programId, const std::string &log);

		GLuint programId() const
		{ return _programId; }

		const std::string &log() const
		{ return _log; }

	private:
		const GLuint _programId;
		const std::string _log;
	};

	class shadertoy_EXPORT ProgramValidateError : public shadertoy::ShadertoyError
	{
	public:
		explicit ProgramValidateError(GLuint programId, const std::string &log);

		GLuint programId() const
		{ return _programId; }

		const std::string &log() const
		{ return _log; }

	private:
		const GLuint _programId;
		const std::string _log;
	};

	class shadertoy_EXPORT Program : public Resource<
		Program,
		SingleAllocator<&glCreateProgram, &glDeleteProgram>,
		NullProgramError>
	{
	public:
		// glLinkProgram
		void Link();

		// glUseProgram
		void Use();

		// glValidateProgram
		void Validate();

		// glGetUniformLocation
		UniformLocation GetUniformLocation(const GLchar *name);

		// glAttachShader
		void AttachShader(const Shader &shader);

		// glGetProgramInfoLog
		std::string Log();
	};
}
}

#endif /* _SHADERTOY_OPENGL_PROGRAM_HPP_ */
