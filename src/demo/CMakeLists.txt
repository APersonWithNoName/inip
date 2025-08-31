aux_source_directory(. DEMO_SRC)

add_executable(${DEMO_NAME} ${DEMO_SRC})
target_link_libraries(${DEMO_NAME} ${ORIGINAL_STATIC_LIB_NAME})
set_target_properties(${DEMO_NAME} PROPERTIES
	OUTPUT_NAME ${LIB_NAME}
    POSITION_INDEPENDENT_CODE ON
)

install(
	TARGETS ${DEMO_NAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
