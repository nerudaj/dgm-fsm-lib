cmake_minimum_required ( VERSION 3.26 )

set ( TARGET "01-loggable-blackboard" )

bootstrap_clang_format()

add_executable ( ${TARGET}
	"${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
)

target_link_libraries ( ${TARGET} libfsm examples-common )

apply_compile_options ( ${TARGET} )

set_target_properties( ${TARGET} PROPERTIES FOLDER "examples" )
