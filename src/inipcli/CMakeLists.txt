aux_source_directory(. CLI_SRC)

add_executable(${CLI_NAME} ${CLI_SRC})
target_link_libraries(${CLI_NAME}
	PRIVATE
	${ORIGINAL_STATIC_LIB_NAME}
	CLI11::CLI11
)

target_include_directories(${CLI_NAME}
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/3rdlib/CLI11/include
	${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties(${CLI_NAME} PROPERTIES
	OUTPUT_NAME ${LIB_NAME}
    POSITION_INDEPENDENT_CODE ON
)

install(
	TARGETS ${CLI_NAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

configure_file(
	cliconfig.h.in
	${CMAKE_CURRENT_BINARY_DIR}/cliconfig.h
	@ONLY
)

