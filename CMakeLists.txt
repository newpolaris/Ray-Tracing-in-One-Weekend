# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (RayTracer)

set(gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll")
set(APP_TARGET RayTracer.app)
set(UNITTEST_TARGET UnitTest.app)

#if( APPLE )
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
#endif()

find_package(OpenGL REQUIRED)
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else(OPENMP_FOUND)
	if(DEFINED OpenMP_C_FLAGS AND APPLE)
		set(libomp omp)
	endif()
endif(OPENMP_FOUND)

set(UseGLI TRUE)
set(UseZlib TRUE)
set(UseGTest TRUE)

# Compile external dependencies 
add_subdirectory(external)

# On Visual 2005 and above, this module can set the debug working directory
cmake_policy(SET CMP0026 OLD)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-fe2273")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)

include_directories(
	src/
	external/
	external/AntTweakBar-1.16/include/
	external/glfw-3.1.2/include/GLFW/
	external/glm/
	external/glew-1.13.0/include/
	external/assimp-3.0.1270/include/
	external/bullet-2.81-rev2613/src/
	external/gli/
	external/zlib/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
	imgui
	gli
	zlibstatic
	${libomp}
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

file(GLOB GLSW external/glsw/*.c external/glsw/*.h)
add_library(glsw ${GLSW})

file(GLOB_RECURSE SRC src/*)

set(UNITTEST_SRC 
	test/aabb.cpp
	test/string.cpp
	test/random.cpp
	src/Math/AABB.cpp
	src/Math/Ray.cpp
	src/Math/Random.cpp
)

add_executable(${APP_TARGET} ${SRC})
target_link_libraries(${APP_TARGET} glsw ${ALL_LIBS})

add_executable(${UNITTEST_TARGET} ${UNITTEST_SRC})
target_link_libraries(${UNITTEST_TARGET} ${ALL_LIBS} gtest gtest_main)
add_test(${UNITTEST_TARGET} ${UNITTEST_TARGET})

# Xcode and Visual working directories
set_target_properties(${APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
create_target_launcher(${APP_TARGET} WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")

set_target_properties(${UNITTEST_TARGET} PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
create_target_launcher(${UNITTEST_TARGET} WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")
