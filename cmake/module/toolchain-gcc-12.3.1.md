# Readme of compiler

## Supported C features

The supported features of C compiler can be evaluated using `cmake`.

Just add following lines to the correspoding `CMakeLists.txt`.

```cmake
foreach(i ${CMAKE_C_COMPILE_FEATURES})
  message("${i}")
endforeach()
```

```shell
c_std_90
c_function_prototypes
c_std_99
c_restrict
c_variadic_macros
c_std_11
c_static_assert
c_std_17
c_std_23
```

## Supported C++ features

The supported features the of C++ compiler can be evaluated using `cmake`.

Add the following lines to the appropriate `CMakeLists.txt` fiile.

```cmake
foreach(i ${CMAKE_CXX_COMPILE_FEATURES})
  message("${i}")
endforeach()
```

```shell
cxx_std_98
cxx_template_template_parameters
cxx_std_11
cxx_alias_templates
cxx_alignas
cxx_alignof
cxx_attributes
cxx_auto_type
cxx_constexpr
cxx_decltype
cxx_decltype_incomplete_return_types
cxx_default_function_template_args
cxx_defaulted_functions
cxx_defaulted_move_initializers
cxx_delegating_constructors
cxx_deleted_functions
cxx_enum_forward_declarations
cxx_explicit_conversions
cxx_extended_friend_declarations
cxx_extern_templates
cxx_final
cxx_func_identifier
cxx_generalized_initializers
cxx_inheriting_constructors
cxx_inline_namespaces
cxx_lambdas
cxx_local_type_template_args
cxx_long_long_type
cxx_noexcept
cxx_nonstatic_member_init
cxx_nullptr
cxx_override
cxx_range_for
cxx_raw_string_literals
cxx_reference_qualified_functions
cxx_right_angle_brackets
cxx_rvalue_references
cxx_sizeof_member
cxx_static_assert
cxx_strong_enums
cxx_thread_local
cxx_trailing_return_types
cxx_unicode_literals
cxx_uniform_initialization
cxx_unrestricted_unions
cxx_user_literals
cxx_variadic_macros
cxx_variadic_templates
cxx_std_14
cxx_aggregate_default_initializers
cxx_attribute_deprecated
cxx_binary_literals
cxx_contextual_conversions
cxx_decltype_auto
cxx_digit_separators
cxx_generic_lambdas
cxx_lambda_init_captures
cxx_relaxed_constexpr
cxx_return_type_deduction
cxx_variable_templates
cxx_std_17
cxx_std_20
cxx_std_23
```
