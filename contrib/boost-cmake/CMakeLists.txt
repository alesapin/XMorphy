add_definitions(-Wno-unused-variable -Wno-deprecated-declarations)

set(PO_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/../program_options)

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
target_include_directories(Boost_program_options SYSTEM PUBLIC ${PO_LIBRARY_DIR}/include)
target_compile_definitions(Boost_program_options PUBLIC BOOST_SYSTEM_NO_DEPRECATED)
