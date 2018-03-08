#ifndef _SHADERTOY_UNIFORM_STATE_HPP_
#define _SHADERTOY_UNIFORM_STATE_HPP_

#include "shadertoy/pre.hpp"

namespace shadertoy
{

/**
 * @brief Information about a GLSL type deduced from a C++ type.
 * First element is the type name, second element is the potential array suffix.
 */
typedef std::tuple<std::string, std::string> GLSLTypeInfo;

/**
 * @brief boost::variant visitor to return the GLSL typename of a boost::variant used
 * in a dynamic inputs block. This class may be derived to implement support
 * for more GLSL types.
 */
struct shadertoy_EXPORT DynamicShaderInputsGLSLTypeVisitor : public boost::static_visitor<GLSLTypeInfo>
{
	inline GLSLTypeInfo operator()(int) const { return std::make_tuple("int", ""); }
	inline GLSLTypeInfo operator()(glm::ivec2) const { return std::make_tuple("ivec2", ""); }
	inline GLSLTypeInfo operator()(glm::ivec3) const { return std::make_tuple("ivec3", ""); }
	inline GLSLTypeInfo operator()(glm::ivec4) const { return std::make_tuple("ivec4", ""); }

	inline GLSLTypeInfo operator()(float) const { return std::make_tuple("float", ""); }
	inline GLSLTypeInfo operator()(glm::vec2) const { return std::make_tuple("vec2", ""); }
	inline GLSLTypeInfo operator()(glm::vec3) const { return std::make_tuple("vec3", ""); }
	inline GLSLTypeInfo operator()(glm::vec4) const { return std::make_tuple("vec4", ""); }

	inline GLSLTypeInfo operator()(unsigned int) const { return std::make_tuple("unsigned int", ""); }
	inline GLSLTypeInfo operator()(glm::uvec2) const { return std::make_tuple("uvec2", ""); }
	inline GLSLTypeInfo operator()(glm::uvec3) const { return std::make_tuple("uvec3", ""); }
	inline GLSLTypeInfo operator()(glm::uvec4) const { return std::make_tuple("uvec4", ""); }

	template<class T, std::size_t N>
	GLSLTypeInfo operator()(std::array<T, N> &) const {
		return std::make_tuple(std::get<0>((*this)(T())),
			std::string("[") + std::string(N) + std::string("]") +
			std::get<1>((*this)(T())));
	}

	template<class T>
	GLSLTypeInfo operator()(std::shared_ptr<T> &) const { return (*this)(T()); }
};

/**
 * @brief boost::variant visitor that sets the value to be sent to the driver based on
 * the type of the object contained in the boost::variant.
 */
class shadertoy_EXPORT DynamicShaderInputUniformSetter : public boost::static_visitor<bool>
{
	/// OpenGL GLSL uniform location to set
	gl::uniform_location &location;

public:
	template<class T>
	inline bool operator()(const T& value) const
	{
		location.set_value(1, &value);
		return true;
	}

	template<class T, std::size_t N>
	inline bool operator()(const std::array<T, N> &value) const
	{
		location.set_value(N, value.data());
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
	DynamicShaderInputUniformSetter(gl::uniform_location &location)
		: location(location)
	{}
};

/**
 * @brief      A typed shader input uniform.
 *
 * @tparam UniformName Name of the uniform in the shader program
 * @tparam UniformType Type of the associated GLSL uniform
 * @tparam TUniform    Type of the uniform (GLfloat, GLint, glm::vec3, etc)
 * @tparam Count       Number of instances of the type. Must be strictly positive
 */
template<const char *UniformName, const char *UniformType, typename TUniform,
	size_t Count = 1, typename = typename std::enable_if<(Count >= 1)>::type>
struct shadertoy_EXPORT ShaderInput
{
	static constexpr const char *Name = UniformName;
	static constexpr const char *GLSLTypeName = UniformType;
	static constexpr const size_t N = Count;
	using ValueType = TUniform;
	using ArrayType = std::array<ValueType, N>;

private:
	ArrayType _values;

public:
	/**
	 * @brief Obtains the values of this input as an array.
	 */
	inline ArrayType &Values() { return _values; }

	/**
	 * @brief Initializes a new shader input.
	 */
	ShaderInput()
		: _values()
	{
	}

	/**
	 * @brief Appends the GLSL definition of this input to the given output stream.
	 *
	 * @param os Output stream to append to
	 */
	static void AppendDefinition(std::ostream &os)
	{
		os << "uniform " << GLSLTypeName << " " << Name;

		if (N > 1)
		{
			os << "[" << N << "]";
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
struct shadertoy_EXPORT DynamicShaderInputs
{
	static constexpr const char *Name = DynamicInputName;
	static constexpr const size_t N = 0;

	typedef boost::variant<std::shared_ptr<Types>...> VariantPtr;
	typedef DynamicShaderInputs<DynamicInputName, Types...> ValueType;

private:
	std::map<std::string, VariantPtr> InputMap;

public:
	/**
	 * @brief Adds a new input to this dynamic block.
	 *
	 * @param name  Name to use for the uniform variable. This must be a
	 *              valid GLSL identifier.
	 * @param args  Arguments to the constructor of the initial value.
	 */
	template<typename T, typename ...Args>
	void Add(const std::string &name, Args... args)
	{
		InputMap.insert(std::make_pair<std::string, VariantPtr>(std::string(name),
			std::make_shared<T>(std::forward<Args...>(args...))));
	}

	/**
	 * @brief Obtains a reference to the value of a dynamic uniform.
	 *
	 * @param  name Name of the uniform to obtain.
	 * @return      Reference to the value of this uniform.
	 */
	template<typename T>
	T &Get(const std::string &name)
	{
		return *boost::get<std::shared_ptr<T>>(InputMap[name]);
	}

	/**
	 * @brief Removes a dynamic uniform.
	 *
	 * @param name Name of the uniform to remove from this block.
	 */
	void Remove(const std::string &name)
	{
		InputMap.erase(name);
	}

	/**
	 * @brief Appends the GLSL definition of this input to the given output stream.
	 *
	 * @param os Output stream to append to
	 */
	void AppendDefinition(std::ostream &os)
	{
		os << "/* " << Name << " uniforms */" << std::endl;

		for (auto &pair : InputMap)
		{
			auto type_info(boost::apply_visitor(GLSLTypeVisitor(), pair.second));
			os << "uniform " << std::get<0>(type_info) << " "
				<< pair.first << std::get<1>(type_info) << ";" << std::endl;
		}
	}

	/**
	 * @brief Binds the inputs of this dynamic uniform block to the given program.
	 *
	 * @param program Program to bind uniforms to.
	 */
	std::map<std::string, gl::uniform_location> BindInputs(gl::program &program)
	{
		std::map<std::string, gl::uniform_location> result;

		for (auto &pair : InputMap)
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
		return boost::apply_visitor(DynamicShaderInputUniformSetter(location), InputMap[name]);
	}
};

/**
 * @brief      A base class for representing the state of inputs bound to
 *             particular shader program.
 */
class shadertoy_EXPORT BoundInputsBase
{
protected:
	virtual ~BoundInputsBase() = default;

public:
	/**
	 * @brief      Sets the current value of inputs bound to this program
	 *             instance in the associated shader program.
	 */
	virtual void Apply() = 0;
};

/**
 * @brief      A collection of named inputs, to be bound to actual programs.
 *
 * @tparam Inputs ShaderInput definitions
 */
template<class ...Inputs>
class shadertoy_EXPORT ShaderInputs
{
public:
	/// Templated shader input type itself
	typedef ShaderInputs<Inputs...> StateType;

	/// Index sequence matching Inputs definition
	using Indices = std::make_index_sequence<sizeof...(Inputs)>;

private:
	/// boost::variant pointer to the different supported input types
	typedef boost::variant<std::shared_ptr<Inputs>...> VariantPtr;

	/// Map of inputs where keys are uniform names, for fast lookup.
	std::map<std::string, VariantPtr> InputMap;

	/// Tuple of initialized inputs
	std::tuple<std::shared_ptr<Inputs>...> AllInputs;

public:
	/**
	 * @brief      Represents the binding of inputs to a specific program.
	 */
	class BoundInputs : public BoundInputsBase
	{
	public:
		/// State these bound inputs are derived from
		StateType &State;

	private:
		template<typename Input, typename Enable = void>
		struct Uniform {};

		/**
		 * @brief Bound uniform object for scalar inputs.
		 */
		template<typename Input>
		struct Uniform<Input, typename std::enable_if<(Input::N > 0)>::type>
		{
			gl::uniform_location Location;

			Uniform(std::shared_ptr<Input> &, gl::program &program)
				: Location(program.get_uniform_location(Input::Name))
			{
			}

			/**
			 * @brief Applies the values of the given input to the associated location.
			 *
			 * @param  valptr Pointer to the input containing the value to set.
			 * @return        true if the uniform location was set, false
			 *                otherwise.
			 */
			bool set_value(std::shared_ptr<Input> &valptr)
			{
				return Location.set_value(
					valptr->Values().size(),
					static_cast<const typename Input::ValueType *>(valptr->Values().data()));
			}
		};

		/**
		 * @brief Bound uniform object for dynamic inputs.
		 */
		template<typename Input>
		struct Uniform<Input, typename std::enable_if<(Input::N < 1)>::type>
		{
			std::map<std::string, gl::uniform_location> Locations;

			Uniform(std::shared_ptr<Input> &input, gl::program &program)
				: Locations(input->BindInputs(program))
			{
			}

			/**
			 * @brief Applies all the values of the given dynamic input to the
			 * associated locations.
			 *
			 * @param  valptr Pointer to the input containing the values to set.
			 * @return        true if the uniform locations were set, false
			 *                otherwise
			 */
			bool set_value(std::shared_ptr<Input> &valptr)
			{
				bool result = true;

				for (auto &pair : Locations)
					result = result && valptr->set_value(pair.first, pair.second);

				return result;
			}
		};

		/// Uniform objects tuple
		std::tuple<Uniform<Inputs>...> Uniforms;

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
			auto valptr = std::get<Index>(State.AllInputs);
			auto &uniform(std::get<Index>(Uniforms));

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
		BoundInputs(StateType &state, gl::program &program, std::index_sequence<Indices...>)
			: State(state),
			  Uniforms(Uniform<Inputs>(std::get<Indices>(state.AllInputs), program)...)
		{
		}

		/**
		 * @brief      Applies the value of all uniforms to the current program.
		 */
		void Apply() override
		{
			set_values(std::make_index_sequence<sizeof...(Inputs)>());
		}
	};

	private:
	/**
	 * @brief Internal implementation of AppendDefinition using index_sequence.
	 */
	template<size_t... Indices>
	void AppendDefinitions(std::ostream &os, std::index_sequence<Indices...>)
	{
		int _[] = {(std::get<Indices>(AllInputs)->AppendDefinition(os), 0)...};
		(void) _;
	}

	public:
	/**
	 * @brief Get a reference to the dynamic input object of type Input.
	 *
	 * @tparam Input Uniform typed identifier
	 * @return Reference to the dynamic input object of type Input.
	 */
	template<typename Input, typename = typename std::enable_if<(Input::N < 1)>::type>
	Input &V()
	{
		return *boost::get<std::shared_ptr<Input>>(InputMap[Input::Name]);
	}

	/**
	 * @brief      Get a reference to the storage (as a single value) for the given uniform input.
	 *
	 * @tparam Input Uniform typed identifier
	 * @return
	 */
	template<typename Input, typename = typename std::enable_if<(Input::N == 1)>::type>
	typename Input::ValueType &V()
	{
		return std::get<0>(boost::get<std::shared_ptr<Input>>(InputMap[Input::Name])->Values());
	}

	/**
	 * @brief      Get a reference to the storage (as a single value) for the given uniform input.
	 *
	 * @tparam Input Uniform typed identifier
	 * @return
	 */
	template<typename Input, typename = typename std::enable_if<(Input::N == 1)>::type>
	const typename Input::ValueType &V() const
	{
		return std::get<0>(boost::get<std::shared_ptr<Input>>(InputMap.find(Input::Name)->second)->Values());
	}

	/**
	 * @brief      Get a reference to the storage (as an array) for the given uniform input.
	 *
	 * @tparam Input Uniform typed identifier
	 * @return
	 */
	template<typename Input, typename = typename std::enable_if<(Input::N > 1)>::type>
	typename Input::ArrayType &V()
	{
		return boost::get<std::shared_ptr<Input>>(InputMap[Input::Name])->Values();
	}

	/**
	 * @brief      Get a reference to the storage (as an array) for the given uniform input.
	 *
	 * @tparam Input Uniform typed identifier
	 * @return
	 */
	template<typename Input, typename = typename std::enable_if<(Input::N > 1)>::type>
	const typename Input::ArrayType &V() const
	{
		return boost::get<std::shared_ptr<Input>>(InputMap.find(Input::Name)->second)->Values();
	}

	/**
	 * @brief      Binds uniform inputs to a specific programs, and returns the handle object.
	 *
	 * @param program Program to bind to.
	 * @return
	 */
	std::shared_ptr<BoundInputs> GetBoundInputs(gl::program &program)
	{
		return std::make_shared<BoundInputs>(*this, program, Indices());
	}

	/**
	 * @brief      Get the GLSL code for defining the uniforms part of this
	 *             template instance.
	 *
	 * @return     GLSL code to include in the shader compilation stage.
	 */
	std::string GetDefinitions()
	{
		std::stringstream ss;

		// Invoke append definition for each input
		AppendDefinitions(ss, Indices());

		return ss.str();
	}

	/**
	 * @brief      Initialize a new instance of the ShaderInputs class.
	 */
	ShaderInputs()
		: InputMap{(std::make_pair<std::string, VariantPtr>(
		Inputs::Name,
		std::make_shared<Inputs>()))...},
		  AllInputs((boost::get<std::shared_ptr<Inputs>>(InputMap[Inputs::Name]))...)
	{
	}
};
}

// Definitions for shader inputs
#include "shadertoy/UniformState_impl.hpp"

namespace shadertoy
{

/// Default program state typedef
typedef ShaderInputs<
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
> ShaderInputsType;

}

#endif /* _SHADERTOY_UNIFORM_STATE_HPP_ */
