cmake_minimum_required(VERSION 4.2)
project(TrackRecoSkim LANGUAGES CXX)
if(NOT LDMXSW_INSTALL_PREFIX)
  set(LDMXSW_INSTALL_PREFIX $ENV{LDMX_SW_INSTALL})
endif()

find_path(
  ldmxsw_include_dir
  NAMES Process.h
  PATHS ${LDMXSW_INSTALL_PREFIX}/include
  PATH_SUFFIXES Framework
  REQUIRED)

# If ldmx-sw isn't found, throw an error
if(NOT ldmxsw_include_dir)
  message(FATAL_ERROR "ldmx-sw install not found. Give the path as
    '-DLDMX_INSTALL_PREFIX=<path-to-ldmx-sw-install>'")
endif()

# back out of include/Framework to get install prefix
cmake_path(GET ldmxsw_include_dir PARENT_PATH ldmxsw_install_prefix)
cmake_path(GET ldmxsw_install_prefix PARENT_PATH ldmxsw_install_prefix)
# link to ALL dynamic libraries in the ldmx-sw install prefix
# this is overkill if there are other libraries there but
# allows us to avoid having to update a list of names
file(GLOB ldmxsw_libs ${ldmxsw_install_prefix}/lib/*.so)

# Create the target
add_library(LDMXSW::Interface INTERFACE IMPORTED GLOBAL)
set_target_properties(
  LDMXSW::Interface
  PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${ldmxsw_install_prefix}/include"
             INTERFACE_LINK_LIBRARIES "${ldmxsw_libs}")

message(STATUS "Found ldmx-sw at: ${ldmxsw_install_prefix}")
set(LDMXSW_FOUND TRUE)

# If an install location hasn't been set via CMAKE_INSTALL_PREFIX,
# set it to a default depending on where ldmxsw is
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  if (ldmxsw_install_prefix MATCHES "^/usr/local.*")
    # system install of ldmx-sw implies usage of production image
    # have ldmx-analysis install default to be separate
    set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install CACHE PATH "" FORCE)
  else()
    # non-system install of ldmx-sw implies development
    # have ldmx-analysis installed along-side
    set(CMAKE_INSTALL_PREFIX ${ldmxsw_install_prefix} CACHE PATH "" FORCE)
  endif()
endif()
message(STATUS "Install track analyzer to: ${CMAKE_INSTALL_PREFIX}")

find_package(ROOT 6.16 CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS log)

add_library(TrackAnalyzer SHARED chelsea-code/ldmx/EAT_Tracking/TrackAnalyzer.cxx)
target_link_libraries(TrackAnalyzer PUBLIC LDMXSW::Interface ROOT::Hist ROOT::TreePlayer ROOT::Physics Boost::log)
install(TARGETS TrackAnalyzer LIBRARY DESTINATION lib)
