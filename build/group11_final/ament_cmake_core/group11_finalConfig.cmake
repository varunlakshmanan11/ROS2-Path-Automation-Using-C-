# generated from ament/cmake/core/templates/nameConfig.cmake.in

# prevent multiple inclusion
if(_group11_final_CONFIG_INCLUDED)
  # ensure to keep the found flag the same
  if(NOT DEFINED group11_final_FOUND)
    # explicitly set it to FALSE, otherwise CMake will set it to TRUE
    set(group11_final_FOUND FALSE)
  elseif(NOT group11_final_FOUND)
    # use separate condition to avoid uninitialized variable warning
    set(group11_final_FOUND FALSE)
  endif()
  return()
endif()
set(_group11_final_CONFIG_INCLUDED TRUE)

# output package information
if(NOT group11_final_FIND_QUIETLY)
  message(STATUS "Found group11_final: 1.0.0 (${group11_final_DIR})")
endif()

# warn when using a deprecated package
if(NOT "" STREQUAL "")
  set(_msg "Package 'group11_final' is deprecated")
  # append custom deprecation text if available
  if(NOT "" STREQUAL "TRUE")
    set(_msg "${_msg} ()")
  endif()
  # optionally quiet the deprecation message
  if(NOT ${group11_final_DEPRECATED_QUIET})
    message(DEPRECATION "${_msg}")
  endif()
endif()

# flag package as ament-based to distinguish it after being find_package()-ed
set(group11_final_FOUND_AMENT_PACKAGE TRUE)

# include all config extra files
set(_extras "")
foreach(_extra ${_extras})
  include("${group11_final_DIR}/${_extra}")
endforeach()
