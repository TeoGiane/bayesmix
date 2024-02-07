function(compile_protobuf_files) #FOLDER_PATH PROTO_HEADERS PROTO_SOURCES)
    # Parse input arguments
    set(oneValueArgs FOLDER HEADERS SOURCES)
    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Make custom command to compile each ProtoFile in FOLDER_PATH
    # message("FOLDER_PATH: ${arg_FOLDER}")
    file(GLOB ProtoFiles "${arg_FOLDER}/*.proto")
    set(PROTO_DIR proto)
    foreach(PROTO_FILE IN LISTS ProtoFiles)
    message(STATUS "protoc proto(cc): ${PROTO_FILE}")
    get_filename_component(PROTO_DIR ${PROTO_FILE} DIRECTORY)
    get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
    set(PROTO_HDR ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.pb.h)
    set(PROTO_SRC ${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.pb.cc)
    message(STATUS "protoc hdr: ${PROTO_HDR}")
    message(STATUS "protoc src: ${PROTO_SRC}")
    add_custom_command(
        OUTPUT ${PROTO_SRC} ${PROTO_HDR}
        COMMAND protobuf::protoc
        "--proto_path=${arg_FOLDER}"
        "--proto_path=${Protobuf_INCLUDE_DIRS}"
        "--cpp_out=${PROJECT_BINARY_DIR}"
        "--python_out=${BASEPATH}/python/bayesmixpy/proto"
        ${PROTO_FILE}
        DEPENDS ${PROTO_FILE} protobuf::protoc
        COMMENT "Generate C++ protocol buffer for ${PROTO_FILE}"
        VERBATIM)
    list(APPEND PROTO_HEADERS ${PROTO_HDR})
    list(APPEND PROTO_SOURCES ${PROTO_SRC})
    endforeach()
    SET_SOURCE_FILES_PROPERTIES(${PROTO_SRCS} ${PROTO_HDRS} PROPERTIES GENERATED TRUE)

    # Propagate PROTO_HDRS and PROTO_SRCS to parent scope
    set(${arg_HEADERS} ${PROTO_HEADERS} PARENT_SCOPE)
    # unset(PROTO_HEADERS)
    # message("PROTO_HDRS: ${PROTO_HDRS}")
    # message("arg_HEADERS: ${arg_HEADERS}")
    set(${arg_SOURCES} ${PROTO_SOURCES} PARENT_SCOPE)
    # unset(PROTO_HEADERS)
    # message("PROTO_SRCS: ${PROTO_SRCS}")
    # message("arg_SOURCES: ${arg_SOURCES}")
endfunction()
