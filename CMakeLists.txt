cmake_minimum_required( VERSION 3.20 )
project(
  libVC_injectTest
  VERSION 2.0
  DESCRIPTION
    "Libvc inject test"
  LANGUAGES CXX C)
# target
set ( EXE_NAME "run_loadSo_test" )
# look for dependencies
list ( APPEND CMAKE_MODULE_PATH "/usr/local/lib/cmake" )
list ( APPEND CMAKE_MODULE_PATH "./cmake" )
#----- Set the name of the Version Compiler library

find_package ( LibVersioningCompiler REQUIRED )
include_directories ( ${LIBVC_INCLUDE_DIRS} )
link_directories( ${LIVC_LIB_DIR} )

find_package(Boost QUIET REQUIRED )

# finalize dependencies
set ( DEP_LIBS ${DEP_LIBS} ${LIBVC_LIBRARIES} )

# sources
set ( SRC_INJTST ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
                )
# compiler options
if(NOT CMAKE_BUILD_TYPE)
  message("No build type detected. Build type set to Release by default")
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)

set_source_files_properties( ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp PROPERTIES COMPILE_FLAGS
"-D_PATH_TO_KERNEL=\"\\\"${CMAKE_CURRENT_SOURCE_DIR}/\\\"\"")
# finalize target
add_executable( ${EXE_NAME} ${SRC_INJTST} )
target_link_libraries( ${EXE_NAME} ${DEP_LIBS} )

# install path
if ( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
  set (CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})
endif ()

install ( TARGETS ${EXE_NAME} DESTINATION bin )