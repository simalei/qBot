cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(qBot VERSION 0.1.0)

file(GLOB_RECURSE SOURCES
	src/*.cpp
)

if(${CMAKE_SYSTEM} MATCHES "Darwin" OR ${CMAKE_SYSTEM} MATCHES "Linux")
	message(FATAL_ERROR "Unsupported operating system. Only Windows is supported right now.")
endif()


add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
	libraries/imgui-hook
	libraries/imgui
  	libraries/minhook/include
	libraries/gd.h
	libraries/gd.h/include
	libraries/imgui-notify/example/
	libraries/imgui-notify/example/src
	src/
)

add_subdirectory(libraries/minhook)
add_subdirectory(libraries/imgui-hook)
add_subdirectory(libraries/cocos-headers)
add_subdirectory(libraries/nfd-extended)
add_subdirectory(libraries/irrklang-headers)


target_link_libraries(${PROJECT_NAME}
	${CMAKE_SOURCE_DIR}/libraries/cocos-headers/cocos2dx/libcocos2d.lib
	imgui-hook
	minhook
	opengl32
	cocos2d
	nfd
	irrKlang
)