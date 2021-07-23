list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

if (NOT BUILD_ONLY_PYTHON_LIBRARY)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/sanitizers-cmake/cmake")
    add_subdirectory(sanitizers-cmake)
endif()

add_subdirectory(pybind11)
add_subdirectory(FunctionalPlus)
add_subdirectory(frugally-deep)
add_subdirectory(fastText)
add_subdirectory(googletest)

option(BUILD_TESTS OFF)
SET(BUILD_STATIC_LIBS ON)
SET(BUILD_SHARED_LIBS OFF)
SET(SAMPLE OFF)
SET(MI_BUILD_TESTS OFF)

add_subdirectory(mimalloc)
add_subdirectory(tinyxml2)
add_subdirectory(boost-cmake)
add_subdirectory(icu-cmake)
add_subdirectory(tabulate)

set(frugally-deep_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/frugally-deep/include" CACHE INTERNAL "frugally-deep_INCLUDE_DIR")

set(Eigen3_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/eigen" CACHE INTERNAL "Eigen3_INCLUDE_DIR")

set(FunctionalPlus_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/FunctionalPlus/include" CACHE INTERNAL "FunctionalPlus_INCLUDE_DIR")

set(nlohmann_json_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/json/include" CACHE INTERNAL "nlohmann_json_INCLUDE_DIR")

set(fasttext_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/fastText/src" CACHE INTERNAL "fasttext_INCLUDE_DIR")

set(incbin_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/incbin" CACHE INTERNAL "incbin_INCLUDE_DIR")

set(gtest_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest/include" CACHE INTERNAL "gtest_INCLUDE_DIR")
set(gtest_LIBRARIES gtest gtest_main CACHE INTERNAL "gtest_LIBRARIES")

set(TinyXML2_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tinyxml2" CACHE INTERNAL "TinyXML2_INCLUDE_DIR")
set(TinyXML2_LIBRARIES tinyxml2 CACHE INTERNAL "TinyXML2_LIBRARIES")

set(MIMALLOC_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mimalloc" CACHE INTERNAL "MIMALLOC_LIBRARY_DIR")
set(MIMALLOC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mimalloc/include" CACHE INTERNAL "MIMALLOC_INCLUDE_DIR")
