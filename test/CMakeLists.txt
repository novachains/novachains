# set executable output directory #set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${NOVA_OUTPUT_DIR}/bin" )
set ( EXECUTABLE_OUTPUT_PATH "${NOVA_OUTPUT_DIR}/test" )
# set library output directory #set ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${NOVA_OUTPUT_DIR}/lib" )
set ( LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}" )

# auto add sub directories
# if you won't add some directory, put it into the file excludedirs.txt with new line
AUTOADDSUBDIR ( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )
