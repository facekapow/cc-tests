project(cc-tests)
cmake_minimum_required(VERSION 3.13)

option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)

if(${FORCE_COLORED_OUTPUT})
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		add_compile_options(-fdiagnostics-color=always)
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		add_compile_options(-fcolor-diagnostics)
	endif()
endif()

add_executable(cctest
	"${PROJECT_SOURCE_DIR}/src/cc.c"
	"${PROJECT_SOURCE_DIR}/src/ccn.c"
	"${PROJECT_SOURCE_DIR}/src/ccrsa.c"
	"${PROJECT_SOURCE_DIR}/src/cczp.c"
	"${PROJECT_SOURCE_DIR}/src/ccder.c"
	"${PROJECT_SOURCE_DIR}/src/ccdigest.c"
	"${PROJECT_SOURCE_DIR}/src/ccmd2.c"
	"${PROJECT_SOURCE_DIR}/src/ccmd4.c"
	"${PROJECT_SOURCE_DIR}/src/ccmd5.c"
	"${PROJECT_SOURCE_DIR}/src/ccripemd.c"
	"${PROJECT_SOURCE_DIR}/src/ccsha1.c"
	"${PROJECT_SOURCE_DIR}/src/ccsha2.c"
	"${PROJECT_SOURCE_DIR}/src/ccsha3.c"
)

target_include_directories(cctest PRIVATE "${PROJECT_SOURCE_DIR}/include")

set_target_properties(cctest PROPERTIES
	C_STANDARD 99
	C_STANDARD_REQUIRED TRUE
	C_EXTENSIONS OFF
)
