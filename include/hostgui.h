#ifndef HOSTGUI_H_
#define HOSTGUI_H_

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <thread>
#include <iostream>
#include <vector>
#include <atomic>
#include "shader.h"

//TODO: 3d plane render
//TODO: mouse click response
//TODO: image play, RGB, disparity/pointcloud
//TODO: 2d plot, vectors
//TODO: seperate ogl window

class HostGui{
public:
    HostGui();
    HostGui(const HostGui&) = default;
    HostGui(HostGui&&) = default;
    HostGui& operator=(const HostGui&) = default;
    HostGui& operator=(HostGui&&) = default;
    ~HostGui();

    bool is_on(){return static_cast<bool>(is_on_);}

private:
    std::thread thread_;
    void ThreadMain();
    std::atomic<bool> is_on_ = true;

    const unsigned int SCR_WIDTH_ = 1280;
    const unsigned int SCR_HEIGHT_ = 720;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void glfw_error_callback(int error, const char* description);
    void processInput(GLFWwindow *window);

    void GuiWindow();
    void RenderPoints(std::vector<float> data, Shader &shader, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection);
};

#endif // HOSTGUI_H_