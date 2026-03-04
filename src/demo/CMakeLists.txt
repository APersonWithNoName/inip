aux_source_directory(. DEMO_SRC)

add_executable(${DEMO_NAME} ${DEMO_SRC})
target_link_libraries(${DEMO_NAME}
	PRIVATE
	${ORIGINAL_STATIC_LIB_NAME}
	CLI11::CLI11
	replxx
)

set_target_properties(${DEMO_NAME} PROPERTIES
	OUTPUT_NAME ${LIB_NAME}
    POSITION_INDEPENDENT_CODE ON
)

install(
	TARGETS ${DEMO_NAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 确保 replxx 的头文件不会被安装
set_target_properties(replxx PROPERTIES
    EXCLUDE_FROM_ALL TRUE
    EXCLUDE_FROM_DEFAULT_BUILD TRUE
)

# 如果 replxx 有安装目标，明确禁用它
if(TARGET replxx_install)
    set_target_properties(replxx_install PROPERTIES
        EXCLUDE_FROM_ALL TRUE
    )
endif()
