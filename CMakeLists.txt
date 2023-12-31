cmake_minimum_required (VERSION 3.12)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DAGL_DEBUG")
set(BUILD_SHARED_LIBS false)

option(AGL_BUILD_TESTS ON) # true for testing reasons

project(AGL)

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

target_include_directories(
	AGL_LIB
	PUBLIC "agl/include/"
)

if(AGL_BUILD_TESTS)
	enable_testing()
	
	include(FetchContent)
	FetchContent_Declare(
		googletest
		URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
	)
	# For Windows: Prevent overriding the parent project's compiler/linker settings
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)

	# AGL_LIB #

	file (
		GLOB_RECURSE 
		AGL_LIB_TEST_SOURCES #
		CONFIGURE_DEPENDS 
		"agl/test/*.cpp" 
	)

	add_executable(
		AGL_LOGGER_TEST
		${AGL_LOGGER_TEST_SOURCES}
	)

	target_link_libraries(
		AGL_LIB_TEST
		"agl/test/test.cpp"		
		PRIVATE AGL_LOGGER
		PRIVATE GTest::gtest_main
	)

	target_include_directories(
		AGL_LOGGER_TEST
		PRIVATE "logger/test/"
	)

	add_test(
		NAME AGL_LOGGER_TEST
		COMMAND $<TARGET_FILE:AGL_LOGGER_TEST>
	)

endif()

