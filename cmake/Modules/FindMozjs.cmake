# Tries to find Mozjs
#
# Usage of this module as follows:
#
#     FIND_PACKEGE( Mozjs [version] [REQUIRED] )
#
# Variables used by this module, they can change the default behaviors and need
# to be set before calling FIND_PACKEGE:
#
# MOZJS_FIND_INCLUDE_PATH  Set this variable to specify the file search path
# MOZJS_FIND_LIB_PATH      Set this variable to specify the lib search path
# MOZJS_USE_DLL            Set this variable to specify whether search static
#                          library or dynamic library
# MOZJS_VERION_EXACT       Must the same version with specified
#
# Variables defined by this module:
#
# Mozjs_FOUND              Whether the module found or not
# Mozjs_INCLUDE_DIRS       The location of Mozjs headers
# Mozjs_LIBRARY_DIRS       The location of Mozjs library
# Mozjs_LIBRARIES          The libraries
# Mozjs_VERSION            The module version
# Mozjs_JS_THREADSAFE      Whether JS_THREADSAFE has defined or not

IF ( NOT MOZJS_FIND_INCLUDE_PATH )
   LIST( APPEND MOZJS_FIND_INCLUDE_PATH "/usr/include/js" "/usr/include/*" "/usr/local/include/*" )
ENDIF()

IF ( NOT MOZJS_FIND_LIB_PATH )
   LIST( APPEND MOZJS_FIND_LIB_PATH "/usr/lib" "/usr/local/lib/" )
ENDIF()

IF ( NOT MOZJS_USE_DLL )
   SET( MOZJS_USE_DLL FALSE )
ENDIF()

IF ( NOT MOZJS_VERION_EXACT )
   SET ( MOZJS_VERION_EXACT FALSE )
ENDIF()

SET ( Mozjs_LIB_PREFIX "lib" )
SET ( Mozjs_LIBRARY_NAME "${Mozjs_LIB_PREFIX}mozjs" )
SET ( Mozjs_VERSION "" )
SET( Mozjs_JS_THREADSAFE FALSE )

MESSAGE( STATUS "" )
MESSAGE( STATUS "" )

# find headers
FIND_PATH( Mozjs_INCLUDE_DIRS jsapi.h PATHS ${MOZJS_FIND_INCLUDE_PATH} )

# find version file directory
FIND_PATH( Mozjs_VERFILE_DIRS jsversion.h PATHS ${MOZJS_FIND_INCLUDE_PATH} )
# has found
IF ( Mozjs_VERFILE_DIRS )
   FILE( READ "${Mozjs_VERFILE_DIRS}/jsversion.h" MOZJS_VER_CONTEXT )
   STRING ( REGEX MATCH "#define *JS_VERSION *[0-9]*" MOZJS_VER_CONTEXT_TMP ${MOZJS_VER_CONTEXT} )
   STRING ( REGEX MATCH "[0-9]*$" Mozjs_VERSION ${MOZJS_VER_CONTEXT_TMP} )
   SET( Mozjs_LIBRARY_NAME "${Mozjs_LIBRARY_NAME}${Mozjs_VERSION}" )
   
   IF ( Mozjs_FIND_VERSION )
      IF ( MOZJS_VERION_EXACT )
         IF ( NOT "${Mozjs_FIND_VERSION}" STREQUAL "${Mozjs_VERSION}" )
            MESSAGE( FATAL_ERROR "Detected version of Mozjs(${Mozjs_VERSION}) is too old. Requested version was ${Mozjs_FIND_VERSION}" )
         ENDIF()
      ELSE()
         IF ( "${Mozjs_VERSION}" STRLESS "${Mozjs_FIND_VERSION}" )
            MESSAGE( FATAL_ERROR "Detected version of Mozjs(${Mozjs_VERSION}) is too old. Requested version was ${Mozjs_FIND_VERSION} (or newer)" )
         ENDIF()
      ENDIF()
   ENDIF()
   
   # configure file
   FILE( READ "${Mozjs_VERFILE_DIRS}/js-config.h" MOZJS_CONFIG_CONTEXT )
   STRING ( REGEX MATCH "(^|[.]*\n) *#define +JS_THREADSAFE +[0-9]*" MOZJS_CONFIG_CONTEXT_TMP ${MOZJS_CONFIG_CONTEXT} )
   IF ( MOZJS_CONFIG_CONTEXT_TMP )
      SET( Mozjs_JS_THREADSAFE TRUE )
   ENDIF()
ENDIF()

IF ( MOZJS_USE_DLL )
   LIST( APPEND Mozjs_LIBRARY_NAMES "${Mozjs_LIBRARY_NAME}.so" )
ELSE()
   LIST( APPEND Mozjs_LIBRARY_NAMES "${Mozjs_LIBRARY_NAME}.a" "${Mozjs_LIBRARY_NAME}-1.0.a" )
ENDIF()

# find libraries
FIND_LIBRARY( Mozjs_LIBRARIES NAMES ${Mozjs_LIBRARY_NAMES} PATHS ${MOZJS_FIND_LIB_PATH} )
# find libraries directory
STRING( REGEX MATCH "${Mozjs_LIBRARY_NAME}.*$" Mozjs_LIB_SHOT_NAME ${Mozjs_LIBRARIES} )
FIND_PATH( Mozjs_LIBRARY_DIRS ${Mozjs_LIB_SHOT_NAME} PATHS ${MOZJS_FIND_LIB_PATH} )

IF( Mozjs_INCLUDE_DIRS AND Mozjs_LIBRARIES AND Mozjs_LIBRARY_DIRS )
   SET( Mozjs_FOUND TRUE )
ENDIF()

# print and report
IF ( Mozjs_FOUND )
   IF ( NOT Mozjs_FIND_QUIETLY )
      MESSAGE( STATUS "Found Mozjs, verion is: ${Mozjs_VERSION}" )
      MESSAGE( STATUS "  Mozjs include dir: ${Mozjs_INCLUDE_DIRS}" )
      MESSAGE( STATUS "  Mozjs library dir: ${Mozjs_LIBRARY_DIRS}" )
      MESSAGE( STATUS "  Mozjs libraries: ${Mozjs_LIBRARIES}" )
   ENDIF()
ELSE()
   IF ( Mozjs_FIND_REQUIRED )
      MESSAGE( FATAL_ERROR "Cound not find mozjs library" )
   ENDIF()
ENDIF()

