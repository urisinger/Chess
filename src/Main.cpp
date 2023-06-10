#include <GLAD/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Shaders.h"
#include "Window.h"
#include "Buffers.h"
#include "BitBoard.h"

#include <iostream>
#include <algorithm>

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



        Window main_win(1000,1000,"meoww",WindowMode::WINDOWED);

        main_win.MakeWindowContextCurrent();
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            glfwTerminate();
            return -1;
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        VertexArray VAO;    

        std::shared_ptr<VertexBuffer> VB     = std::make_shared<VertexBuffer>((void*)sqaure, 8 * sizeof(float));

        VAO.AddAttribute<float>(2, 0, GL_FALSE, 0, VB,0);

        IndexBuffer IBO(sqaure_indcies, 6);

        Texture Tex("../Textures/ChessPiecesArray.png");
        Board Board;
        Shader Backround("../Shaders/Simple.vert", "../Shaders/Backround.frag");
        Shader Pieces("../Shaders/Simple.vert", "../Shaders/Pieces.frag");

        Bitboard::initBitmasks();
        
        Pieces.Bind();
            
        int intboard[64];
        Board.getBoard(intboard);
        glUniform1i(Pieces.Location("_texture"), 0);
        glUniform1iv(Pieces.Location("Board"),64, (GLint*)intboard);
        glUniform2i(Pieces.Location("res"), main_win.GetWindowWidth(), main_win.GetWindowHeight());

        Backround.Bind();

        glUniform2i(Backround.Location("res"), main_win.GetWindowWidth(), main_win.GetWindowHeight());
        glUniform4f(Backround.Location("col1"), 0.82f, 0.545f,0.278f,1.0f);
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
        std::vector<Move> legal;
        int lasttile = -1;



        while (!main_win.WindowShouldClose()) {
            if (glfwGetMouseButton(main_win.GetWindowInstance(), GLFW_MOUSE_BUTTON_LEFT) ) {
                if (!mousealreadyclicked) {
                    double xpos, ypos;
                    glfwGetCursorPos(main_win.GetWindowInstance(), &xpos, &ypos);
                    char tile = floor(8 - (ypos / main_win.GetWindowHeight()) * 8) * 8 + floor((xpos / main_win.GetWindowWidth()) * 8);
                    auto found = std::find_if(legal.begin(), legal.end(), [tile, lasttile](const Move& a) { return a.getTo() == tile && a.getFrom() == lasttile; });
                
                    if (found != legal.end()) {
                        Board.movePiece(*found);

                        Backround.Bind();
                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                        Pieces.Bind();
                        Board.getBoard(intboard);
                        glUniform1iv(Pieces.Location("Board"), 64, (GLint*)intboard);


                        int highlights[64];
                        std::fill(highlights, highlights + 64, 0);

                        glUniform1iv(Pieces.Location("Highlight"), 64, (GLint*)&highlights[0]);

                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                        glfwSwapBuffers(main_win.GetWindowInstance());
                        legal.clear();
                    }
                    else {
                        legal.clear();
                        Backround.Bind();
                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


                        Pieces.Bind();
                        Board.getBoard(intboard);
                        glUniform1iv(Pieces.Location("Board"), 64, (GLint*)intboard);


                        int highlights[64];
                        std::fill(highlights, highlights + 64, 0);

                        legal = Board.GenerateLegalMoves(Board.currentPlayer);
                        for (auto move : legal) {
                            if (move.getFrom() == tile)
                                highlights[move.getTo()] = 1;
                        }


                        glUniform1iv(Pieces.Location("Highlight"), 64, (GLint*)&highlights[0]);

                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                        glfwSwapBuffers(main_win.GetWindowInstance());
                        mousealreadyclicked = true;
                        lasttile = tile;
                    }
                }
            }
            else{
                mousealreadyclicked = false;
            }
            glfwWaitEvents();
        }

    }
        
    glfwTerminate();
    return 0;
}
    
