cmake_minimum_required(VERSION 3.0.2)

project(XMorphy)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


set (CMAKE_CXX_STANDARD 17)
set (CMAKE_POSITION_INDEPENDENT_CODE ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Werror -ffast-math")
set (CMAKE_CXX_FLAGS                     "${CMAKE_CXX_FLAGS} ${COMPILER_FLAGS} ${PLATFORM_EXTRA_CXX_FLAG} ${COMMON_WARNING_FLAGS} ${CXX_WARNING_FLAGS}")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO      "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3 ${CMAKE_CXX_FLAGS_ADD}")
set (CMAKE_CXX_FLAGS_DEBUG               "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -ggdb3 -fno-inline ${CMAKE_CXX_FLAGS_ADD}")

set (CMAKE_C_FLAGS                       "${CMAKE_C_FLAGS} ${COMPILER_FLAGS} ${COMMON_WARNING_FLAGS} ${CMAKE_C_FLAGS_ADD} -ffast-math")
set (CMAKE_C_FLAGS_RELWITHDEBINFO        "${CMAKE_C_FLAGS_RELWITHDEBINFO} -O3 ${CMAKE_C_FLAGS_ADD}")
set (CMAKE_C_FLAGS_DEBUG                 "${CMAKE_C_FLAGS_DEBUG} -O0 -g3 -ggdb3 -fno-inline ${CMAKE_C_FLAGS_ADD}")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set (COMPILER_GCC 1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
    set (COMPILER_CLANG 1) # Safe to treat AppleClang as a regular Clang, in general.
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set (COMPILER_CLANG 1)
endif ()

# Example values: `lld-10`, `gold`.
option (LINKER_NAME "Linker name or full path")

if (COMPILER_GCC AND NOT LINKER_NAME)
    find_program (LLD_PATH NAMES "ld.lld")
    find_program (GOLD_PATH NAMES "ld.gold")
elseif (NOT LINKER_NAME)
    find_program (LLD_PATH NAMES "ld.lld-${COMPILER_VERSION_MAJOR}" "lld-${COMPILER_VERSION_MAJOR}" "ld.lld" "lld")
    find_program (GOLD_PATH NAMES "ld.gold" "gold")
endif ()

if (OS_LINUX AND NOT LINKER_NAME)
    # We prefer LLD linker over Gold or BFD on Linux.
    if (LLD_PATH)
        if (COMPILER_GCC)
            # GCC driver requires one of supported linker names like "lld".
            set (LINKER_NAME "lld")
        else ()
            # Clang driver simply allows full linker path.
            set (LINKER_NAME ${LLD_PATH})
        endif ()
    endif ()

    if (NOT LINKER_NAME)
        if (GOLD_PATH)
            if (COMPILER_GCC)
                set (LINKER_NAME "gold")
            else ()
                set (LINKER_NAME ${GOLD_PATH})
            endif ()
        endif ()
    endif ()
endif ()

if (LINKER_NAME)
    if (COMPILER_CLANG AND (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 12.0.0 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 12.0.0))
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --ld-path=${LINKER_NAME}")
        set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --ld-path=${LINKER_NAME}")
    else ()
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=${LINKER_NAME}")
        set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=${LINKER_NAME}")
    endif ()

    message(STATUS "Using custom linker by name: ${LINKER_NAME}")
endif ()

if (COMPILER_CLANG)
    set (CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -flto=thin")
    set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -flto=thin")
    set (CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} -flto=thin")
endif()

option(BUILD_ONLY_PYTHON_LIBRARY "Build only python library" OFF)

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "None")
    message (STATUS "CMAKE_BUILD_TYPE is not set, set to default = RELWITHDEBINFO")
    set (CMAKE_BUILD_TYPE "RELWITHDEBINFO")
endif ()
string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_UC)
message (STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")


set(BUILD_SHARED_LIBRARIES OFF)
set(XMorphy_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_subdirectory(contrib)

include(cmake/icu.cmake)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ${CMAKE_FIND_LIBRARY_SUFFIXES})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/dicts DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/models DESTINATION ${CMAKE_BINARY_DIR})

if (NOT BUILD_ONLY_PYTHON_LIBRARY)
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/contrib/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})
    find_package(Sanitizers)
endif()

set(xmorphy_headers)
set(xmorphy_sources)

macro(add_glob cur_list)
    file(GLOB __tmp RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${ARGN})
    list(APPEND ${cur_list} ${__tmp})
endmacro()

macro(add_headers_and_sources prefix common_path)
    add_glob(${prefix}_headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${common_path}/*.h)
    add_glob(${prefix}_sources ${common_path}/*.cpp ${common_path}/*.h)
endmacro()

add_headers_and_sources(xmorphy src/xmorphy/build)
add_headers_and_sources(xmorphy src/xmorphy/graphem)
add_headers_and_sources(xmorphy src/xmorphy/DAWG)
add_headers_and_sources(xmorphy src/xmorphy/utils)
add_headers_and_sources(xmorphy src/xmorphy/morph)
add_headers_and_sources(xmorphy src/xmorphy/tag)
add_headers_and_sources(xmorphy src/xmorphy/ml)

add_library(Xmorphy STATIC ${xmorphy_headers} ${xmorphy_sources})

target_include_directories (Xmorphy PUBLIC ${XMorphy_INCLUDE_DIR} ${MIMALLOC_INCLUDE_DIR} PRIVATE ${Boost_INCLUDE_DIR} ${ICU_INCLUDE_DIRS})

target_include_directories(Xmorphy PUBLIC
  ${frugally-deep_INCLUDE_DIR}
  ${Eigen3_INCLUDE_DIR}
  ${FunctionalPlus_INCLUDE_DIR}
  ${nlohmann_json_INCLUDE_DIR}
  ${fasttext_INCLUDE_DIR}
  ${incbin_INCLUDE_DIR})

target_link_libraries(Xmorphy PUBLIC ${ICU_LIBRARIES} mimalloc-static PRIVATE fasttext-static_pic tabulate::tabulate -ldl -lpthread)

if (NOT BUILD_ONLY_PYTHON_LIBRARY)
    add_sanitizers(Xmorphy)

    set(CLI_SOURCE_FILES programs/cli/main.cpp)
    add_executable(xmorphy ${CLI_SOURCE_FILES})
    target_link_libraries(xmorphy PRIVATE Boost_program_options)
    target_link_libraries(xmorphy PRIVATE Xmorphy -static-libgcc -static-libstdc++)

    add_sanitizers(xmorphy)

    set(BUILDER_SOURCE_FILES programs/dictbuilder/main.cpp)
    add_executable(xmorphy-builder ${BUILDER_SOURCE_FILES})
    target_link_libraries(xmorphy-builder PRIVATE Xmorphy ${TinyXML2_LIBRARIES} Boost_program_options)
    target_include_directories(xmorphy-builder PRIVATE ${TinyXML2_INCLUDE_DIR})
    add_sanitizers(xmorphy-builder)

    set(CONVERTER_SOURCE_FILES
      programs/dictconverter/main.cpp
      programs/dictconverter/OpCorporaUDConverter.cpp
      programs/dictconverter/XMLDictLoader.cpp)

    add_executable(xmorphy-converter ${CONVERTER_SOURCE_FILES})
    target_link_libraries(xmorphy-converter PRIVATE Xmorphy ${TinyXML2_LIBRARIES} Boost_program_options)
    target_include_directories(xmorphy-converter PRIVATE ${TinyXML2_INCLUDE_DIR})
    add_sanitizers(xmorphy-converter)

    #set(MORPHEMMER_SOURCE_FILES programs/morphemmer/main.cpp)
    #add_executable(morphemmer ${MORPHEMMER_SOURCE_FILES})
    #target_include_directories(morphemmer PRIVATE ${frugally-deep_INCLUDE_DIR} ${Eigen3_INCLUDE_DIR} ${FunctionalPlus_INCLUDE_DIR} ${nlohmann_json_INCLUDE_DIR} ${incbin_INCLUDE_DIR})
    #target_link_libraries(morphemmer PRIVATE -Wl,--whole-archive -lpthread -Wl,--no-whole-archive -pthread -static-libgcc -static-libstdc++ -lrt)
    #add_sanitizers(morphemmer)

    #set(LEXEME_SOURCE_FILES programs/lexeme/main.cpp)
    #add_executable(lexeme ${LEXEME_SOURCE_FILES})
    #add_sanitizers(lexeme)
    #target_link_libraries(lexeme PRIVATE Xmorphy)

    set(PERF_SOURCE_FILES programs/perf/main.cpp)
    add_executable(perf ${PERF_SOURCE_FILES})
    target_link_libraries(perf PRIVATE Xmorphy)

    add_executable(xmorphy_unit_tests
      ${CMAKE_CURRENT_SOURCE_DIR}/tests/ut/gtest_unistring.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/tests/ut/gtest_tokenizer.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/tests/ut/gtest_synthesis.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/tests/ut/gtest_dictanalyzer.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/tests/ut/gtest_splitter.cpp
    )

    target_link_libraries(xmorphy_unit_tests PRIVATE ${gtest_LIBRARIES} Xmorphy)
    add_sanitizers(xmorphy_unit_tests)
endif()

pybind11_add_module(pyxmorphy python/xmorphy.cpp)

target_include_directories(pyxmorphy PRIVATE ${XMorphy_INCLUDE_DIR})
target_link_libraries(pyxmorphy PRIVATE Xmorphy)
