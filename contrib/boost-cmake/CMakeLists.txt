add_definitions(-Wno-unused-variable -Wno-deprecated-declarations)


set(CONFIG_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/config)
set(ANY_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/any)
set(CORE_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/core)
set(TYPE_TRAITS_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/type_traits)
set(ITERATOR_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/iterator)
set(TYPE_INDEX_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/type_index)
set(DETAIL_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/detail)
set(MPL_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/mpl)
set(CONTAINER_HASH_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/container_hash)
set(PREPROCESSOR_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/preprocessor)
set(STATIC_ASSERT_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/static_assert)
set(THROW_EXCEPTION_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/throw_exception)
set(SMART_PTR_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/smart_ptr)
set(ASSERT_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/assert)
set(DESCRIBE_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/describe)
set(BIND_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/bind)
set(MP11_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/mp11)
set(FUNCTION_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/function)
set(INTEGER_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/integer)
set(LEXICAL_CAST_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/lexical_cast)
set(RANGE_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/range)
set(CONCEPT_CHECK_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/concept_check)
set(UTILITY_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/utility)
set(NUMERIC_CONVERSION_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/numeric_conversion)
set(ARRAY_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/array)
set(CONTAINER_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/container)
set(MOVE_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/move)
set(TOKENIZER_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/tokenizer)
set(ALGORITHM_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/algorithm)
set(BIMAP_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/bimap)
set(ASSIGN_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/assign)
set(SERIALIZATION_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/serialization)
set(TUPLE_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/tuple)
set(MULTI_INDEX_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/multi_index)
set(FOREACH_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/foreach)
set(MULTIPRECISION_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/multiprecision)
set(PROPERTY_TREE_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/property_tree)
set(OPTIONAL_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/optional)
set(FORMAT_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/format)


set(BOOST_HEADERONLY_INCLUDE_DIR
    ${CONFIG_LIBRARY_DIR}/include
    ${ANY_LIBRARY_DIR}/include
    ${CORE_LIBRARY_DIR}/include
    ${TYPE_TRAITS_LIBRARY_DIR}/include
    ${ITERATOR_LIBRARY_DIR}/include
    ${TYPE_INDEX_LIBRARY_DIR}/include
    ${DETAIL_LIBRARY_DIR}/include
    ${MPL_LIBRARY_DIR}/include
    ${CONTAINER_HASH_LIBRARY_DIR}/include
    ${PREPROCESSOR_LIBRARY_DIR}/include
    ${STATIC_ASSERT_LIBRARY_DIR}/include
    ${THROW_EXCEPTION_LIBRARY_DIR}/include
    ${SMART_PTR_LIBRARY_DIR}/include
    ${ASSERT_LIBRARY_DIR}/include
    ${DESCRIBE_LIBRARY_DIR}/include
    ${BIND_LIBRARY_DIR}/include
    ${MP11_LIBRARY_DIR}/include
    ${FUNCTION_LIBRARY_DIR}/include
    ${INTEGER_LIBRARY_DIR}/include
    ${LEXICAL_CAST_LIBRARY_DIR}/include
    ${RANGE_LIBRARY_DIR}/include
    ${CONCEPT_CHECK_LIBRARY_DIR}/include
    ${UTILITY_LIBRARY_DIR}/include
    ${NUMERIC_CONVERSION_LIBRARY_DIR}/include
    ${ARRAY_LIBRARY_DIR}/include
    ${CONTAINER_LIBRARY_DIR}/include
    ${MOVE_LIBRARY_DIR}/include
    ${TOKENIZER_LIBRARY_DIR}/include
    ${ALGORITHM_LIBRARY_DIR}/include
    ${BIMAP_LIBRARY_DIR}/include
    ${ASSIGN_LIBRARY_DIR}/include
    ${SERIALIZATION_LIBRARY_DIR}/include
    ${TUPLE_LIBRARY_DIR}/include
    ${MULTI_INDEX_LIBRARY_DIR}/include
    ${FOREACH_LIBRARY_DIR}/include
    ${MULTIPRECISION_LIBRARY_DIR}/include
    ${PROPERTY_TREE_LIBRARY_DIR}/include
    ${OPTIONAL_LIBRARY_DIR}/include
    ${FORMAT_LIBRARY_DIR}/include
    CACHE INTERNAL "BOOST_HEADERONLY_INCLUDE_DIR")


set(gtest_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest/include")

set(PO_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../boost/program_options)


SET(boost_program_options_sources
    ${PO_LIBRARY_DIR}/src/cmdline.cpp
    ${PO_LIBRARY_DIR}/src/config_file.cpp
    ${PO_LIBRARY_DIR}/src/convert.cpp
    ${PO_LIBRARY_DIR}/src/options_description.cpp
    ${PO_LIBRARY_DIR}/src/parsers.cpp
    ${PO_LIBRARY_DIR}/src/positional_options.cpp
    ${PO_LIBRARY_DIR}/src/split.cpp
    ${PO_LIBRARY_DIR}/src/utf8_codecvt_facet.cpp
    ${PO_LIBRARY_DIR}/src/value_semantic.cpp
    ${PO_LIBRARY_DIR}/src/variables_map.cpp
)

add_library(Boost_program_options STATIC ${boost_program_options_sources})
target_include_directories(Boost_program_options SYSTEM PUBLIC ${PO_LIBRARY_DIR}/include PRIVATE ${BOOST_HEADERONLY_INCLUDE_DIR})

target_compile_definitions(Boost_program_options PUBLIC BOOST_SYSTEM_NO_DEPRECATED)
