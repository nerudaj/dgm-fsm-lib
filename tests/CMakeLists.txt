cmake_minimum_required ( VERSION 3.26 )

set ( TARGET tests )

bootstrap_clang_format()
glob_headers_and_sources ( HEADERS SOURCES )

add_executable ( ${TARGET}
	${HEADERS} ${SOURCES}
	"${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
)

target_include_directories ( ${TARGET}
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries ( ${TARGET}
	fsm-lib
)

apply_compile_options ( ${TARGET} )

add_test (
	NAME ${TARGET}
	COMMAND ${TARGET}
)
