cmake_minimum_required(VERSION 3.16.0)

project(examples_bayesmix)

compile_protobuf_files(
  FOLDER "${CMAKE_CURRENT_LIST_DIR}/gamma_hierarchy/proto"
  HEADERS gamma_PROTO_HDRS
  SOURCES gamma_PROTO_SRCS
)

add_executable(run_gamma $<TARGET_OBJECTS:bayesmix>
    ${gamma_PROTO_HDRS} ${gamma_PROTO_SRCS}
    gamma_hierarchy/run_gamma_gamma.cc
    gamma_hierarchy/gammagamma_hierarchy.h
    gamma_hierarchy/gamma_likelihood.h
    gamma_hierarchy/gamma_prior_model.h
    gamma_hierarchy/gammagamma_updater.h
    gamma_hierarchy/gamma_utils.h
)

target_include_directories(run_gamma PUBLIC ${INCLUDE_PATHS})
target_link_libraries(run_gamma PUBLIC ${LINK_LIBRARIES})
target_compile_options(run_gamma PUBLIC ${COMPILE_OPTIONS})
