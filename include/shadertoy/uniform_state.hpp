#ifndef _SHADERTOY_UNIFORM_STATE_HPP_
#define _SHADERTOY_UNIFORM_STATE_HPP_

#include "shadertoy/pre.hpp"

#include <map>
#include <sstream>
#include <string>
#include <tuple>

#include <boost/variant.hpp>

namespace shadertoy
{

/**
 * @brief Information about a GLSL type deduced from a C++ type.
 * First element is the type name, second element is the potential array suffix.
 */
typedef std::tuple<std::string, std::string> glsl_type_info;

/**
 * @brief boost::variant visitor to return the GLSL typename of a boost::variant used
 * in a dynamic inputs block. This class may be derived to implement support
 * for more GLSL types.
 */
struct shadertoy_EXPORT dynamic_shader_inputs_glsl_type_visitor : public boost::static_visitor<glsl_type_info>
{
	inline glsl_type_info operator()(int) const { return std::make_tuple("int", ""); }
	inline glsl_type_info operator()(glm::ivec2) const { return std::make_tuple("ivec2", ""); }
	inline glsl_type_info operator()(glm::ivec3) const { return std::make_tuple("ivec3", ""); }
	inline glsl_type_info operator()(glm::ivec4) const { return std::make_tuple("ivec4", ""); }

	inline glsl_type_info operator()(float) const { return std::make_tuple("float", ""); }
	inline glsl_type_info operator()(glm::vec2) const { return std::make_tuple("vec2", ""); }
	inline glsl_type_info operator()(glm::vec3) const { return std::make_tuple("vec3", ""); }
	inline glsl_type_info operator()(glm::vec4) const { return std::make_tuple("vec4", ""); }

	inline glsl_type_info operator()(unsigned int) const { return std::make_tuple("unsigned int", ""); }
	inline glsl_type_info operator()(glm::uvec2) const { return std::make_tuple("uvec2", ""); }
	inline glsl_type_info operator()(glm::uvec3) const { return std::make_tuple("uvec3", ""); }
	inline glsl_type_info operator()(glm::uvec4) const { return std::make_tuple("uvec4", ""); }

	template<class T, std::size_t N>
	glsl_type_info operator()(std::array<T, N> &) const {
		return std::make_tuple(std::get<0>((*this)(T())),
			std::string("[") + std::string(N) + std::string("]") +
			std::get<1>((*this)(T())));
	}
};

/**
 * @brief boost::variant visitor that sets the value to be sent to the driver based on
 * the type of the object contained in the boost::variant.
 */
class shadertoy_EXPORT dynamic_shader_input_uniform_setter : public boost::static_visitor<bool>
{
	/// OpenGL GLSL uniform location to set
	gl::uniform_location &location_;

public:
	template<class T>
	inline bool operator()(const T& value) const
	{
		location_.set_value(1, &value);
		return true;
	}

	template<class T, std::size_t N>
	inline bool operator()(const std::array<T, N> &value) const
	{
		location_.set_value(N, value.data());
		return true;
	}

	template<class T>
	inline bool operator()(const std::shared_ptr<T> &ptr) const
	{
		return (*this)(*ptr);
	}

	/**
	 * @brief Creates a new setter visitor.
	 *
	 * @param location GLSL program location to set
	 */
	dynamic_shader_input_uniform_setter(gl::uniform_location &location)
		: location_(location)
	{}
};

/**
 * @brief      A typed shader input uniform.
 *
 * @tparam UniformName Name of the uniform in the shader program
 * @tparam UniformType Type of the associated GLSL uniform
 * @tparam TUniform    Type of the uniform (GLfloat, GLint, glm::vec3, etc)
 * @tparam Size       Number of instances of the type. Must be strictly positive
 */
template<const char *UniformName, const char *UniformType, typename TUniform,
	size_t Size = 1, typename = typename std::enable_if<(Size >= 1)>::type>
struct shadertoy_EXPORT shader_input
{
	static constexpr const char *name = UniformName;
	static constexpr const char *glsl_type_name = UniformType;
	static constexpr const size_t size = Size;
	using value_type = TUniform;
	using array_type = std::array<value_type, size>;

private:
	array_type values_;

public:
	/**
	 * @brief Obtains the values of this input as an array.
	 */
	inline array_type &values() { return values_; }

	/**
	 * @brief Initializes a new shader input.
	 */
	shader_input()
		: values_()
	{
	}

	/**
	 * @brief Appends the GLSL definition of this input to the given output stream.
	 *
	 * @param os Output stream to append to
	 */
	static void append_definition(std::ostream &os)
	{
		os << "uniform " << glsl_type_name << " " << name;

		if (size > 1)
		{
			os << "[" << size << "]";
		}

		os << ";" << std::endl;
	}
};

/**
 * @brief Represents a block of dynamic inputs. Dynamic inputs are defined by the
 * calling program at run-time, before the GLSL programs are compiled.
 *
 * @tparam DynamicInputName Name of the uniform block. This is used to
 *                          distinguish this input from other static inputs.
 * @tparam GLSLTypeVisitor  An implementation of a boost::variant visitor that
 *                          follows the conventions set by
 *                          DynamicShaderInputsGLSLTypeVisitor.
 * @tparam Types            List of typenames that define the types supported by
 *                          this dynamic uniform block.
 */
template<const char *DynamicInputName, class GLSLTypeVisitor, class ...Types>
struct shadertoy_EXPORT dynamic_shader_inputs
{
	static constexpr const char *name = DynamicInputName;
	static constexpr const size_t size = 0;

	typedef boost::variant<Types...> variant_input;
	typedef dynamic_shader_inputs<DynamicInputName, Types...> value_type;

	using glsl_type_visitor = GLSLTypeVisitor;

private:
	std::map<std::string, variant_input> input_map;

public:
	/**
	 * @brief Adds a new input to this dynamic block.
	 *
	 * @param name  Name to use for the uniform variable. This must be a
	 *              valid GLSL identifier.
	 * @param args  Arguments to the constructor of the initial value.
	 */
	template<typename T, typename ...Args>
	void insert(const std::string &name, Args... args)
	{
		input_map.insert(std::make_pair<std::string, variant_input>(std::string(name),
			T(std::forward<Args...>(args...))));
	}

	/**
	 * @brief Obtains a reference to the value of a dynamic uniform.
	 *
	 * @param  name Name of the uniform to obtain.
	 * @return      Reference to the value of this uniform.
	 */
	template<typename T>
	T &get(const std::string &name)
	{
		return boost::get<T>(input_map[name]);
	}

	/**
	 * @brief Removes a dynamic uniform.
	 *
	 * @param name Name of the uniform to remove from this block.
	 */
	void erase(const std::string &name)
	{
		input_map.erase(name);
	}

	/**
	 * @brief Appends the GLSL definition of this input to the given output stream.
	 *
	 * @param os Output stream to append to
	 */
	void append_definition(std::ostream &os)
	{
		os << "/* " << name << " uniforms */" << std::endl;

		for (auto &pair : input_map)
		{
			auto type_info(boost::apply_visitor(glsl_type_visitor(), pair.second));
			os << "uniform " << std::get<0>(type_info) << " "
				<< pair.first << std::get<1>(type_info) << ";" << std::endl;
		}
	}

	/**
	 * @brief Binds the inputs of this dynamic uniform block to the given program.
	 *
	 * @param program Program to bind uniforms to.
	 */
	std::map<std::string, gl::uniform_location> bind_inputs(gl::program &program)
	{
		std::map<std::string, gl::uniform_location> result;

		for (auto &pair : input_map)
		{
			result.insert(std::make_pair(pair.first, program.get_uniform_location(pair.first.c_str())));
		}

		return result;
	}

	/**
	 * @brief Applies the values of the input with the given name, to the given
	 * location.
	 *
	 * @param  name     Name of the uniform input to apply
	 * @param  location GLSL uniform location to set
	 * @return          true if the location was set, false otherwise
	 */
	bool set_value(const std::string &name, gl::uniform_location &location)
	{
		return boost::apply_visitor(dynamic_shader_input_uniform_setter(location), input_map[name]);
	}
};

/**
 * @brief      A base class for representing the state of inputs bound to
 *             particular shader program.
 */
class shadertoy_EXPORT bound_inputs_base
{
protected:
	virtual ~bound_inputs_base() = default;

public:
	/**
	 * @brief      Sets the current value of inputs bound to this program
	 *             instance in the associated shader program.
	 */
	virtual void apply() = 0;
};

/**
 * @brief      A collection of named inputs, to be bound to actual programs.
 *
 * @tparam Inputs ShaderInput definitions
 */
template<class ...Inputs>
class shadertoy_EXPORT shader_inputs
{
public:
	/// Templated shader input type itself
	typedef shader_inputs<Inputs...> state_type;

	/// Index sequence matching Inputs definition
	using indices = std::make_index_sequence<sizeof...(Inputs)>;

private:
	/// Tuple of initialized inputs
	std::tuple<Inputs...> all_inputs_;

	template<size_t index, typename Target, typename Input, typename... Ts>
	inline constexpr typename std::enable_if<std::is_same<Input, Target>::value, Target&>::type get_input() {
		return std::get<index>(all_inputs_);
	}

	template<size_t index, typename Target, typename Input, typename... Ts>
	inline constexpr typename std::enable_if<!(std::is_same<Input, Target>::value) && index <= sizeof...(Inputs), Target&>::type get_input() {
		return get_input<index + 1, Target, Ts...>();
	}

	template<typename InputType>
	constexpr InputType& input() { return get_input<0, InputType, Inputs...>(); }

public:
	/**
	 * @brief      Represents the binding of inputs to a specific program.
	 */
	class bound_inputs : public bound_inputs_base
	{
	public:
		/// State these bound inputs are derived from
		state_type &state;

	private:
		template<typename Input, typename Enable = void>
		struct uniform {};

		/**
		 * @brief Bound uniform object for scalar inputs.
		 */
		template<typename Input>
		struct uniform<Input, typename std::enable_if<(Input::size > 0)>::type>
		{
			gl::uniform_location location;

			uniform(Input &, gl::program &program)
				: location(program.get_uniform_location(Input::name))
			{
			}

			/**
			 * @brief Applies the values of the given input to the associated location.
			 *
			 * @param  val Reference to the input containing the value to set.
			 * @return true if the uniform location was set, false otherwise.
			 */
			bool set_value(Input &val)
			{
				return location.set_value(
					val.values().size(),
					static_cast<const typename Input::value_type *>(val.values().data()));
			}
		};

		/**
		 * @brief Bound uniform object for dynamic inputs.
		 */
		template<typename Input>
		struct uniform<Input, typename std::enable_if<(Input::size < 1)>::type>
		{
			std::map<std::string, gl::uniform_location> locations;

			uniform(Input &input, gl::program &program)
				: locations(input.bind_inputs(program))
			{
			}

			/**
			 * @brief Applies all the values of the given dynamic input to the
			 * associated locations.
			 *
			 * @param  val Reference to the input containing the values to set.
			 * @return true if the uniform locations were set, false otherwise
			 */
			bool set_value(Input &val)
			{
				bool result = true;

				for (auto &pair : locations)
				{
					bool success = val.set_value(pair.first, pair.second);
					result = result && success;
				}

				return result;
			}
		};

		/// Uniform objects tuple
		std::tuple<uniform<Inputs>...> uniforms_;

		/**
		 * @brief      Sets the value of a given uniform in this bound input object
		 *
		 * @tparam Index Index of the uniform in the parent state type declaration
		 * @tparam Type  Type of the shader input object in the parent state type declaration
		 * @return       true if the value has been set, false if the associated uniform was inactive
		 */
		template<size_t Index, class Type>
		bool set_value()
		{
			auto valptr = std::get<Index>(state.all_inputs_);
			auto &uniform(std::get<Index>(uniforms_));

			// Set uniform using state value
			return uniform.set_value(valptr);
		}

		/**
		 * @brief      Sets all the values of uniforms declared by the parent state
		 *
		 * @tparam Indices Index range of the uniforms in the parent state type declaration
		 */
		template<size_t... Indices>
		void set_values(std::index_sequence<Indices...>)
		{
			bool _[] = {set_value<Indices, Inputs>()...};
			(void) _;
		}

	public:
		/**
		 * @brief      Initializes a new instance of a bound uniform object.
		 *
		 * @tparam Indices Index range of the uniforms in the parent state type declaration
		 * @param state Parent state of this uniform object
		 * @param program Program to bind to
		 */
		template<size_t... Indices>
		bound_inputs(state_type &state, gl::program &program, std::index_sequence<Indices...>)
			: state(state),
			  uniforms_(uniform<Inputs>(std::get<Indices>(state.all_inputs_), program)...)
		{
		}

		/**
		 * @brief      Applies the value of all uniforms to the current program.
		 */
		void apply() override
		{
			set_values(std::make_index_sequence<sizeof...(Inputs)>());
		}
	};

	private:
	/**
	 * @brief Internal implementation of AppendDefinition using index_sequence.
	 */
	template<size_t... Indices>
	void append_definitions(std::ostream &os, std::index_sequence<Indices...>)
	{
		int _[] = {(std::get<Indices>(all_inputs_).append_definition(os), 0)...};
		(void) _;
	}

	public:
	/**
	 * @brief Get a reference to the dynamic input object of type Input.
	 *
	 * @tparam Input Uniform typed identifier
	 * @return Reference to the dynamic input object of type Input.
	 */
	template<typename Input, typename = typename std::enable_if<(Input::size < 1)>::type>
	Input &get()
	{
		return input<Input>();
	}

	/**
	 * @brief      Get a reference to the storage (as a single value) for the given uniform input.
	 *
	 * @tparam Input Uniform typed identifier
	 * @return
	 */
	template<typename Input, typename = typename std::enable_if<(Input::size == 1)>::type>
	typename Input::value_type &get()
	{
		return std::get<0>(input<Input>().values());
	}

	/**
	 * @brief      Get a reference to the storage (as a single value) for the given uniform input.
	 *
	 * @tparam Input Uniform typed identifier
	 * @return
	 */
	template<typename Input, typename = typename std::enable_if<(Input::size == 1)>::type>
	const typename Input::value_type &get() const
	{
		return std::get<0>(input<Input>().values());
	}

	/**
	 * @brief      Get a reference to the storage (as an array) for the given uniform input.
	 *
	 * @tparam Input Uniform typed identifier
	 * @return
	 */
	template<typename Input, typename = typename std::enable_if<(Input::size > 1)>::type>
	typename Input::array_type &get()
	{
		return input<Input>().values();
	}

	/**
	 * @brief      Get a reference to the storage (as an array) for the given uniform input.
	 *
	 * @tparam Input Uniform typed identifier
	 * @return
	 */
	template<typename Input, typename = typename std::enable_if<(Input::size > 1)>::type>
	const typename Input::array_type &get() const
	{
		return input<Input>().values();
	}

	/**
	 * @brief      Binds uniform inputs to a specific programs, and returns the handle object.
	 *
	 * @param program Program to bind to.
	 * @return
	 */
	std::shared_ptr<bound_inputs> bind_inputs(gl::program &program)
	{
		return std::make_shared<bound_inputs>(*this, program, indices());
	}

	/**
	 * @brief      Get the GLSL code for defining the uniforms part of this
	 *             template instance.
	 *
	 * @return     GLSL code to include in the shader compilation stage.
	 */
	std::string definitions_string()
	{
		std::stringstream ss;

		// Invoke append definition for each input
		append_definitions(ss, indices());

		return ss.str();
	}

	/**
	 * @brief      Initialize a new instance of the ShaderInputs class.
	 */
	shader_inputs()
		: all_inputs_()
	{
	}
};
}

// Definitions for shader inputs
#include "shadertoy/uniform_state_impl.hpp"

namespace shadertoy
{

/// Default program state typedef
typedef shader_inputs<
	iResolution,
	iTime,
	iTimeDelta,
	iFrame,
	iFrameRate,
	iChannelTime,
	iChannelResolution,
	iMouse,
	iChannel0,
	iChannel1,
	iChannel2,
	iChannel3,
	iDate,
	iSampleRate
> shader_inputs_t;

}

#endif /* _SHADERTOY_UNIFORM_STATE_HPP_ */
