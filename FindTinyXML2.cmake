# Copyright (c) 2014 Andrew Kelley
# This file is MIT licensed.
# See http://opensource.org/licenses/MIT

# TINYXML2_FOUND
# TINYXML2_INCLUDE_DIR
# TINYXML2_LIBRARIES

find_path(TinyXML2_INCLUDE_DIR NAMES tinyxml2.h)
find_library(TinyXML2_LIBRARIES NAMES tinyxml2)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TinyXML2 DEFAULT_MSG TinyXML2_LIBRARIES TinyXML2_INCLUDE_DIR)

mark_as_advanced(TinyXML2_INCLUDE_DIR TinyXML2_LIBRARIES)
