# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# FindASSIMP
# --------
#
# Find the OpenGL Extension Wrangler Library (ASSIMP)
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the :prop_tgt:`IMPORTED` target ``ASSIMP::ASSIMP``,
# if ASSIMP has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   ASSIMP_INCLUDE_DIRS - include directories for ASSIMP
#   ASSIMP_LIBRARIES - libraries to link against ASSIMP
#   ASSIMP_FOUND - true if ASSIMP has been found and can be used

find_path(ASSIMP_INCLUDE_DIR assimp/scene.h)

find_library(ASSIMP_LIBRARY_RELEASE NAMES assimp-vc140-mt PATH_SUFFIXES Release)
find_library(ASSIMP_LIBRARY_DEBUG NAMES assimp-vc140-mt PATH_SUFFIXES Debug)

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(ASSIMP
                                  REQUIRED_VARS ASSIMP_INCLUDE_DIR ASSIMP_LIBRARY_DEBUG)

mark_as_advanced(ASSIMP_LIBRARY_RELEASE)
mark_as_advanced(ASSIMP_LIBRARY_DEBUG)
mark_as_advanced(ASSIMP_INCLUDE_DIR)
