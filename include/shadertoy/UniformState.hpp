#ifndef _SHADERTOY_UNIFORM_STATE_HPP_
#define _SHADERTOY_UNIFORM_STATE_HPP_

#include <boost/variant.hpp>

#include "shadertoy/pre.hpp"

namespace shadertoy
{

/**
 * @brief      A typed shader input uniform.
 *
 * @tparam UniformName Name of the uniform in the shader program
 * @tparam UniformType Type of the associated GLSL uniform
 * @tparam TUniform    Type of the uniform (GLfloat, GLint, oglplus::Vec3f, etc)
 * @tparam Count       Number of instances of the type. Must be strictly positive
 */
template<const char *UniformName, const char *UniformType, typename TUniform,
	size_t Count = 1, typename = typename std::enable_if<(Count >= 1)>::type>
struct shadertoy_EXPORT ShaderInput
{
	static constexpr const char *Name = UniformName;
	static constexpr const char *GLSLTypeName = UniformType;
	static constexpr size_t N = Count;
	using ValueType = TUniform;
	using ArrayType = std::array<ValueType, N>;

	ArrayType Values;

	ShaderInput()
		: Values()
	{
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
	typedef boost::variant<std::shared_ptr<Inputs>...> VariantPtr;

	/// Map of inputs where keys are uniform names, for fast lookup.
	std::map<std::string, VariantPtr> InputMap;

	/// Tuple of initialized inputs
	std::tuple<std::shared_ptr<Inputs>...> AllInputs;

	template<class Input>
	static void AppendDefinition(std::ostream &os)
	{
		os << "uniform " << Input::GLSLTypeName << " " << Input::Name;
		
		if (Input::N > 1)
		{
			os << "[" << Input::N << "]";
		}

		os << ";" << std::endl;
	}

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
		template<typename Input>
		struct Uniform
		{
			oglplus::UniformLoc Location;
			oglplus::Uniform<typename Input::ValueType> Var;

			Uniform(oglplus::Program &program)
				: Location(program, Input::Name, false),
				  Var(Location)
			{
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
		bool SetValue()
		{
			auto valptr = std::get<Index>(State.AllInputs);
			auto &uniform(std::get<Index>(Uniforms));

			if (uniform.Location.IsActive())
			{
				// Set uniform using state value
				uniform.Var.Set(static_cast<size_t>(valptr->Values.size()),
								static_cast<const typename Type::ValueType *>(valptr->Values.data()));

				return true;
			}

			return false;
		}

		/**
		 * @brief      Sets all the values of uniforms declared by the parent state
		 *
		 * @tparam Indices Index range of the uniforms in the parent state type declaration
		 */
		template<size_t... Indices>
		void SetValues(std::index_sequence<Indices...>)
		{
			bool _[] = {SetValue<Indices, Inputs>()...};
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
		BoundInputs(StateType &state, oglplus::Program &program, std::index_sequence<Indices...>)
			: State(state),
			  Uniforms(Uniform<Inputs>(program)...)
		{
		}

		/**
		 * @brief      Applies the value of all uniforms to the current program.
		 */
		void Apply() override
		{
			SetValues(std::make_index_sequence<sizeof...(Inputs)>());
		}
	};

	/**
	 * @brief      Get a reference to the storage (as a single value) for the given uniform input.
	 *
	 * @tparam Input Uniform typed identifier
	 * @return
	 */
	template<typename Input, typename = typename std::enable_if<(Input::N == 1)>::type>
	typename Input::ValueType &V()
	{
		return std::get<0>(boost::get<std::shared_ptr<Input>>(InputMap[Input::Name])->Values);
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
		return std::get<0>(boost::get<std::shared_ptr<Input>>(InputMap.find(Input::Name)->second)->Values);
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
		return boost::get<std::shared_ptr<Input>>(InputMap[Input::Name])->Values;
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
		return boost::get<std::shared_ptr<Input>>(InputMap.find(Input::Name)->second)->Values;
	}

	/**
	 * @brief      Binds uniform inputs to a specific programs, and returns the handle object.
	 *
	 * @param program Program to bind to.
	 * @return
	 */
	std::shared_ptr<BoundInputs> GetBoundInputs(oglplus::Program &program)
	{
		return std::make_shared<BoundInputs>(*this, program, Indices());
	}

	/**
	 * @brief      Get the GLSL code for defining the uniforms part of this
	 *             template instance.
	 *
	 * @return     GLSL code to include in the shader compilation stage.
	 */
	static std::string GetDefinitions()
	{
		std::stringstream ss;

		// Invoke append definition for each input
		int _[] = {(AppendDefinition<Inputs>(ss), 0)...};
		(void) _;

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
