cmake_minimum_required (VERSION 3.12)

# VS CMake debugging 
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DAGL_DEBUG")

option(AGL_BUILD_TESTS OFF) 
option(AGL_BUILD_EDITOR OFF)
#set(AGL_BUILD_TESTS ON CACHE BOOL "" FORCE) # for dev reasons TODO: hash out on realease build


# AGL
# ---
project(AGL)

# DEPENDENCIES
# ------------

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

# OpenGL
# ------
find_package(OpenGL 4.2 REQUIRED)

# GLFW
# -----
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        7b6aead9fb88b3623e3b3725ebb42670cbe4c579
)
FetchContent_GetProperties(glfw)

if(NOT glfw_POPULATED)
	FetchContent_Populate(glfw)
	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
endif()

# AGL_LIB
# -------
file (
	GLOB_RECURSE AGL_LIB_SOURCES
	LIST_DIRECTORIES FALSE
	"agl/external/*.c"
	"agl/external/*.h"
	"agl/external/*.hpp"
	"agl/include/*.hpp" 
	"agl/include/*.inl"
	"agl/src/*.cpp"
	"debug/*.natvis"
)

add_library(
	AGL_LIB
	${AGL_LIB_SOURCES}
)

add_dependencies(
	AGL_LIB
	glfw
	glm
	OpenGL::GL
)

target_include_directories(
	AGL_LIB
	PUBLIC "agl/external/"
	PUBLIC "agl/include/"
    PUBLIC "${freetype_SOURCE_DIR}/include/"
    PUBLIC "${glfw_SOURCE_DIR}/include/"
	PUBLIC ${glm_SOURCE_DIR}
)

target_link_libraries(
	AGL_LIB
	PUBLIC glfw
	PUBLIC glm
	PUBLIC OpenGL::GL
)	

foreach(_source IN ITEMS ${AGL_LIB_SOURCES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${CMAKE_CURRENT_SOURCE_DIR}/" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

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
		GLOB_RECURSE AGL_LIB_TEST_SOURCES
		LIST_DIRECTORIES FALSE 
		"agl/test/*.hpp" 
		"agl/test/*.inl"
		"agl/test/*.cpp"
		"main/include/*.inl"
		"main/include/*.hpp" 
		"main/src/*.cpp"
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

	foreach(_source IN ITEMS ${AGL_LIB_TEST_SOURCES})
		get_filename_component(_source_path "${_source}" PATH)
		file(RELATIVE_PATH _source_path_rel "${CMAKE_CURRENT_SOURCE_DIR}/" "${_source_path}")
		string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
		source_group("${_group_path}" FILES "${_source}")
	endforeach()
endif() # TESTS

# EDITOR
# ------
if(AGL_BUILD_EDITOR)
	file (
		GLOB_RECURSE AGL_EDITOR_SOURCES
		LIST_DIRECTORIES FALSE 
		"editor/include/*.inl"
		"editor/include/*.hpp" 
		"editor/src/*.cpp"
	)

	add_library(
		AGL_EDITOR
		${AGL_EDITOR_SOURCES}
	)

	add_dependencies(
		AGL_EDITOR
		AGL_LIB
	)

	target_include_directories(
		AGL_EDITOR
		PUBLIC "editor/include/"
	)

	target_link_libraries(
		AGL_EDITOR
		PUBLIC AGL_LIB
	)

	foreach(_source IN ITEMS ${AGL_EDITOR_SOURCES})
		get_filename_component(_source_path "${_source}" PATH)
		file(RELATIVE_PATH _source_path_rel "${CMAKE_CURRENT_SOURCE_DIR}/" "${_source_path}")
		string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
		source_group("${_group_path}" FILES "${_source}")
	endforeach()

	# EDITOR MAIN
	file (
		GLOB_RECURSE AGL_EDITOR_MAIN_SOURCES
		LIST_DIRECTORIES FALSE 
		"main/include/*.inl"
		"main/include/*.hpp" 
		"main/src/*.cpp"
	)

	add_executable(
		AGL_EDITOR_MAIN
		${AGL_EDITOR_MAIN_SOURCES}
	)

	target_link_libraries(
		AGL_EDITOR_MAIN
		PRIVATE AGL_EDITOR
	)

	add_dependencies(
		AGL_EDITOR_MAIN
		AGL_EDITOR
	)
	
	foreach(_source IN ITEMS ${AGL_EDITOR_MAIN_SOURCES})
		get_filename_component(_source_path "${_source}" PATH)
		file(RELATIVE_PATH _source_path_rel "${CMAKE_CURRENT_SOURCE_DIR}/" "${_source_path}")
		string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
		source_group("${_group_path}" FILES "${_source}")
	endforeach()
endif() # AGL_BUILD_EDITOR
