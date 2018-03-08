#ifndef IMPLEMENT_UNIFORM_STATE
#if !defined(DECLARE_UNIFORM) && !defined(DECLARE_UNIFORM_N) && !defined(DECLARE_DYNAMIC_UNIFORM)
/**
 * @brief          Helper macro to declare a uniform instance
 */
#define DECLARE_UNIFORM(type, name, glsl)                                      \
  shadertoy_EXPORT extern const char name##Name[];                             \
  shadertoy_EXPORT extern const char name##GLSLTypeName[];                     \
  typedef ShaderInput<name##Name, name##GLSLTypeName, type> name
/**
 * @brief          Helper macro to declare a uniform instance as an array
 */
#define DECLARE_UNIFORM_N(type, name, n, glsl)                                 \
  shadertoy_EXPORT extern const char name##Name[];                             \
  shadertoy_EXPORT extern const char name##GLSLTypeName[];                     \
  typedef ShaderInput<name##Name, name##GLSLTypeName, type, n> name

/**
 * @brief          Helper macro to declare a dynamic uniform block
 */
#define DECLARE_DYNAMIC_UNIFORM(name, ...)                                     \
  shadertoy_EXPORT extern const char name##Name[];                             \
  typedef shadertoy::DynamicShaderInputs<                                      \
      name##Name, shadertoy::DynamicShaderInputsGLSLTypeVisitor, __VA_ARGS__>  \
      name

#endif /* !defined(DECLARE_UNIFORM) && !defined(DECLARE_UNIFORM_N) && !defined(DECLARE_DYNAMIC_UNIFORM) */
#else /* IMPLEMENT_UNIFORM_STATE */
#undef DECLARE_UNIFORM
#undef DECLARE_UNIFORM_N
#undef DECLARE_DYNAMIC_UNIFORM

#define DECLARE_UNIFORM(type, name, glsl)                                      \
  const char name##Name[] = #name;                                             \
  const char name##GLSLTypeName[] = glsl
#define DECLARE_UNIFORM_N(type, name, n, glsl)                                 \
  const char name##Name[] = #name;                                             \
  const char name##GLSLTypeName[] = glsl

#define DECLARE_DYNAMIC_UNIFORM(name, ...) const char name##Name[] = #name

#endif /* IMPLEMENT_UNIFORM_STATE */
