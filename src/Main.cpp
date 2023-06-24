
#if 0         
#include "Application.h"

int main()
{
    if (!glfwInit())
        exit(-1);


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    Masks::initBitmasks();

    ChessApp app(1000, 1000);

    app.Run();
    return 0;
}
#else

#include "UCIconnect.h"



int main()
{


    Masks::initBitmasks();

    UCIconnection connection;

    connection.Loop();
    return 1;
}


#endif
