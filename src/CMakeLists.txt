find_package(
  Boost 1.74
  COMPONENTS headers program_options system
  REQUIRED)
find_package(spdlog REQUIRED)
find_package(Protobuf REQUIRED)

# The function protobuf_generate_cpp fails due to protobuf::protoc not being set correctly
# Workaround with a custom command!
add_custom_command(
  OUTPUT nar-messages.pb.cc include/message-definitions/nar-messages.pb.h
  COMMAND ${Protobuf_PROTOC_EXECUTABLE} --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
          -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/nar-messages.proto
	  )
add_library(nar-messages nar-messages.pb.cc)
target_link_libraries(nar-messages protobuf::libprotobuf)
target_include_directories(
  nar-messages
  PUBLIC $<INSTALL_INTERFACE:include> $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  INTERFACE $<INSTALL_INTERFACE:include> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(nar-server
	SettingsDescription.cpp
	nar-server.cpp
	WsAcceptor.cpp
	WsConnection.cpp
)

target_link_libraries(
	nar-server PRIVATE
	Boost::boost
	Boost::program_options
	Boost::system
	${CMAKE_THREAD_LIBS_INIT}
	nar-messages
	spdlog::spdlog
)

target_include_directories(nar-server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS nar-server DESTINATION bin)
