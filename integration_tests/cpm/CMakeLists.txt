cmake_minimum_required ( VERSION 3.26 )

include ( get_cpm.cmake )

set ( CMAKE_CXX_STANDARD 23 )

project ( test-cpm )

CPMAddPackage("gh:nerudaj/dgm-fsm-lib#v2")

add_executable ( ${PROJECT_NAME} Main.cpp )

target_link_libraries ( ${PROJECT_NAME} libfsm )
