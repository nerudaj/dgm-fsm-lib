cmake_minimum_required ( VERSION 3.19 )

# Project and compilation rules
include ( "${CMAKE_SOURCE_DIR}/cmake/cpp.cmake" )

project ( dgm-fsm-lib )
include ( "${CMAKE_SOURCE_DIR}/cmake/version.cmake" )
enable_testing()

include ( "${CMAKE_SOURCE_DIR}/cmake/dependencies.cmake" )

add_subdirectory ( lib )

# Packaging rules
install (
	FILES       "${CMAKE_SOURCE_DIR}/changelog.txt"
	DESTINATION "."
)

install (
	FILES       "${CMAKE_SOURCE_DIR}/LICENSE"
	DESTINATION "."
)

install (
	FILES       "${CMAKE_SOURCE_DIR}/Readme.md"
	DESTINATION "."
)

# Enable packaging
set ( CPACK_GENERATOR "ZIP" )
set ( CPACK_PACKAGE_VERSION_MAJOR       "${PROJECT_VERSION_MAJOR}" )
set ( CPACK_PACKAGE_VERSION_MINOR       "${PROJECT_VERSION_MINOR}" )
set ( CPACK_PACKAGE_VERSION_PATCH       "${PROJECT_VERSION_PATCH}" )
set ( CPACK_PACKAGE_FILE_NAME           "${PROJECT_NAME}-${PROJECT_VERSION}-windows-vc17-${CMAKE_GENERATOR_PLATFORM}" )

include ( CPack )
