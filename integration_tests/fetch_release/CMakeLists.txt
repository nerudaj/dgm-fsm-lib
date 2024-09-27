cmake_minimum_required ( VERSION 3.26 )

set ( CMAKE_CXX_STANDARD 23 )

# First, download and unpack the release using FetchContent
include ( FetchContent )

FetchContent_Declare ( LIBFSM
	URL "https://github.com/nerudaj/fsm-lib/releases/download/v2.0.0/fsm-lib-v2.0.0-Windows-MSVC-x64.zip"
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable ( LIBFSM )

# Then find the static libraries
find_library(LIBFSM_D fsm-lib-d NAMES fsm-lib-d.lib HINTS "${libfsm_SOURCE_DIR}/lib")
if ( "${LIBFSM_D}" EQUAL "LIB_DGM_FSM_D-NOTFOUND" )
	message ( FATAL_ERROR "Cannot find libfsm-d.lib" )
endif()

find_library(LIBFSM_R fsm-lib NAMES fsm-lib.lib HINTS "${libfsm_SOURCE_DIR}/lib")
if ( "${LIBFSM_R}" EQUAL "LIB_DGM_FSM_R-NOTFOUND" )
	message ( FATAL_ERROR "Cannot find libfsm.lib" )
endif()

# And made a meta-target for both debug and release configurations
set( LIBFSM_DR optimized ${LIBFSM_R} debug ${LIBFSM_D} )

# And create a linkable CMake target
add_library ( fsm-lib INTERFACE )
target_include_directories ( fsm-lib INTERFACE "${libfsm_SOURCE_DIR}/include" )
target_link_libraries ( fsm-lib INTERFACE ${LIBFSM_DR} )

project ( test-cpm )

add_executable ( ${PROJECT_NAME} Main.cpp )

target_link_libraries ( ${PROJECT_NAME} fsm-lib )
