cmake_minimum_required ( VERSION 3.19 )

project ( libfsm )

include_directories ( "${JSON_FOLDER}/include" )

# Automatically include new source files
file (
	GLOB LIB_HEADERS
	CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

file (
	GLOB LIB_SOURCES
	CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Add source files to source group so MSVC organizes them into filters
source_group (
	TREE
	"${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		${LIB_HEADERS}
		${LIB_SOURCES}
)

# Setup build target
add_library ( ${PROJECT_NAME} STATIC
	${LIB_HEADERS} ${LIB_SOURCES}
	"${CMAKE_CURRENT_SOURCE_DIR}/.editorconfig"
)

target_include_directories ( ${PROJECT_NAME}
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_features( ${PROJECT_NAME}
	INTERFACE ${MY_CXX_COMPILE_FEATURES}
)

target_compile_options ( ${PROJECT_NAME}
	PRIVATE ${CXX_COMPILE_OPTIONS}
)

add_subdirectory ( tests )

# Packaging rules
set ( DEBUG_BINARY_DIR   "${CMAKE_CURRENT_BINARY_DIR}/Debug" )
set ( RELEASE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/Release" )

install (
	DIRECTORY    "${CMAKE_CURRENT_SOURCE_DIR}/include"
	DESTINATION  "."
)

install (
	FILES        "${RELEASE_BINARY_DIR}/libfsm.lib"
	DESTINATION  "lib"
)

install (
	FILES        "${DEBUG_BINARY_DIR}/libfsm-d.lib"
	DESTINATION  "lib"
)

install (
	FILES        "${DEBUG_BINARY_DIR}/libfsm-d.pdb"
	DESTINATION  "lib"
)
