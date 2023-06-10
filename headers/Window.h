#pragma once
#include <GLFW/glfw3.h>


enum class WindowMode
{
    FULLSCREEN = 0,
    WINDOWED = 1
};

struct WindowProps
{
    unsigned int    WindowWidth;
    unsigned int    WindowHeight;
    const char* WindowTitle;
    WindowMode  WM;

    WindowProps(unsigned int windowWidth = 1280,
                unsigned int windowHeight = 720,
                const char* windowTitle = "OpenGL",
                WindowMode wm = WindowMode::WINDOWED)
            : WindowWidth(windowWidth), WindowHeight(windowHeight), WindowTitle(windowTitle), WM(wm)
    {
    }
};

class Window
{
private:
    GLFWwindow* m_Window;

    unsigned int    m_WindowMinimumWidth;
    unsigned int    m_WindowMaximumWidth;
    unsigned int    m_WindowMinimumHeight;
    unsigned int    m_WindowMaximumHeight;

    unsigned int    m_MonitorWidth;
    unsigned int    m_MonitorHeight;

    struct WindowData
    {
        unsigned int    WindowWidth;
        unsigned int    WindowHeight;
        const char* WindowTitle;
        WindowMode  WM;
    } m_WindowData;

private:
    void InitWindow(const WindowProps& props = WindowProps());

public:
    Window(const unsigned int& windowWidth, const unsigned int& windowHeight, const char* windowTitle, WindowMode windowMode);
    Window();
    ~Window();

    void MakeWindowContextCurrent();
    void WindowOnFocus();

    void MaximizeWindow();
    void MinimizeWindow();
    void RestoreWindow();
    void CloseWindow();

    void SetWindowWidth(const unsigned int& windowWidth);
    void SetWindowHeight(const unsigned int& windowHeight);
    void SetWindowSizeLimit(const unsigned int& windowMinWidth, const unsigned int& windowMinHeight, const unsigned int& windowMaxWidth, const unsigned int& windowMaxHeight);
    void SetWindowTitle(const char* windowTitle);

    unsigned int GetMonitorWidth();
    unsigned int GetMonitorHeight();

    inline unsigned int GetWindowWidth() const { return m_WindowData.WindowWidth; }
    inline unsigned int GetWindowHeight() const { return m_WindowData.WindowHeight; }
    inline unsigned int GetWindowMinimumWidth() const { return m_WindowMinimumWidth; }
    inline unsigned int GetWindowMaximumWidth() const { return m_WindowMaximumWidth; }
    inline unsigned int GetWindowMinimumHeight() const { return m_WindowMinimumHeight; }
    inline unsigned int GetWindowMaximumHeight() const { return m_WindowMaximumHeight; }
    inline const char* GetWindowTitle() const { return m_WindowData.WindowTitle; }
    inline int WindowShouldClose() const { return glfwWindowShouldClose(m_Window); }

    inline GLFWwindow* GetWindowInstance() const { return m_Window; }
};