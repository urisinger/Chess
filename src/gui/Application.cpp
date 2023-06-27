#include "gui/Application.h"

// Define vertex positions for full screen  
float sqaureVertcies[8] = {
    1.0f, 1.0f,
    -1.0f, -1.0f,
    -1.0f, 1.0f,
    1.0f, -1.0f,
};

// Define indices for drawing the full screen
unsigned int sqaure_indcies[6] = {
    0, 1, 2,
    0, 1, 3
};

// Constructor for the ChessApp class
ChessApp::ChessApp(int xSize, int ySize) :
    _window(1000, 1000, "GrandChess", WindowMode::WINDOWED),
    _backroundShader("./Shaders/Simple.vert", "./Shaders/Backround.frag"),
    _piecesShader("./Shaders/Simple.vert", "./Shaders/Pieces.frag"),
    _pieces("./Textures/ChessPiecesArray.png")
{
    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create and set up the square vertex buffer
    _sqauareVBO = std::make_shared<VertexBuffer>((void*)sqaureVertcies, 8 * sizeof(float));
    _sqauareVAO.AddAttribute<float>(2, 0, GL_FALSE, 0, _sqauareVBO, 0);

    // Set up the square index buffer
    _sqaureIBO.AddData(sqaure_indcies, 6);

    // Bind the background shader and set its uniform values
    _backroundShader.Bind();
    glUniform2i(_backroundShader.Location("res"), _window.GetWindowWidth(), _window.GetWindowHeight());
    glUniform4f(_backroundShader.Location("col1"), 0.82f, 0.545f, 0.278f, 1.0f);
    glUniform4f(_backroundShader.Location("col2"), 1.0f, 0.808f, 0.62f, 1.0f);

    // Bind the pieces shader and set its uniform values
    _piecesShader.Bind();
    glUniform2i(_piecesShader.Location("res"), _window.GetWindowWidth(), _window.GetWindowHeight());
    glUniform1i(_piecesShader.Location("_texture"), 0);
}

// Destructor for the ChessApp class
ChessApp::~ChessApp() {
    glfwTerminate();
}

// Run the chess application
void ChessApp::Run() {
    _engine.maxTime = 1000;

    _sqauareVAO.Bind();
    _sqaureIBO.Bind();
    std::fill(highlights, highlights + 64, 0);

    _window.MakeWindowContextCurrent();
    DrawBackround();
    DrawPieces();
    glfwSwapBuffers(_window.GetWindowInstance());
    _engine.startTime = GetTimeMs();
    Move bestmove = _engine.BestMove(10000, _board);

    bool mousealreadyclicked = false;
    int lasttile = -1;
    LegalMoves legal = _board.GenerateLegalMoves(WHITE);

    while (!_window.WindowShouldClose()) {
        // Check if the spacebar is pressed
        if (glfwGetKey(_window.GetWindowInstance(), GLFW_KEY_SPACE) == GLFW_PRESS) {
            // Check if bestmove is valid
            if (bestmove != Move()) {
                _board.MakeMove(bestmove);
                _engine.offset++;
            }
            std::fill(highlights, highlights + 64, 0);

            DrawBackround();
            DrawPieces();
            glfwSwapBuffers(_window.GetWindowInstance());
            _engine.startTime = GetTimeMs();

            bestmove = _engine.BestMove(10000, _board);
            if (*((unsigned int*)&bestmove)) {
                std::cout << bestmove.to_str();
            }
            else {
                std::cout << "haha! you mate!";
            }
            std::cout << std::endl;
        }

        // Check if the left mouse button is clicked
        if (glfwGetMouseButton(_window.GetWindowInstance(), GLFW_MOUSE_BUTTON_LEFT)) {
            if (!mousealreadyclicked) {
                double xpos, ypos;
                glfwGetCursorPos(_window.GetWindowInstance(), &xpos, &ypos);
                int tile = (int)(8 - (ypos / _window.GetWindowHeight()) * 8) * 8 + (int)((xpos / _window.GetWindowWidth()) * 8);
                auto found = std::find_if(legal.moves, legal.moves + legal.count, [tile, lasttile](const Move& a) { return a.getTo() == tile && a.getFrom() == lasttile; });
                // Check if move is legal
                if (found != legal.moves + legal.count) {
                    _board.MakeMove(*found);

                    std::fill(highlights, highlights + 64, 0);

                    DrawBackround();
                    DrawPieces();
                    glfwSwapBuffers(_window.GetWindowInstance());
                    _engine.startTime = GetTimeMs();

                    bestmove = _engine.BestMove(10000, _board);
                    if (*((unsigned int*)&bestmove)) {
                        std::cout << bestmove.to_str();
                    }
                    else {
                        std::cout << "haha! you mate!";
                    }
                    std::cout << std::endl;

                    legal.clear();
                }
                else {
                    // Render legal moves
                    std::fill(highlights, highlights + 64, 0);

                    legal = _board.GenerateLegalMoves(_board.currentPlayer);
                    for (int i = 0; i < legal.count; i++) {
                        if (legal.moves[i].getFrom() == tile)
                            highlights[legal.moves[i].getTo()] = 1;
                    }

                    DrawBackround();
                    DrawPieces();
                    glfwSwapBuffers(_window.GetWindowInstance());

                    mousealreadyclicked = true;
                    lasttile = tile;
                }
            }
        }
        else {
            mousealreadyclicked = false;
        }
        glfwWaitEvents();
    }
}

// Draw the background using the background shader
void ChessApp::DrawBackround() {
    _backroundShader.Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

// Draw the chess pieces using the pieces shader
void ChessApp::DrawPieces() {
    _board.getBoard(boardArr);
    _piecesShader.Bind();
    glUniform1iv(_piecesShader.Location("Board"), 64, (GLint*)boardArr);
    glUniform1iv(_piecesShader.Location("Highlight"), 64, (GLint*)highlights);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
