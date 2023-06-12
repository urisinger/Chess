#include <GLAD/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Shaders.h"
#include "Window.h"
#include "Buffers.h"
#include "BitBoard.h"

#include <iostream>
#include <algorithm>
#include <chrono>

struct ScoredMove {
    int score;
    Move move;
};

int countLegalMoves(Board& board, int moveDepth) {
    if (moveDepth == 0) {
        return 1;
    }

    int countt = 0;
    auto moves = board.GenerateLegalMoves(board.currentPlayer);

    for (int i = 0; i < moves.count; i++) {
        Board newBoard = board;
        newBoard.movePiece(moves.moves[i]);

        countt += countLegalMoves(newBoard, moveDepth - 1);
    }

    return countt;
}

int count = 0;
int Minimax(int depth, Board& board, int alpha, int beta, bool maximizingPlayer) {
    if (depth == 0) {
        // Evaluate the current board position and return the evaluation score
        count++;
        return (maximizingPlayer ? 1 : -1) * (board.currentPlayer == WHITE ? 1 : -1) * board.eval();
    }

    auto moves = board.GenerateLegalMoves(board.currentPlayer);
    if (moves.count == 0) {
        // Handle the case where no legal moves are available
        count++;
        return board.isKingAttacked(board.currentPlayer) ? (maximizingPlayer ? std::numeric_limits<int>::min()+1 : std::numeric_limits<int>::max()-1) : 0;
    }

    if (maximizingPlayer) {
        int maxScore = std::numeric_limits<int>::min();
        for (int i = 0; i < moves.count; i++) {
            Board newboard = board;
            newboard.movePiece(moves.moves[i]);

            int currentScore = Minimax(depth - 1, newboard, alpha, beta, false);

            maxScore = std::max(maxScore, currentScore);
            alpha = std::max(alpha, maxScore);

            if (alpha >= beta) {
                // Beta cutoff
                break;
            }
        }
        return maxScore;
    }
    else {
        int minScore = std::numeric_limits<int>::max();
        for (int i = 0; i < moves.count; i++) {
            Board newboard = board;
            newboard.movePiece(moves.moves[i]);

            int currentScore = Minimax(depth - 1, newboard, alpha, beta, true);

            minScore = std::min(minScore, currentScore);
            beta = std::min(beta, minScore);

            if (beta <= alpha) {
                // Alpha cutoff
                break;
            }
        }
        return minScore;
    }
}

Move BestMove(int depth, Board& board) {
    auto moves = board.GenerateLegalMoves(board.currentPlayer);
    if (moves.count == 0) {
        // Handle the case where no legal moves are available
        return Move(); // Return an empty move
    }

    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();
    int maxScore = std::numeric_limits<int>::min();
    Move bestMove;

    for (int i = 0; i < moves.count; i++) {
        Board newboard = board;
        newboard.movePiece(moves.moves[i]);

        int currentScore = Minimax(depth - 1, newboard, alpha, beta, false);

        if (currentScore > maxScore) {
            maxScore = currentScore;
            bestMove = moves.moves[i];
        }

        alpha = std::max(alpha, maxScore);

        if (alpha >= beta) {
            // Beta cutoff
            break;
        }
    }

    std::cout << maxScore << "\n";
    return bestMove;
}


int main()
{
    {
        if (!glfwInit())
            return -1;


        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        float sqaure[8] = {
            1.0f,1.0f,
            -1.0f,-1.0f,
            -1.0f,1.0f,
            1.0f,-1.0f,

        };

        unsigned int sqaure_indcies[6] = {
            0,1,2,
            0,1,3
        };



        Window main_win(1000, 1000, "CHESS GAME OK?", WindowMode::WINDOWED);

        main_win.MakeWindowContextCurrent();
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            glfwTerminate();
            return -1;
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        VertexArray VAO;

        std::shared_ptr<VertexBuffer> VB = std::make_shared<VertexBuffer>((void*)sqaure, 8 * sizeof(float));

        VAO.AddAttribute<float>(2, 0, GL_FALSE, 0, VB, 0);

        IndexBuffer IBO(sqaure_indcies, 6);

        Texture Tex("../Textures/ChessPiecesArray.png");
        Board board;
        Shader Backround("../Shaders/Simple.vert", "../Shaders/Backround.frag");
        Shader Pieces("../Shaders/Simple.vert", "../Shaders/Pieces.frag");

        Bitboard::initBitmasks();

        Pieces.Bind();

        int intboard[64];
        board.getBoard(intboard);
        glUniform1i(Pieces.Location("_texture"), 0);
        glUniform1iv(Pieces.Location("Board"), 64, (GLint*)intboard);
        glUniform2i(Pieces.Location("res"), main_win.GetWindowWidth(), main_win.GetWindowHeight());

        Backround.Bind();

        glUniform2i(Backround.Location("res"), main_win.GetWindowWidth(), main_win.GetWindowHeight());
        glUniform4f(Backround.Location("col1"), 0.82f, 0.545f, 0.278f, 1.0f);
        glUniform4f(Backround.Location("col2"), 1.0f, 0.808f, 0.62f, 1.0f);


        VAO.Bind();
        IBO.Bind();

        Tex.bind(0);

        Backround.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        Pieces.Bind();
        glUniform1iv(Pieces.Location("Board"), 64, (GLint*)intboard);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(main_win.GetWindowInstance());

        bool mousealreadyclicked = false;
        LegalMoves legal;
        int lasttile = -1;
        auto start = std::chrono::high_resolution_clock::now();
        count = 0;
        int countt = countLegalMoves(board,4);
        std::cout << (std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start)).count() << ": " <<countt << std::endl;

        while (!main_win.WindowShouldClose()) {
            if (glfwGetMouseButton(main_win.GetWindowInstance(), GLFW_MOUSE_BUTTON_LEFT)) {
                if (!mousealreadyclicked) {
                    double xpos, ypos;
                    glfwGetCursorPos(main_win.GetWindowInstance(), &xpos, &ypos);
                    int tile = (int)(8 - (ypos / main_win.GetWindowHeight()) * 8) * 8 + (int)((xpos / main_win.GetWindowWidth()) * 8);
                    auto found = std::find_if(legal.moves, legal.moves + legal.count, [tile, lasttile](const Move& a) { return a.getTo() == tile && a.getFrom() == lasttile; });

                    if (found != legal.moves + legal.count) {
                        board.movePiece(*found);

                        Backround.Bind();
                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                        Pieces.Bind();
                        board.getBoard(intboard);
                        glUniform1iv(Pieces.Location("Board"), 64, (GLint*)intboard);


                        int highlights[64];
                        std::fill(highlights, highlights + 64, 0);

                        glUniform1iv(Pieces.Location("Highlight"), 64, (GLint*)&highlights[0]);

                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                        glfwSwapBuffers(main_win.GetWindowInstance());

                        auto start = std::chrono::high_resolution_clock::now();
                        count = 0;
                        Move move = BestMove(8, board);
                        std::cout << (std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start)).count() << ": " << move.to_str() << ": " << count << std::endl;



                        legal.clear();
                    }
                    else {
                        legal.clear();
                        Backround.Bind();
                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


                        Pieces.Bind();
                        board.getBoard(intboard);
                        glUniform1iv(Pieces.Location("Board"), 64, (GLint*)intboard);


                        int highlights[64];
                        std::fill(highlights, highlights + 64, 0);

                        auto start = std::chrono::high_resolution_clock::now();
                        legal = board.GenerateLegalMoves(board.currentPlayer);
                        std::cout << (std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start)).count() << std::endl;
                        for (int i = 0; i < legal.count; i++) {
                            if (legal.moves[i].getFrom() == tile)
                                highlights[legal.moves[i].getTo()] = 1;
                        }


                        glUniform1iv(Pieces.Location("Highlight"), 64, (GLint*)&highlights[0]);

                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                        glfwSwapBuffers(main_win.GetWindowInstance());
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

    glfwTerminate();
    return 0;
}

