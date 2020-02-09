pybind11_add_module(pyxmorphy xmorphy.cpp)
target_link_libraries(pyxmorphy PUBLIC Xmorphy)
