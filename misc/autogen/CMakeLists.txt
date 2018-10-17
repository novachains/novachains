cmake_minimum_required( VERSION 2.6 )

project( AUTOGEN )

# set current directory
SET ( AUTOGEN_CUR_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )
# set executable output directory #set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${AUTOGEN_CUR_DIR}/bin" )
set ( EXECUTABLE_OUTPUT_PATH "${AUTOGEN_CUR_DIR}/bin" )

# set root directory
IF ( NOT NOVA_ROOT_DIR )
   MESSAGE( STATUS "SET NOVA_ROOT_DIR" )
   STRING ( REGEX REPLACE "/[^/|\\]*$" "" NOVA_ROOT_DIR_TMP ${AUTOGEN_CUR_DIR} )
   STRING ( REGEX REPLACE "/[^/|\\]*$" "" NOVA_ROOT_DIR ${NOVA_ROOT_DIR_TMP} )

   # add modules directory
   list( APPEND CMAKE_MODULE_PATH "${NOVA_ROOT_DIR}/cmake/Modules" )
   # set include directory
   set ( NOVA_INCLUDE_DIR "${NOVA_ROOT_DIR}/src/include" )
   # include directory and import novaconf
   INCLUDE_DIRECTORIES( ${NOVA_INCLUDE_DIR} )

   INCLUDE( novaoption )
   INCLUDE( novaenv )
   INCLUDE( novatool )
   INCLUDE( novaconf )

   # print tip
   MESSAGE( STATUS "" )
   MESSAGE( STATUS "++++++++++++++++++++++++++++++++" )
   MESSAGE( STATUS "Operator system is: ${CMAKE_SYSTEM}" )
   MESSAGE( STATUS "Project Name is: ${PROJECT_NAME}" )
   MESSAGE( STATUS "Current directory is: ${CMAKE_CURRENT_SOURCE_DIR}" )
   MESSAGE( STATUS "Root directory is: ${NOVA_ROOT_DIR}")
   MESSAGE( STATUS "Include directory is : ${NOVA_INCLUDE_DIR}" )
   MESSAGE( STATUS "Executable output directory is: ${EXECUTABLE_OUTPUT_PATH}")
   MESSAGE( STATUS "--------------------------------" )
   MESSAGE( STATUS "" )
ENDIF()

# version
SET ( CURVERSION 1 )
# get version
SUBVERSION( ${NOVA_ROOT_DIR} CURVERSION )
MESSAGE( STATUS "Code Version is: ${CURVERSION}" )

# update version configure file
SET( VER_CONF_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ver_conf.h.in" )
SET( GEN_VER_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/ver_conf.h" )
FILE( READ ${VER_CONF_FILE} CONFTEXT )
STRING( REPLACE "$WCREV$" ${CURVERSION} NEW_CONFTEXT ${CONFTEXT} )
FILE( WRITE ${GEN_VER_FILE} ${NEW_CONFTEXT} )
MESSAGE( STATUS "Generate ${GEN_VER_FILE}" )

AUX_SOURCE_DIRECTORY( .             AUTOGEN_SRCS )
AUX_SOURCE_DIRECTORY( fileList      AUTOGEN_SRCS )
AUX_SOURCE_DIRECTORY( options       AUTOGEN_SRCS )
AUX_SOURCE_DIRECTORY( rc            AUTOGEN_SRCS )
AUX_SOURCE_DIRECTORY( spidermonkey  AUTOGEN_SRCS )
AUX_SOURCE_DIRECTORY( trace         AUTOGEN_SRCS )
AUX_SOURCE_DIRECTORY( version       AUTOGEN_SRCS )

ADD_EXECUTABLE( autogen ${AUTOGEN_SRCS} )

# run auto gen
ADD_CUSTOM_TARGET( DOGEN ALL
                   COMMAND autogen
                   DEPENDS autogen )
