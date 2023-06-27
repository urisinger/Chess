cmake_minimum_required (VERSION 3.8)
project ("GrandChess")	

if(UNIX)
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -m64 -mbmi")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/Linux-Debug)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/Linux-Release)
else(UNIX)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/x64-Debug)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/x64-Release)


endif(UNIX)


if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(UNIX)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ofast ")
        set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Ofast ")
    else(UNIX)
        set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} /Ox ")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Ox ")  
    endif(UNIX)
endif(CMAKE_BUILD_TYPE STREQUAL "Release")

add_executable (GrandChess "src/Main.cpp"  "libs/glad/glad.c" "src/gui/Shaders.cpp" "src/gui/Window.cpp" "src/gui/Shaders.cpp" 
"src/gui/Buffers.cpp" "src/magic bitboard/Board.cpp" "src/magic bitboard/BitMasks.cpp" "src/engine/ChessEngine.cpp"
"src/gui/Application.cpp" "src/engine/UCIconnect.cpp" "src/magic bitboard/misc.cpp" "src/engine/TT.cpp")

target_include_directories(GrandChess PRIVATE "libs/include" "headers")

if(UNIX)
    target_link_libraries(GrandChess glfw)
else(UNIX)
    target_link_libraries(GrandChess PRIVATE "../../libs/glfw/glfw3")
endif(UNIX)

