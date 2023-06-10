#pragma once
#include <GLAD/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Shaders.h"
#include "Window.h"
#include "Buffers.h"
#include "ChessBoard.h"

#include <iostream>

class Application {

public:
	Application(int height, int width);
	~Application();

	void Run();
private:
	glad_glWindowPos2d
	std::vector<Window> Windows;
	ChessBoard Board;
};