project(XMorphy)
cmake_minimum_required(VERSION 3.0.2)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


include_directories(${Boost_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Werror")
set(BUILD_SHARED_LIBRARIES OFF)
set(XMORPHY_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_subdirectory(contrib)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/contrib/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})

find_package(Boost COMPONENTS locale program_options regex filesystem REQUIRED)
find_package(ICU REQUIRED)
find_package(TinyXML2 REQUIRED)
find_package(Sanitizers)

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

add_headers_and_sources(xmorphy src/build)
add_headers_and_sources(xmorphy src/graphem)
add_headers_and_sources(xmorphy src/disamb)
add_headers_and_sources(xmorphy src/DAWG)
add_headers_and_sources(xmorphy src/utils)
add_headers_and_sources(xmorphy src/morph)
add_headers_and_sources(xmorphy src/IO)
add_headers_and_sources(xmorphy src/tag)
add_headers_and_sources(xmorphy src/ml)

add_library(Xmorphy STATIC ${xmorphy_headers} ${xmorphy_sources})
target_include_directories (Xmorphy PUBLIC ${XMORPHY_INCLUDE_DIR})
target_include_directories (Xmorphy PUBLIC ${CPP_RESOURCE_INCLUDE_DIR})
target_include_directories(Xmorphy PUBLIC ${frugally-deep_INCLUDE_DIR} ${Eigen3_INCLUDE_DIR} ${FunctionalPlus_INCLUDE_DIR} ${nlohmann_json_INCLUDE_DIR} ${fasttext_INCLUDE_DIR})
target_link_libraries(Xmorphy ${Boost_LIBRARIES} ${ICU_LIBRARIES} ${fasttext_LIBRARIES} -lpthread)

add_resource(Xmorphy "hyphdict"      "${CMAKE_CURRENT_SOURCE_DIR}/data/dicts/hyphdict.txt")
add_resource(Xmorphy "prefixdict"    "${CMAKE_CURRENT_SOURCE_DIR}/data/dicts/prefixdict.txt")
add_resource(Xmorphy "affixdict"     "${CMAKE_CURRENT_SOURCE_DIR}/data/dicts/udaffixdict.bin")
add_resource(Xmorphy "maindict"      "${CMAKE_CURRENT_SOURCE_DIR}/data/dicts/udmaindict.bin")
add_resource(Xmorphy "disambdict"    "${CMAKE_CURRENT_SOURCE_DIR}/data/dicts/uddisambdict.bin")
add_resource(Xmorphy "suffixdict"    "${CMAKE_CURRENT_SOURCE_DIR}/data/dicts/udsuffixdict.bin")
add_resource(Xmorphy "embeddings"    "${CMAKE_CURRENT_SOURCE_DIR}/data/models/morphorueval_cbow.embedding_50.bin")
add_resource(Xmorphy "disambmodel"   "${CMAKE_CURRENT_SOURCE_DIR}/data/models/disamb_50.json")
add_resource(Xmorphy "morphemmodel"  "${CMAKE_CURRENT_SOURCE_DIR}/data/models/morphem.json")

set(CLI_SOURCE_FILES programs/cli/main.cpp)
add_executable(xmorphy ${CLI_SOURCE_FILES})
add_sanitizers(xmorphy)

target_link_libraries(xmorphy Xmorphy)

set(BUILDER_SOURCE_FILES programs/dictbuilder/main.cpp)
add_executable(xmorphy-builder ${BUILDER_SOURCE_FILES})
target_link_libraries(xmorphy-builder Xmorphy ${TinyXML2_LIBRARIES})
target_include_directories(xmorphy-builder PRIVATE ${TinyXML2_INCLUDE_DIR})

set(CONVERTER_SOURCE_FILES
  programs/dictconverter/main.cpp
  programs/dictconverter/OpCorporaUDConverter.cpp
  programs/dictconverter/XMLDictLoader.cpp)
add_executable(xmorphy-converter ${CONVERTER_SOURCE_FILES})
target_link_libraries(xmorphy-converter Xmorphy ${TinyXML2_LIBRARIES})
target_include_directories(xmorphy-converter PRIVATE ${TinyXML2_INCLUDE_DIR})
add_sanitizers(xmorphy-converter)

set(MODEL_TEST_SOURCE_FILES programs/model_test/main.cpp)
add_executable(model-test ${MODEL_TEST_SOURCE_FILES})
target_link_libraries(model-test Xmorphy)
add_sanitizers(model-test)

install(TARGETS xmorphy Xmorphy
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
)

add_custom_command(TARGET xmorphy POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:xmorphy>)

install(DIRECTORY data/models DESTINATION share/xmorphy)
install(DIRECTORY data/dicts DESTINATION share/xmorphy)

pybind11_add_module(pyxmorphy python/xmorphy.cpp)
target_link_libraries(pyxmorphy PUBLIC Xmorphy)
