cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

project(qBot VERSION 0.1.0)

file(GLOB_RECURSE SOURCES
	src/*.cpp
)

if(${CMAKE_SYSTEM} MATCHES "Darwin" OR ${CMAKE_SYSTEM} MATCHES "Linux")
	message(FATAL_ERROR "Unsupported operating system. Only Windows is supported right now.")
endif()

add_library(${PROJECT_NAME} SHARED ${SOURCES})

add_definitions(-DCURL_STATICLIB)

target_include_directories(${PROJECT_NAME} PRIVATE
		libraries/imgui-hook
		libraries/imgui
		libraries/minhook/include
		libraries/gd.h
		libraries/gd.h/include
		libraries/imgui-notify/example/
		libraries/imgui-notify/example/src
		libraries/7zip-cpp/Include/7zpp
		libraries/rapidjson/include
		src/
)

add_subdirectory(libraries/minhook)
add_subdirectory(libraries/imgui-hook)
add_subdirectory(libraries/cocos-headers)
add_subdirectory(libraries/nfd-extended)
add_subdirectory(libraries/7zip-cpp)
add_subdirectory(libraries/curl)
add_subdirectory(libraries/rapidjson)
add_subdirectory(libraries/ClipboardXX)

target_link_libraries(${PROJECT_NAME}
	imgui-hook
	minhook
	opengl32
	cocos2d
	nfd
	7zpp
	libcurl
	ClipboardXX
	${CMAKE_SOURCE_DIR}/libraries/glew32.lib
	${CMAKE_SOURCE_DIR}/libraries/cocos-headers/cocos2dx/libcocos2d.lib
)