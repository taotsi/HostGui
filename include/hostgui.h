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

/*
    coordinate system from this to OpenGL:
    this:
    z
    |
    |   y
    | /
    0------x
    OpenGL:
        y
        |
        |
        |
        0------x
      /
    z
*/

/*
    a rectangle is composed of two triangles
    p3-----*p2*
    | \     |
    |   \   |
    |     \ |
    *p1*-----p4
*/

class HostGui{
public:
    HostGui();
    HostGui(const HostGui&) = default;
    HostGui(HostGui&&) = default;
    HostGui& operator=(const HostGui&) = default;
    HostGui& operator=(HostGui&&) = default;
    ~HostGui();

    bool is_on(){return static_cast<bool>(is_on_);}

    void AddPlane(float x1, float y1, float z1, float x2, float y2, float z3);
    void FlushPlanes();
    void AddPointToGo(float x, float y, float z);
    void FlushPointToGo();
    void AddPlotExam(int no, float value);

private:
    std::thread thread_;
    void ThreadMain();
    std::atomic<bool> is_on_ = true;

    const unsigned int SCR_WIDTH_ = 1280;
    const unsigned int SCR_HEIGHT_ = 720;

    // [x11, y11, z11, x12, y12, z12, x21, y21, z21, x22, y22, z22,...]
    std::vector<float> planes_;
    std::vector<std::vector<float>> plots_;
    // [x1, y1, z1, x2, y2, z2, ...]
    std::vector<float> path_to_go_;
    // [x1, y1, z1, x2, y2, z2, ...]
    std::vector<float> path_been_;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void glfw_error_callback(int error, const char* description);
    void processInput(GLFWwindow *window);

    void MainPanel();

    void RenderPoints(std::vector<float> data, Shader &shader, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection);
};

#endif // HOSTGUI_H_