include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/deps/
    ${PROJECT_SOURCE_DIR}/deps/imgui
    ${PROJECT_SOURCE_DIR}/deps/gl3w/
    ${PROJECT_SOURCE_DIR}/deps/glfw321/include
    ${PROJECT_SOURCE_DIR}/deps/stb
    ${PROJECT_SOURCE_DIR}/deps/glm
)

link_directories(${PROJECT_SOURCE_DIR}/deps/glfw321/lib-vc2015)

add_executable(HostGui
    main.cc
    hostgui.cc
    imgui_impl_glfw.cpp
    imgui_impl_opengl3.cpp
    ${PROJECT_SOURCE_DIR}/deps/imgui/imgui_demo.cpp
    ${PROJECT_SOURCE_DIR}/deps/imgui/imgui_draw.cpp
    ${PROJECT_SOURCE_DIR}/deps/imgui/imgui_widgets.cpp
    ${PROJECT_SOURCE_DIR}/deps/imgui/imgui.cpp
    ${PROJECT_SOURCE_DIR}/deps/gl3w/GL/gl3w.c
)

target_link_libraries(HostGui glfw3.lib)
target_link_libraries(HostGui opengl32.lib)

if(MSVC)
    set_target_properties(HostGui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
    set_target_properties(HostGui PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/bin)
    set_target_properties(HostGui PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/bin)
    # etc for the other available configuration types (MinSizeRel, RelWithDebInfo)
endif(MSVC)