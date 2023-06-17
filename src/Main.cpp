#include "Application.h"

int main()
{
    if (!glfwInit())
        exit(-1);


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    ChessApp app(1000, 1000);

    app.Run();
    return 0;
}

