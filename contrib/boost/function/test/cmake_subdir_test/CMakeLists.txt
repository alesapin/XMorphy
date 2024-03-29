# Copyright 2018, 2019 Peter Dimov
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5)

project(cmake_subdir_test LANGUAGES CXX)

add_subdirectory(../.. boostorg/function)

# boost_add_subdir

function(boost_add_subdir name)

    add_subdirectory(../../../${name} boostorg/${name})

endfunction()

# primary dependencies

boost_add_subdir(assert)
boost_add_subdir(bind)
boost_add_subdir(config)
boost_add_subdir(core)
boost_add_subdir(integer)
boost_add_subdir(preprocessor)
boost_add_subdir(throw_exception)
boost_add_subdir(type_index)
boost_add_subdir(type_traits)
boost_add_subdir(typeof)

# secondary dependencies

boost_add_subdir(static_assert)
boost_add_subdir(container_hash)
boost_add_subdir(describe)
boost_add_subdir(mp11)

# --target check

add_executable(quick ../quick.cpp)
target_link_libraries(quick Boost::function Boost::core)

enable_testing()
add_test(quick quick)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C $<CONFIG>)
