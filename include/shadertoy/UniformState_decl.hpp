#ifndef IMPLEMENT_UNIFORM_STATE
#if !defined(DECLARE_UNIFORM) && !defined(DECLARE_UNIFORM_N)
/**
 * @brief          Helper macro to declare a uniform instance
 */
#define DECLARE_UNIFORM(type, name) extern const char name ## Name []; typedef ShaderInput<name ## Name, type> name
/**
 * @brief          Helper macro to declare a uniform instance as an array
 */
#define DECLARE_UNIFORM_N(type, name, n) extern const char name ## Name []; typedef ShaderInput<name ## Name, type, n> name
#endif
#else /* IMPLEMENT_UNIFORM_STATE */
#undef DECLARE_UNIFORM
#undef DECLARE_UNIFORM_N
#define DECLARE_UNIFORM(type, name) const char name ## Name [] = #name
#define DECLARE_UNIFORM_N(type, name, n) const char name ## Name [] = #name
#endif /* IMPLEMENT_UNIFORM_STATE */
