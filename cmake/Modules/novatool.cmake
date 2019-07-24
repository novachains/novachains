
# define macro: get sub directories list
MACRO( SUBDIRLIST result curdir )
   FILE ( GLOB children RELATIVE ${curdir} ${curdir}/* )
   SET ( dirlist "" )
   FOREACH ( child ${children} )
      IF ( IS_DIRECTORY ${curdir}/${child} )
         LIST ( APPEND dirlist ${child} )
      ENDIF()
   ENDFOREACH()
   SET ( ${result} ${dirlist} )
ENDMACRO()

# define macro: add sub directory by CMakeList
MACRO( AUTOADDSUBDIR curdir bindir )
	# set excludes
	SET ( EXCLUDEFILE "${curdir}/excludedirs.txt" )
	SET ( CMAKELISTFILE "CMakeLists.txt" )
	SET ( EXCLUDEDIRS "" )

	IF( EXISTS ${EXCLUDEFILE} )
	   FILE ( STRINGS ${EXCLUDEFILE} EXCLUDEDIRS )
	   MESSAGE( STATUS "Exclude context is: ${EXCLUDEDIRS}" )
	ENDIF()

	# list current directory subs
	SUBDIRLIST( SUBDIRS ${curdir} )

	# find CMakeLists and ADD_SUBDIRECTORY
	FOREACH ( subdir ${SUBDIRS} )
	   SET ( USEDIR 1 )
	   IF ( EXISTS ${curdir}/${subdir}/${CMAKELISTFILE} )
		  FOREACH( exdir ${EXCLUDEDIRS} )
			 IF( ${subdir} STREQUAL ${exdir} )
				SET ( USEDIR 0 )
                BREAK()
			 ENDIF()
		  ENDFOREACH()

		  IF( USEDIR )
			 MESSAGE ( STATUS "Found sub directory: ${subdir}" )
			 ADD_SUBDIRECTORY( ${subdir} ${bindir}/${subdir} )
		  ENDIF()
	   ENDIF ()
	ENDFOREACH()
ENDMACRO()

# define macro: get version
MACRO( SUBVERSION rootdir version )
   IF( EXISTS "${rootdir}/.svn/" )
      FIND_PACKAGE( Subversion 1.4 )
      IF( Subverion_FOUND )
         Subverion_WC_INFO( ${rootdir} SVNVERION )
         MESSAGE( STATUS "Current SVN Version is: ${SVNVERION}" )
         SET( ${version} ${SVNVERION} )
      ENDIF()
   ENDIF()
ENDMACRO()

# define macro: NOVA_TARGET_LINK_WHOLE_LIBRARIES
MACRO( NOVA_TARGET_LINK_WHOLE_LIBRARIES dest )
   IF ( WIN32 )
      TARGET_LINK_LIBRARIES( ${dest} ${ARGN} )
   ELSEIF( CMAKE_SYSTEM_NAME MATCHES "Darwin" )
      TARGET_LINK_LIBRARIES( ${dest} -Wl,-all_load ${ARGN} )
   ELSE()
      TARGET_LINK_LIBRARIES( ${dest} -Wl,--start-group -Wl,--whole-archive ${ARGN} -Wl,--no-whole-archive -Wl,--end-group )
   ENDIF()
ENDMACRO()

# define macro: NOVA_LINK_WHOLE_LIBRARIES
MACRO( NOVA_LINK_WHOLE_LIBRARIES )
   IF ( WIN32 )
      LINK_LIBRARIES( ${ARGN} )
   ELSEIF( CMAKE_SYSTEM_NAME MATCHES "Darwin" )
      LINK_LIBRARIES( -Wl,-all_load ${ARGN} )
   ELSE()
      LINK_LIBRARIES( -Wl,--start-group -Wl,--whole-archive ${ARGN} -Wl,--no-whole-archive -Wl,--end-group )
   ENDIF()
ENDMACRO()
