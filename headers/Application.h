#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Shaders.h"
#include "Window.h"
#include "Buffers.h"
#include "BitBoard.h"

#include <iostream>

#include "ChessEngine.h"


class ChessApp {
public:
	ChessApp(int xSize, int ySize);

	~ChessApp();

	void Run();

	void DrawBackround();

	void DrawPieces();
private:

	Window _window;


	int boardArr[64];

	int highlights[64];
	Shader _backroundShader;
	Shader _piecesShader;

	VertexArray _sqauareVAO;

	std::shared_ptr<VertexBuffer> _sqauareVBO;
	IndexBuffer _sqaureIBO;

	Texture _pieces;
	Board _board;
	ChessEngine _engine;
};