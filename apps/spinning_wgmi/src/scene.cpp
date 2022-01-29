#include "scene.hpp"
#include "shapes.hpp"
#include "cubemap.hpp"
#include "texture_2d.hpp"
#include <iostream>

namespace scene {
    node_t make_wgmi_head(float radius, float thickness) {

        auto torus = scene::node_t{};
        auto torus_template = shapes::make_torus(radius, thickness);
        torus.model.meshes.push_back(mesh::init(torus_template));
        auto torus_mat = model::material_t{};
        torus_mat.diffuse = glm::vec4(0,0,0,1);
        torus_mat.specular = glm::vec3(0);
        torus.model.materials.push_back({});

        auto horiz = scene::node_t{};
        for(int i = 0; i < 4; ++i) {
            auto ring = torus;
            ring.rotation.z = glm::radians(45.0f) * i;
            horiz.children.push_back(ring);
        }
        horiz.rotation.x = glm::radians(90.0f);

        auto verts = scene::node_t{};
        float start_angle = glm::radians(315.0f);
        for(int i = 0; i < 3; ++i) {
            float angle = start_angle + i * glm::radians(45.0f);
            float x = radius * glm::cos(angle);
            float y = radius * glm::sin(angle);
            auto temp = shapes::make_torus(x, thickness);
            auto ring = scene::node_t{};
            ring.model.meshes.push_back(mesh::init(temp));
            ring.model.materials.push_back({});
            ring.translation.y = y;
            verts.children.push_back(ring);
        }

        auto head = scene::node_t{};
        head.children.push_back(verts);
        head.children.push_back(horiz);

        return head;
    }
} // namespace scene