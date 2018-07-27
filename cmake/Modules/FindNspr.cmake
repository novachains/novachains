# Tries to find Nspr
#
# Usage of this module as follows:
#
#     FIND_PACKEGE( Nspr [REQUIRED] )
#
# Variables used by this module, they can change the default behaviors and need
# to be set before calling FIND_PACKEGE:
#
# NSPR_FIND_INCLUDE_PATH   Set this variable to specify the file search path
# NSPR_FIND_LIB_PATH       Set this variable to specify the lib search path
# NSPR_USE_DLL             Set this variable to specify whether search static
#                          library or dynamic library
#
# Variables defined by this module:
#
# Nspr_FOUND               Whether the module found or not
# Nspr_INCLUDE_DIRS        The location of Nspr headers
# Nspr_LIBRARY_DIRS        The location of Nspr library
# Nspr_LIBRARIES           The libraries

IF ( NOT NSPR_FIND_INCLUDE_PATH )
   LIST( APPEND NSPR_FIND_INCLUDE_PATH "/usr/include/nspr" "/usr/include/*" "/usr/local/include/*" )
ENDIF()

IF ( NOT NSPR_FIND_LIB_PATH )
   LIST( APPEND NSPR_FIND_LIB_PATH "/usr/lib" "/usr/local/lib/" "/usr/lib/x86_64-linux-gnu" )
ENDIF()

IF ( NOT NSPR_USE_DLL )
   SET( NSPR_USE_DLL FALSE )
ENDIF()

SET ( Nspr_LIB_PREFIX "lib" )
SET ( Nspr_LIBRARY_NAME "${Nspr_LIB_PREFIX}nspr" )

MESSAGE( STATUS "" )
MESSAGE( STATUS "" )

# find headers
FIND_PATH( Nspr_INCLUDE_DIRS nspr.h PATHS ${NSPR_FIND_INCLUDE_PATH} )

IF ( NSPR_USE_DLL )
   LIST( APPEND Nspr_LIBRARY_NAMES "${Nspr_LIBRARY_NAME}.so" )
ELSE()
   LIST( APPEND Nspr_LIBRARY_NAMES "${Nspr_LIBRARY_NAME}4.a" )
ENDIF()

# find libraries
FIND_LIBRARY( Nspr_LIBRARIES NAMES ${Nspr_LIBRARY_NAMES} PATHS ${NSPR_FIND_LIB_PATH} )
# find libraries directory
STRING( REGEX MATCH "${Nspr_LIBRARY_NAME}.*$" Nspr_LIB_SHOT_NAME ${Nspr_LIBRARIES} )
FIND_PATH( Nspr_LIBRARY_DIRS ${Nspr_LIB_SHOT_NAME} PATHS ${NSPR_FIND_LIB_PATH} )

IF( Nspr_INCLUDE_DIRS AND Nspr_LIBRARIES AND Nspr_LIBRARY_DIRS )
   SET( Nspr_FOUND TRUE )
ENDIF()

# print and report
IF ( Nspr_FOUND )
   IF ( NOT Nspr_FIND_QUIETLY )
      MESSAGE( STATUS "Found Nspr" )
      MESSAGE( STATUS "  Nspr include dir: ${Nspr_INCLUDE_DIRS}" )
      MESSAGE( STATUS "  Nspr library dir: ${Nspr_LIBRARY_DIRS}" )
      MESSAGE( STATUS "  Nspr libraries: ${Nspr_LIBRARIES}" )
   ENDIF()
ELSE()
   IF ( Nspr_FIND_REQUIRED )
      MESSAGE( FATAL_ERROR "Cound not find Nspr library" )
   ENDIF()
ENDIF()

