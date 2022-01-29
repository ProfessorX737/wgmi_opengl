#ifndef COMP3421_SCENE_HPP
#define COMP3421_SCENE_HPP

#include "model.hpp"
#include "euler_camera.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>

namespace scene {
    struct node_t {
        enum KIND {
            EMPTY, STATIC_MESH, REFLECTIVE, WATER_SURFACE, WATER,
        } kind = EMPTY;
        model::model_t model;
        glm::vec3 translation = glm::vec3(0.0);
        glm::vec3 rotation = glm::vec3(0.0); // vec3 of euler angles
        glm::vec3 scale = glm::vec3(1.0);
        std::vector<node_t> children;
        glm::vec2 polygon_offset = glm::vec2(0.0);
        bool visible = true;
        bool clipping = false;

        // for rainbow color
        bool rainbow_colors = false;
        glm::vec3 color_rotation = glm::vec3(0.0); // vec3 of euler angles
        float color_offset = 0.0f;

        bool show_line_mesh = false;
    };

    node_t make_wgmi_head(float radius, float thickness);

} // namespace scene

#endif // COMP3421_SCENE_HPP
