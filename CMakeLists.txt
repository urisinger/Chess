
cmake_minimum_required (VERSION 3.8)
project ("Paint")	

if(UNIX)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/Linux-Debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/Linux-Release)
else(UNIX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/Win-Debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/Win-Release)

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

endif(UNIX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ofast ")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast ")

  # Enable link-time optimization (LTO)
endif()

add_executable (Paint "src/Main.cpp"  "libs/glad/glad.c" "src/opengl/Shaders.cpp" "src/opengl/Window.cpp" "src/opengl/Shaders.cpp" "src/opengl/Buffers.cpp" "src/BitBoard.cpp" "src/BitMasks.cpp" "src/ChessEngine.cpp" "src/Application.cpp")

target_include_directories(Paint PRIVATE "libs/include" "headers")

if(UNIX)
    target_link_libraries(Paint glfw)
else(UNIX)
    target_link_libraries(Paint PRIVATE "../../libs/glfw/glfw3")
endif(UNIX)


if( supported )
    message(STATUS "IPO / LTO enabled")
    set_property(TARGET example PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()