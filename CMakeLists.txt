cmake_minimum_required (VERSION 3.12)

# VS CMake debugging 
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DAGL_DEBUG")
set(BUILD_SHARED_LIBS OFF)

option(AGL_BUILD_TESTS OFF)
set(AGL_BUILD_TESTS ON CACHE BOOL "" FORCE) # for dev reasons TODO: hash out on realease build

# AGL
# ---
project(AGL)

include(FetchContent)

# GLM
# ---
FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG		   0.9.9.2
)

FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
	FetchContent_Populate(glm)
	set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
	add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
endif()

FetchContent_MakeAvailable(glm)

# AGL_LIB
# -------
file (
	GLOB_RECURSE 
	AGL_LIB_SOURCES #
	CONFIGURE_DEPENDS 
	"agl/src/*.cpp"
)

file (
	GLOB_RECURSE 
	AGL_LIB_HEADERS #
	CONFIGURE_DEPENDS 
	"agl/include/*.hpp" 
	"agl/include/*.inl"
)

add_library(
	AGL_LIB 
	${AGL_LIB_SOURCES}
	${AGL_LIB_HEADERS}
)

add_dependencies(
	AGL_LIB
	glm
)

target_include_directories(
	AGL_LIB
	PUBLIC "agl/include/"
	PUBLIC ${glm_SOURCE_DIR}
)

target_link_libraries(
	AGL_LIB
	PUBLIC glm
)	

# AGL MAIN
# --------
file (
	GLOB_RECURSE 
	AGL_MAIN_SOURCES #
	CONFIGURE_DEPENDS 
	"main/src/*.cpp"
)

file (
	GLOB_RECURSE 
	AGL_MAIN_HEADERS #
	CONFIGURE_DEPENDS 
	"main/include/*.hpp" 
	"main/include/*.inl"
)

add_executable(
	AGL_MAIN
	${AGL_MAIN_HEADERS}
	${AGL_MAIN_SOURCES}
)

target_link_libraries(
	AGL_MAIN
	PRIVATE AGL_LIB
)

# TESTS
# -----
if(AGL_BUILD_TESTS)
	enable_testing()
	
	FetchContent_Declare(
		googletest
		URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
	)
	# For Windows: Prevent overriding the parent project's compiler/linker settings
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)

	file (
		GLOB_RECURSE 
		AGL_LIB_TEST_SOURCES #
		CONFIGURE_DEPENDS 
		"agl/test/*.hpp" 
		"agl/test/*.inl"
		"agl/test/*.cpp"
	)

	add_executable(
		AGL_LIB_TEST
		${AGL_LIB_TEST_SOURCES}
	)

	target_link_libraries(
		AGL_LIB_TEST
		PRIVATE AGL_LIB
		PRIVATE GTest::gtest_main
	)

	add_test(
		NAME AGL_LIB_TEST
		COMMAND $<TARGET_FILE:AGL_LIB_TEST>
	)

endif()

add_compile_options("-g3")
