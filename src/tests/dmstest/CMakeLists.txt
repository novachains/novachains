
#add whole lib
NOVA_LINK_WHOLE_LIBRARIES( pmd dao )

AUX_SOURCE_DIRECTORY( . SRC_FILES )
ADD_EXECUTABLE( dmstest ${SRC_FILES} )

NOVA_TARGET_LINK_WHOLE_LIBRARIES( dmstest bson pd oss util msg net dms )
