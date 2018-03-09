#include <algorithm>
#include <sstream>
#include <vector>

#include <epoxy/gl.h>

#include <glm/glm.hpp>

#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/gl.hpp"

#include "shadertoy/shader_compiler.hpp"

using namespace std;
using namespace shadertoy;

shader_compiler::shader_compiler()
	: named_sources_()
{
}

void shader_compiler::compile(gl::shader &shader)
{
	// Transform pairs into list of C strings
	vector<std::string> sources(named_sources_.size());
	transform(named_sources_.begin(), named_sources_.end(), sources.begin(),
		[] (const pair<string, string> &namedSource) {
			return namedSource.second;
		});

	// Also, build a line count
	vector<int> lineCounts(named_sources_.size());
	transform(named_sources_.begin(), named_sources_.end(), lineCounts.begin(),
		[] (const pair<string, string> &namedSource) {
			return count(namedSource.second.begin(),
						 namedSource.second.end(),
						 '\n');
		});

	// Load sources in fragment shader and compile
	try
	{
		shader.source(sources);
		shader.compile();
	}
	catch (gl::shader_compilation_error &ex)
	{
		// Reparse log and replace sources with their actual names
		std::stringstream is(ex.log()), os;
		std::string msg;

		while (!is.eof())
		{
			char c, d;
			size_t pid, pline;

			// Read one message
			getline(is, msg);

			// Try parsing message
			std::stringstream msgis(msg);

			// Matches ID(LINE.
			//         ID:LINE.
			msgis >> pid >> c >> pline >> d;

			if (msgis.fail())
			{
				// Parsing failed
				os << msg << endl;
			}
			else
			{
				// Parsing succeeded, find the source part which contains the
				// actual error
				size_t lc, lcn, li = 0;
				for (lc = 0, lcn = 0;
					 li < lineCounts.size();
					 li++)
				{
					lc = lcn;
					lcn += lineCounts.at(li);
					if (lcn > pline) break;
				}

				// If li exceeds known programs, an error occurred, pass the
				// message verbatim
				if (li >= lineCounts.size())
				{
					os << msg << endl;
					continue;
				}

				// Output a formatted message with the error
				os << named_sources_.at(li).first
				   << c
				   << (pline - lc)
				   << d
				   << msgis.rdbuf()
				   << endl;
			}
		}

		// Update exception and rethrow
		throw gl::shader_compilation_error(GLuint(shader), os.str());
	}
}
