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
#include <mutex>
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
    so it's like from (x, y, z) to (x, z, -y)
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

    void AddPlane(float x1, float y1, float z1, float x2, float y2, float z2);
    void FlushPlanes();
    void AddPointToGo(float x, float y, float z);
    void FlushPointToGo();
    void AddPointBeen(float x, float y, float z);
    void FlushPointBeen();
    void AddPlotVal(int no, float value);

private:
    std::thread thread_;
    void ThreadMain();
    std::mutex mtx_;
    std::atomic<bool> is_on_ = true;

    const unsigned int SCR_WIDTH_ = 1280;
    const unsigned int SCR_HEIGHT_ = 720;

    float map_range_ = 10;

    bool show_planes_ = true;
    bool show_path_togo_ = true;
    bool show_path_been_ = false;
    bool show_image_window_ = false;
    bool show_point_cloud_ = false;

    // data format of planes:
    // [x11, y11, z11, x12, y12, z12, x21, y21, z21, x22, y22, z22,...]
    std::vector<float> planes_;
    // data format of path:
    // [x1, y1, z1, x2, y2, z2, ...]
    std::vector<float> path_togo_;
    std::vector<float> path_been_;
    // data format of point cloud
    std::vector<float> point_cloud_;
    std::vector<std::vector<float>> plots_;

    void RenderPlanes(Shader &shader, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection);
    void RenderPointCloud();
    void RenderPathToGo();
    void RenderPathBeen();
    void RenderImageWindow();

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void glfw_error_callback(int error, const char* description);
    void processInput(GLFWwindow *window);

    void MainPanel();

    void RenderPoints(std::vector<float> data, Shader &shader, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection);
};

#endif // HOSTGUI_H_