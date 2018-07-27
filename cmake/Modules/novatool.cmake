
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
