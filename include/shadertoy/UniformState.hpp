#ifndef _SHADERTOY_UNIFORM_STATE_HPP_
#define _SHADERTOY_UNIFORM_STATE_HPP_

#include <boost/variant.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/array.hpp>

#include "shadertoy/pre.hpp"
#include "shadertoy/Misc.hpp"

namespace shadertoy
{

/**
 * @brief      A typed shader input uniform.
 *
 * @tparam UniformName Name of the uniform in the shader program
 * @tparam TUniform    Type of the uniform (GLfloat, GLint, oglplus::Vec3f, etc)
 * @tparam Count       Number of instances of the type. Must be strictly positive
 */
template<const char *UniformName, typename TUniform, size_t Count = 1,
	typename = typename std::enable_if<(Count >= 1)>::type>
struct ShaderInput
{
	static constexpr const char *Name = UniformName;
	static constexpr size_t N = Count;
	using ValueType = TUniform;
	using ArrayType = std::array<ValueType, N>;

	ArrayType Values;

	ShaderInput()
		: Values()
	{
	}

private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		using boost::serialization::make_nvp;

		if (N == 1)
		{
			ar & make_nvp("value", std::get<0>(Values));
		}
		else
		{
			ar & make_nvp("value", Values);
		}
	}
};

/**
 * @brief      A base class for representing the state of inputs bound to
 *             particular shader program.
 */
class BoundInputsBase
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
class ShaderInputs
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

	friend class boost::serialization::access;

	template<class Archive, size_t Index, class Input>
	void serialize_member(Archive &ar, const unsigned version)
	{
		using boost::serialization::make_nvp;
		ar & make_nvp(Input::Name, *std::get<Index>(AllInputs));
	}

	template<class Archive, size_t... Indices>
	void serialize_members(Archive &ar, const unsigned version, std::index_sequence<Indices...>)
	{
		int _[] = {(serialize_member<Archive, Indices, Inputs>(ar, version), 0)...};
		(void) _;
	}

	template<class Archive>
	void serialize(Archive &ar, const unsigned version)
	{
		serialize_members(ar, version, Indices());
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
		/// Lazy uniform objects tuple
		std::tuple<oglplus::Lazy<oglplus::Uniform<typename Inputs::ValueType>>...> Uniforms;

		/**
		 * @brief      Sets the value of a given lazy uniform in this bound input object
		 *
		 * @tparam Index Index of the uniform in the parent state type declaration
		 * @tparam Type  Type of the shader input object in the parent state type declaration
		 * @return       true if the value has been set, false if the associated uniform was inactive
		 */
		template<size_t Index, class Type>
		bool SetValue()
		{
			auto valptr = std::get<Index>(State.AllInputs);
			try
			{
				// Set uniform using state value
				std::get<Index>(Uniforms).Set(static_cast<size_t>(valptr->Values.size()),
											  static_cast<const typename Type::ValueType *>(valptr->Values.data()));
				return true;
			} catch (oglplus::ProgVarError &ex)
			{
				// Inactive uniform error, other errors are propagated
				/*std::cerr << valptr->Name << ": " << ex.what() << std::endl;*/
				return false;
			}
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
		BoundInputs(StateType &state, oglplus::ProgramName program, std::index_sequence<Indices...>)
			: State(state),
			  Uniforms(oglplus::Lazy<oglplus::Uniform<typename Inputs::ValueType>>(program, Inputs::Name)...)
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
	std::shared_ptr<BoundInputs> GetBoundInputs(oglplus::ProgramName program)
	{
		return std::make_shared<BoundInputs>(*this, program, Indices());
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
	iGlobalTime,
	iTimeDelta,
	iFrame,
	iFrameRate,
	iMouse,
	iChannel0,
	iChannel1,
	iChannel2,
	iChannel3,
	iDate,
	iChannelResolution
> ShaderInputsType;

}

#endif /* _SHADERTOY_UNIFORM_STATE_HPP_ */
