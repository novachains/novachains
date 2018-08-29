ADD_DEFINITIONS( -DSDB_SHELL )

# add spt use whole mode
NOVA_LINK_WHOLE_LIBRARIES( spt )

ADD_EXECUTABLE( nova nova.cpp )
ADD_EXECUTABLE( novabp novabp.cpp )

TARGET_LINK_LIBRARIES( nova ssh2 bson pd util oss mdocml )
TARGET_LINK_LIBRARIES( novabp ssh2 bson pd util oss mdocml )
