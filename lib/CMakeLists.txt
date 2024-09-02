cmake_minimum_required ( VERSION 3.19 )

set ( TARGET libfsm )

bootstrap_clang_format()
glob_headers_and_sources ( HEADERS SOURCES )

# Setup build target
add_library ( ${TARGET} STATIC
	${HEADERS} ${SOURCES}
	"${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
)

target_include_directories ( ${TARGET}
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

#target_link_libraries ( ${TARGET}
#	LINK_PRIVATE nlohmann_json::nlohmann_json
#)

target_compile_features( ${TARGET}
	INTERFACE ${CXX_COMPILE_FEATURES}
)

target_compile_options ( ${TARGET}
	PRIVATE ${CXX_COMPILE_OPTIONS}
)

# Packaging rules
set ( DEBUG_BINARY_DIR   "${CMAKE_CURRENT_BINARY_DIR}/Debug" )
set ( RELEASE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/Release" )

install (
	DIRECTORY    "${CMAKE_CURRENT_SOURCE_DIR}/include"
	DESTINATION  "."
)

install (
	FILES        "${RELEASE_BINARY_DIR}/${TARGET}.lib"
	DESTINATION  "lib"
)

install (
	FILES        "${DEBUG_BINARY_DIR}/${TARGET}-d.lib"
	DESTINATION  "lib"
)

install (
	FILES        "${DEBUG_BINARY_DIR}/${TARGET}-d.pdb"
	DESTINATION  "lib"
)
