#include "Window.h"


Window::Window(const unsigned int& windowWidth, const unsigned int& windowHeight, const char* windowTitle, WindowMode windowMode)
{
    WindowProps windowProperties;

    windowProperties.WindowWidth = windowWidth;
    windowProperties.WindowHeight = windowHeight;
    windowProperties.WindowTitle = windowTitle;
    windowProperties.WM = windowMode;

    InitWindow(windowProperties);
}

Window::Window()
{
    InitWindow();
}

void Window::InitWindow(const WindowProps& windowProperties)
{
    m_WindowData.WindowWidth = windowProperties.WindowWidth;
    m_WindowData.WindowHeight = windowProperties.WindowHeight;
    m_WindowData.WindowTitle = windowProperties.WindowTitle;

    m_Window = glfwCreateWindow(
            windowProperties.WM == WindowMode::FULLSCREEN ? GetMonitorWidth() : windowProperties.WindowWidth,
            windowProperties.WM == WindowMode::FULLSCREEN ? GetMonitorHeight() : windowProperties.WindowHeight,
            windowProperties.WindowTitle,
            windowProperties.WM == WindowMode::FULLSCREEN ? glfwGetPrimaryMonitor() : nullptr, nullptr
    );

    if (m_Window == nullptr) {
        return;
    }
}

Window::~Window()
{
    glfwDestroyWindow(m_Window);
}

void Window::MakeWindowContextCurrent()
{
    glfwMakeContextCurrent(m_Window);
}



void Window::WindowOnFocus()
{
    glfwFocusWindow(m_Window);
}

void Window::MaximizeWindow()
{
    glfwMaximizeWindow(m_Window);
}

void Window::MinimizeWindow()
{
    glfwIconifyWindow(m_Window);
}

void Window::RestoreWindow()
{
    glfwRestoreWindow(m_Window);
}

void Window::CloseWindow()
{
    glfwSetWindowShouldClose(m_Window, GL_TRUE);
}

void Window::SetWindowWidth(const unsigned int& windowWidth)
{
    m_WindowData.WindowWidth = windowWidth;

    glfwSetWindowSize(m_Window, m_WindowData.WindowWidth, m_WindowData.WindowHeight);
}

void Window::SetWindowHeight(const unsigned int& windowHeight)
{
    m_WindowData.WindowHeight = windowHeight;

    glfwSetWindowSize(m_Window, m_WindowData.WindowWidth, m_WindowData.WindowHeight);
}

void Window::SetWindowSizeLimit(const unsigned int& windowMinWidth, const unsigned int& windowMinHeight, const unsigned int& windowMaxWidth,
                                const unsigned int& windowMaxHeight)
{
    m_WindowMinimumWidth = windowMinWidth;
    m_WindowMinimumHeight = windowMinHeight;
    m_WindowMaximumWidth = windowMaxWidth;
    m_WindowMaximumHeight = windowMaxHeight;

    glfwSetWindowSizeLimits(m_Window, m_WindowMinimumWidth, m_WindowMinimumHeight, m_WindowMaximumWidth, m_WindowMaximumHeight);
}

void Window::SetWindowTitle(const char* windowTitle)
{
    m_WindowData.WindowTitle = windowTitle;

    glfwSetWindowTitle(m_Window, m_WindowData.WindowTitle);
}

unsigned int Window::GetMonitorWidth()
{
    const GLFWvidmode* VidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    m_MonitorWidth = VidMode->width;

    return m_MonitorWidth;
}

unsigned int Window::GetMonitorHeight()
{
    const GLFWvidmode* VidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    m_MonitorHeight = VidMode->height;

    return m_MonitorHeight;
}