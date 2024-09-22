cmake_minimum_required ( VERSION 3.26 )

include ( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/get_cpp_init.cmake" )

option ( BOOTSTRAP_CPM "Whether to download CPM" ON )
option ( BUILD_TESTS "Build unit testing target" ON )

if ( ${BOOTSTRAP_CPM} )
	bootstrap_cpm ()
endif ()

get_git_version (
	PROJECT_VERSION_VARIABLE GIT_PROJECT_VERSION
	FULL_VERSION_VARIABLE FULL_PROJECT_VERSION
)

project ( dgm-fsm-lib VERSION "${GIT_PROJECT_VERSION}" )

add_subdirectory ( "${PROJECT_SOURCE_DIR}/lib" )

if ( ${BUILD_TESTS} )
	enable_testing()
	add_subdirectory ( "${PROJECT_SOURCE_DIR}/tests" )
endif ()

# Packaging rules
install (
	FILES       "${PROJECT_SOURCE_DIR}/changelog.txt"
	DESTINATION "."
)

install (
	FILES       "${PROJECT_SOURCE_DIR}/LICENSE"
	DESTINATION "."
)

install (
	FILES       "${PROJECT_SOURCE_DIR}/Readme.md"
	DESTINATION "."
)

# Packaging
if ( ${PROJECT_IS_TOP_LEVEL} )
	set ( COMPILER_ID "${CMAKE_CXX_COMPILER_ID}")
	message ( "Computed compiler ID: ${COMPILER_ID}" )

	set ( CPACK_GENERATOR "ZIP" )
	set ( CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${FULL_PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${COMPILER_ID}-${CMAKE_GENERATOR_PLATFORM}" )

	include ( CPack )
endif()
