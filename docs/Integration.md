# Integration

This library is a C++23 library, so don't forget to always enable it for your target using `set ( CMAKE_CXX_STANDARD 23 )`.

## CMake Package Manager

Assuming that you've enabled CPM in your CMake code, you can bring this library in like this:

```cmake
CPMAddPackage("gh:nerudaj/fsm-cpp#2.0.0")

set ( CMAKE_CXX_STANDARD 23 )

target_link_libraries ( ${MYTARGET} fsm-cpp )
```

Full example is available [here](../integration_tests/cpm).

## Fetch Content with Git

CPM is just a wrapper over native `FetchContent` call. So you can invoke it directly:

```cmake
include ( FetchContent )

FetchContent_Declare ( LIBFSM_GIT
	GIT_REPOSITORY "https://github.com/nerudaj/fsm-lib"
	GIT_TAG "v2.0.0"
)

FetchContent_MakeAvailable ( LIBFSM_GIT )

target_link_libraries ( ${MYTARGET} fsm-lib )
```

Full example is available [here](../integration_tests/fetch_git).

## Fetch Content with Releases

Who wants to waste time building their dependencies? You can use `FetchContent` to fetch published releases. It is a bit more work until I figure out how to properly integrate CMake's `find_package` scripts.

```cmake
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
```

Yep... I have to put it in some sort of a script. Anyway, full example is available [here](../integration_tests/fetch_release).
