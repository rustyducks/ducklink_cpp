include(FindProtobuf)
find_package(Protobuf 3.1.0)
if (NOT ${Protobuf_FOUND})
   message("Protobuf package not found, trying without package")
   set(Protobuf_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)
   set(Protobuf_LIBRARIES ${CMAKE_INSTALL_PREFIX}/lib/libprotobuf.a)
   set(Protobuf_PROTOC_EXECUTABLE ${CMAKE_INSTALL_PREFIX}/bin/protoc)
endif()
file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/protoduck PROTOMODEL_PATH)
file(TO_NATIVE_PATH ${CMAKE_SOURCE_DIR}/include/generated PROTOBINDING_PATH)
file(MAKE_DIRECTORY ${PROTOBINDING_PATH})

file(GLOB DATAMODEL_PROTOS "${CMAKE_CURRENT_SOURCE_DIR}/protoduck/*.proto")
foreach(proto ${DATAMODEL_PROTOS})
   file(TO_NATIVE_PATH ${proto} proto_native)
   execute_process(COMMAND ${Protobuf_PROTOC_EXECUTABLE} --proto_path=${PROTOMODEL_PATH} --cpp_out=${PROTOBINDING_PATH} ${proto_native}
   RESULT_VARIABLE rv)
   # Optional, but that can show the user if something have gone wrong with the proto generation 
   if(${rv})
        message("Generation of data model returned ${rv} for proto ${proto_native}")
   endif()
endforeach(proto)
aux_source_directory(${PROTOBINDING_PATH} Protoduck_SRC_FILES)
add_library(Protoduck STATIC ${Protoduck_SRC_FILES})
set_target_properties(Protoduck PROPERTIES COMPILE_FLAGS "-Wall -Wextra -pedantic")
target_include_directories(Protoduck PUBLIC ${Protobuf_INCLUDE_DIRS})
target_link_libraries(Protoduck ${Protobuf_LIBRARIES})
set_target_properties(Protoduck PROPERTIES PUBLIC_HEADER "${PROTOBINDING_PATH}/messages.pb.h")
#message("Protobuf: ${Protobuf_LIBRARIES}   ${Protobuf_INCLUDE_DIRS}  ${CMAKE_INSTALL_PREFIX}")
install(TARGETS Protoduck
        EXPORT ProtoduckTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION include
        PUBLIC_HEADER DESTINATION include/Ducklink
)
install(EXPORT ProtoduckTargets DESTINATION lib/cmake/Protoduck)
install(FILES ${CMAKE_SOURCE_DIR}/cmake/ProtoduckConfig.cmake DESTINATION lib/cmake/Protoduck)

set(Serialib_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/serialib/lib/serialib.cpp" PARENT_SCOPE)
set(Serialib_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/serialib/lib" PARENT_SCOPE)