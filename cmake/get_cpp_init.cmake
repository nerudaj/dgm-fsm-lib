function ( fetch_cpp_init )
    include ( FetchContent )

    FetchContent_Declare (
        cppinit
        GIT_REPOSITORY https://github.com/nerudaj/clang-format
        GIT_TAG "feature-cmake-scripts"
    )

    FetchContent_MakeAvailable ( cppinit )

    set ( CPPINIT_FOLDER "${cppinit_SOURCE_DIR}" PARENT_SCOPE )
endfunction ()

fetch_cpp_init ()

include ( "${CPPINIT_FOLDER}/cmake/bootstrap.cmake" )
include ( "${CPPINIT_FOLDER}/cmake/cpp.cmake" )
