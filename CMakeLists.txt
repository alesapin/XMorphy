project(XMorphy)
cmake_minimum_required(VERSION 3.0.2)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Boost COMPONENTS locale program_options regex filesystem  REQUIRED)
find_package(ICU REQUIRED)
find_package(TinyXML2 REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
include_directories(${TinyXML2_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Werror")
SET(BUILD_SHARED_LIBRARIES OFF)
set(XMORPHY_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_subdirectory(contrib)

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
add_headers_and_sources(xmorphy src/ml)
add_headers_and_sources(xmorphy src/IO)
add_headers_and_sources(xmorphy src/phem)
add_headers_and_sources(xmorphy src/tag)

add_library(Xmorphy STATIC ${xmorphy_headers} ${xmorphy_sources})
target_include_directories (Xmorphy PUBLIC ${XMORPHY_INCLUDE_DIR})
target_include_directories (Xmorphy PUBLIC ${catboost_model_headers})
target_link_libraries(Xmorphy ${Boost_LIBRARIES} ${ICU_LIBRARIES} ${TinyXML2_LIBRARIES} crfpp dl)

set(CLI_SOURCE_FILES programs/cli/main.cpp)
add_executable(xmorphy ${CLI_SOURCE_FILES})

target_include_directories(xmorphy PRIVATE ${CPP_RESOURCE_INCLUDE_DIR})
target_link_libraries(xmorphy Xmorphy)

set(BUILDER_SOURCE_FILES programs/dictbuilder/main.cpp)
add_executable(xmorphy-builder ${BUILDER_SOURCE_FILES})
target_link_libraries(xmorphy-builder Xmorphy)

add_resource(xmorphy "hyphdict"      "${CMAKE_CURRENT_SOURCE_DIR}/data/dicts/hyphdict.txt")
add_resource(xmorphy "phemdict_b"    "${CMAKE_CURRENT_SOURCE_DIR}/data/dicts/phemdict_b")
add_resource(xmorphy "phemdict_f"    "${CMAKE_CURRENT_SOURCE_DIR}/data/dicts/phemdict_f")
add_resource(xmorphy "phemdict_m"    "${CMAKE_CURRENT_SOURCE_DIR}/data/dicts/phemdict_m")
add_resource(xmorphy "prefixdict"    "${CMAKE_CURRENT_SOURCE_DIR}/data/dicts/prefixdict.txt")
add_resource(xmorphy "affixdict"     "${CMAKE_CURRENT_SOURCE_DIR}/data/dicts/affixdict.bin")
add_resource(xmorphy "maindict"      "${CMAKE_CURRENT_SOURCE_DIR}/data/dicts/maindict.bin")
add_resource(xmorphy "disambdict"    "${CMAKE_CURRENT_SOURCE_DIR}/data/dicts/disambdict.bin")
add_resource(xmorphy "suffixdict"    "${CMAKE_CURRENT_SOURCE_DIR}/data/dicts/suffixdict.bin")

set(MULTIPLIER_SOURCE_FILES programs/multiplier/main.cpp)
add_executable(multiplier ${MULTIPLIER_SOURCE_FILES})
target_link_libraries(multiplier Xmorphy)

set(JANITOR_SOURCE_FILES programs/janitor/main.cpp)
add_executable(janitor ${JANITOR_SOURCE_FILES})
target_link_libraries(janitor Xmorphy)


install(TARGETS xmorphy Xmorphy
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
)

add_custom_command(TARGET xmorphy POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:xmorphy>)

add_custom_command(TARGET xmorphy POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       ${catboost_model_lib} $<TARGET_FILE_DIR:xmorphy>)

install(DIRECTORY data/models DESTINATION share/xmorphy)
install(DIRECTORY data/dicts DESTINATION share/xmorphy)
install(FILES ${catboost_model_lib} DESTINATION lib)
