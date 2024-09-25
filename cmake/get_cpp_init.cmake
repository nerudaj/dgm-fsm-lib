function ( fetch_cpp_init )
    include ( FetchContent )

    FetchContent_Declare (
        cpp-init
        GIT_REPOSITORY https://github.com/nerudaj/cpp-init
        GIT_TAG "v0.1.1"
    )

    FetchContent_MakeAvailable ( cpp-init )

    set ( CPPINIT_FOLDER "${cpp-init_SOURCE_DIR}" PARENT_SCOPE )
endfunction ()

fetch_cpp_init ()

include ( "${CPPINIT_FOLDER}/cmake/bootstrap.cmake" )
include ( "${CPPINIT_FOLDER}/cmake/cpp.cmake" )
