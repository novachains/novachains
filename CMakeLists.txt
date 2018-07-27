cmake_minimum_required( VERSION 2.6 )

project( NOVA )
ENABLE_TESTING()

# options
OPTION ( USE_AUTOGEN "Use auto gen" ON )

# set root directory
set ( NOVA_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )
# set output directory
set ( NOVA_OUTPUT_DIR "${NOVA_ROOT_DIR}/output" )
# set build directory
SET ( NOVA_BUILD_DIR "${NOVA_ROOT_DIR}/build" )
# set executable output directory #set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${NOVA_OUTPUT_DIR}/bin" )
set ( EXECUTABLE_OUTPUT_PATH "${NOVA_OUTPUT_DIR}/bin" )
# set library output directory #set ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${NOVA_OUTPUT_DIR}/lib" )
set ( LIBRARY_OUTPUT_PATH "${NOVA_OUTPUT_DIR}/lib" )

# add modules directory
list( APPEND CMAKE_MODULE_PATH "${NOVA_ROOT_DIR}/cmake/Modules" )

# set include directory
set ( NOVA_INCLUDE_DIR "${NOVA_ROOT_DIR}/src/include" )
INCLUDE_DIRECTORIES( ${NOVA_INCLUDE_DIR} )
LINK_DIRECTORIES(${LIBRARY_OUTPUT_PATH})
INCLUDE( novatool )
INCLUDE( novaconf )

# print tip
MESSAGE( STATUS "" )
MESSAGE( STATUS "++++++++++++++++++++++++++++++++" )
MESSAGE( STATUS "Operator system is: ${CMAKE_SYSTEM}" )
MESSAGE( STATUS "Project Name is: ${PROJECT_NAME}" )
MESSAGE( STATUS "Root directory is: ${NOVA_ROOT_DIR}")
MESSAGE( STATUS "Executable output directory is: ${EXECUTABLE_OUTPUT_PATH}")
MESSAGE( STATUS "Library output directory is: ${LIBRARY_OUTPUT_PATH}")
MESSAGE( STATUS "Build output directory is: ${NOVA_BUILD_DIR}")
MESSAGE( STATUS "--------------------------------" )
MESSAGE( STATUS "" )

# auto gen
IF( USE_AUTOGEN )
   ADD_SUBDIRECTORY( misc/autogen )
ENDIF()
# source directory
ADD_SUBDIRECTORY( src ${NOVA_BUILD_DIR}/src )
# test directory
ADD_SUBDIRECTORY( test ${NOVA_BUILD_DIR}/test )
