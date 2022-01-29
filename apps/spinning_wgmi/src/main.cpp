#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <memory>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include <chicken3421/chicken3421.hpp>

#include "texture_2d.hpp"
#include "shapes.hpp"
#include "euler_camera.hpp"
#include "memes.hpp"
#include "renderer.hpp"
#include "framebuffer.hpp"

const int SCR_WIDTH = 1280;
const int SCR_HEIGHT = 720;

const char *WIN_TITLE = "WGMI";

namespace {
    double time_delta() {
        static double then = glfwGetTime();
        double now = glfwGetTime();
        double dt = now - then;
        then = now;
        return dt;
    }
} // namespace

scene::node_t make_shape(mesh::mesh_template_t temp) {
    auto shape = scene::node_t{};
    shape.model.meshes.push_back(mesh::init(temp));
    auto shape_mat = model::material_t{};
    shape_mat.diffuse = glm::vec4(0,0,0,1);
    shape_mat.specular = glm::vec3(0);
    shape.model.materials.push_back(shape_mat);
    return shape;
}

int main() {
#ifndef __APPLE__
    chicken3421::enable_debug_output();
#endif
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow *window = marcify(chicken3421::make_opengl_window(SCR_WIDTH, SCR_HEIGHT, WIN_TITLE));
    glEnable(GL_MULTISAMPLE);
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    auto camera = euler_camera::make_camera({0, 0, 3}, {0, 0, 0});
    float aspect = (float) SCR_WIDTH / (float) SCR_HEIGHT;
    auto renderer = renderer::init(
            glm::perspective(glm::radians(60.0), (double) SCR_WIDTH / (double) SCR_HEIGHT, 0.1, 1000.0));
//    glm::ortho(-aspect,aspect,-1.0f,1.0f, 0.1f, 1000.0f));

    float radius = 1.0f;
    float angle_thickness = glm::radians(5.0f);
    float thickness = radius * angle_thickness;

    auto head_frame = make_shape(shapes::make_sphere_skeleton(radius, angle_thickness, 8));
    head_frame.clipping = true;
    auto face = make_shape(shapes::make_wgmi_face(radius - thickness + 0.01));
    face.clipping = true;
    face.rainbow_colors = true;
    face.color_offset = radius;

    auto face_tex = make_shape(shapes::make_wgmi_face(radius - thickness - 0.01));
    face_tex.clipping = true;
//    auto face = make_shape(shapes::make_sphere(radius + thickness));
    face_tex.model.materials[0].diffuse_map = texture_2d::init("res/textures/wgmi/wgmi_face_sleep.png");
    auto awake_tex = texture_2d::init("res/textures/wgmi/wgmi_face_awake.png");

    auto head = scene::node_t{};
    head.children.push_back(head_frame);
    head.children.push_back(face);
    head.children.push_back(face_tex);
//    head.rotation.y = -M_PI/2;

    auto outer_ring = make_shape(shapes::make_zero_character(radius, 0, thickness));
    outer_ring.rainbow_colors = true;
    outer_ring.color_offset = radius;
//    outer_ring.color_rotation.y = M_PI/2;

    auto scene = scene::node_t{};
    scene.children.push_back(head);
    scene.children.push_back(outer_ring);

    float start_time = glfwGetTime();
    bool end = false;
    while (!glfwWindowShouldClose(window)) {
        auto dt = (float) time_delta();
//        euler_camera::update_camera(camera, window, dt);

        float rot = scene.children[0].rotation.y;
        float delta_rot;
        if(rot > 3.0f * M_PI) scene.children[0].children[2].model.materials[0].diffuse_map = awake_tex;
        if(rot > 3.5f * M_PI) {
            delta_rot = (dt * -glm::sin(rot));
        } else delta_rot = dt;
        scene.children[0].rotation.y += delta_rot;
        scene.children[1].color_rotation.y -= delta_rot;
        scene.children[2].color_rotation.y -= delta_rot;

        renderer::render(renderer, camera, scene);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}
