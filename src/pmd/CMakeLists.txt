INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )
AUX_SOURCE_DIRECTORY( . SRC_FILES )
ADD_LIBRARY( pmd STATIC ${SRC_FILES} )
