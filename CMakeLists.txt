
cmake_minimum_required (VERSION 3.8)
project ("Paint")	

if(UNIX)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/Linux-Debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/Linux-Release)
else(UNIX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/Win-Debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/Win-Release)


endif(UNIX)


if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Ox ")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Ox ")

  # Enable link-time optimization (LTO)
endif()

add_executable (Paint "src/Main.cpp"  "libs/glad/glad.c" "src/opengl/Shaders.cpp" "src/opengl/Window.cpp" "src/opengl/Shaders.cpp" "src/opengl/Buffers.cpp" "src/BitBoard.cpp" "src/BitMasks.cpp" "src/ChessEngine.cpp" "src/Application.cpp" "src/UCIconnect.cpp" "headers/UCIconnect.h")

target_include_directories(Paint PRIVATE "libs/include" "headers")

if(UNIX)
    target_link_libraries(Paint glfw)
else(UNIX)
    target_link_libraries(Paint PRIVATE "../../libs/glfw/glfw3")
endif(UNIX)

