#include "mesh.hpp"
#include "shapes.hpp"
#include <glm/ext.hpp>
#include <utility>
#include <chicken3421/chicken3421.hpp>
#include <iostream>

namespace shapes {
    void calc_vertex_normals(mesh::mesh_template_t &mesh_template) {
        mesh_template.normals = std::vector<glm::vec3>(mesh_template.positions.size(), glm::vec3(0));
        chicken3421::expect(mesh_template.indices.size() != 0,
                            "shapes::calc_normals requires the mesh_template_t to have indices "
                            "defined");
        const auto &pos = mesh_template.positions;
        for (auto i = size_t{0}; i < mesh_template.indices.size() - 2; i += 3) {
            GLuint i1 = mesh_template.indices[i];
            GLuint i2 = mesh_template.indices[i + 1];
            GLuint i3 = mesh_template.indices[i + 2];
            auto a = pos[i2] - pos[i1];
            auto b = pos[i3] - pos[i1];
            auto face_normal = glm::normalize(glm::cross(a, b));
            mesh_template.normals[i1] += face_normal;
            mesh_template.normals[i2] += face_normal;
            mesh_template.normals[i3] += face_normal;
        }
        // normalise all the normals
        for (auto i = size_t{0}; i < mesh_template.normals.size(); ++i) {
            mesh_template.normals[i] = glm::normalize(mesh_template.normals[i]);
        }
    }

    // assumes the mesh_template does not have indices
    void calc_face_normals(mesh::mesh_template_t &mesh_template) {
        chicken3421::expect(mesh_template.indices.size() == 0,
                            "shapes::calc_face_normals requires the mesh_template to not use "
                            "indices");
        mesh_template.normals =
                std::vector<glm::vec3>(mesh_template.positions.size(), glm::vec3(1, 0, 0));
        const auto &pos = mesh_template.positions;
        for (auto i = size_t{0}; i < mesh_template.positions.size() - 2; i += 3) {
            auto a = pos[i + 1] - pos[i];
            auto b = pos[i + 2] - pos[i];
            auto face_normal = glm::normalize(glm::cross(a, b));
            for (auto j = size_t{0}; j < 3; ++j) {
                mesh_template.normals[i + j] = face_normal;
            }
        }
    }

    // Duplicates any attributes based on the indices provided
    mesh::mesh_template_t expand_indices(const mesh::mesh_template_t &mesh_template) {
        chicken3421::expect(mesh_template.indices.size() != 0,
                            "shapes::expand_indices requires the mesh_template to have indices to "
                            "expand");
        auto new_mesh_template = mesh::mesh_template_t{};
        for (auto i: mesh_template.indices) {
            new_mesh_template.positions.push_back(mesh_template.positions[i]);
            if (!mesh_template.colors.empty()) {
                new_mesh_template.colors.push_back(mesh_template.colors[i]);
            }
            if (!mesh_template.tex_coords.empty()) {
                new_mesh_template.tex_coords.push_back(mesh_template.tex_coords[i]);
            }
            if (!mesh_template.normals.empty()) {
                new_mesh_template.normals.push_back(mesh_template.normals[i]);
            }
        }
        return new_mesh_template;
    }

    mesh::mesh_template_t make_zero_character(float radius, float z, float thickness, unsigned int tessellation, bool flip_normals) {
        mesh::mesh_template_t circle;
        float angle_inc = 2.0 * M_PI / tessellation;
        auto delta_radius = std::vector<float>{0, -thickness};
        for(int dri = 0; dri < delta_radius.size(); ++dri) {
            for (int i = 0; i <= tessellation; ++i) {
                float rad = radius + delta_radius[dri];
                float angle = i * angle_inc;
                float x = rad * std::cos(angle);
                float y = rad * std::sin(angle);
                circle.positions.emplace_back(x, y, z);
                auto center = glm::vec3(0,0, z);
                float flip = (flip_normals * -1) * 2 + 1;
                circle.normals.push_back(flip * glm::normalize(center));
            }
        }
        unsigned int prev = 0;
        unsigned int curr = (1u + tessellation);
        if(flip_normals) std::swap(prev, curr);
        for (unsigned int j = 0; j < tessellation; ++j) {
            circle.indices.push_back(curr + j);
            circle.indices.push_back(prev + j);
            circle.indices.push_back(prev + j + 1);
            circle.indices.push_back(prev + j + 1);
            circle.indices.push_back(curr + j + 1);
            circle.indices.push_back(curr + j);
        }
        circle.colors = circle.positions;
        return circle;
    }

    mesh::mesh_template_t make_ring(float radius, float thickness, unsigned int tessellation, bool flip_normals) {
        mesh::mesh_template_t circle;
        float angle_inc = 2.0 * M_PI / tessellation;
        auto delta_thickness = std::vector<float>{thickness/2, -thickness/2};
        for(int dti = 0; dti < delta_thickness.size(); ++dti) {
            for (int i = 0; i <= tessellation; ++i) {
                float angle = i * angle_inc;
                float x = radius * std::cos(angle);
                float y = radius * std::sin(angle);
                circle.positions.emplace_back(x, y, delta_thickness[dti]);
                auto center = glm::vec3(0,0, delta_thickness[dti]);
                float flip = (flip_normals * -1) * 2 + 1;
                circle.normals.push_back(flip * glm::normalize(circle.positions.back() - center));
            }
        }
        unsigned int prev = 0;
        unsigned int curr = (1u + tessellation);
        if(flip_normals) std::swap(prev, curr);
        for (unsigned int j = 0; j < tessellation; ++j) {
            circle.indices.push_back(prev + j);
            circle.indices.push_back(curr + j);
            circle.indices.push_back(curr + j + 1);
            circle.indices.push_back(curr + j + 1);
            circle.indices.push_back(prev + j + 1);
            circle.indices.push_back(prev + j);
        }
        return circle;
    }

    mesh::mesh_template_t make_rect_circle(float radius, float thickness, unsigned int tessellation) {
        auto circle = mesh::mesh_template_t{};
        auto zero1 = make_zero_character(radius, thickness/2, thickness, tessellation, false);
        auto zero2 = make_zero_character(radius, -thickness/2, thickness, tessellation, true);
        auto ring1 = make_ring(radius, thickness, tessellation, false);
        auto ring2 = make_ring(radius - thickness, thickness, tessellation, true);
        auto meshes = std::vector<mesh::mesh_template_t>{zero1, zero2, ring1, ring2};
        for(int i = 0; i < meshes.size(); ++i) {
            auto size = circle.positions.size();
            circle.positions.insert(circle.positions.end(), meshes[i].positions.begin(), meshes[i].positions.end());
            circle.normals.insert(circle.normals.end(), meshes[i].normals.begin(), meshes[i].normals.end());
            for(auto idx : meshes[i].indices) circle.indices.push_back(size + idx);
        }
        return circle;
    }

    mesh::mesh_template_t make_sphere_rings(float radius, float angle_thickness, unsigned int slices, unsigned int tessellation) {
        mesh::mesh_template_t sphere;
        float slice_ang_inc = (float) 2 * M_PI / (float) slices;
        float ang_inc = 2.0f * (float) M_PI / (float) tessellation;
        unsigned int start_angle_i = 3 * slices / 4;
        float depth_thickness = radius * angle_thickness;
        auto delta_angles = std::vector<float>{-angle_thickness/2, angle_thickness/2};
        auto delta_radius = std::vector<float>{0, -depth_thickness};
        for (unsigned int i = start_angle_i; i <= start_angle_i + (slices/2); ++i) {
            for(int dai = 0; dai < delta_angles.size(); ++dai) {
                float alpha = ((slice_ang_inc * (float) i) + delta_angles[dai] - 2 * M_PI) * 0.9;
                float y = radius * std::sin(alpha);
                for(int dri = 0; dri < delta_radius.size(); ++dri) {
                    float slice_radius = (radius * std::cos(alpha)) + delta_radius[dri];
                    for (unsigned int j = 0; j <= tessellation; ++j) {
                        float beta = ang_inc * (float) j;
                        float z = slice_radius * std::cos(beta);
                        float x = slice_radius * std::sin(beta);
                        sphere.positions.emplace_back(x, y, z);
                        sphere.normals.push_back(glm::normalize(sphere.positions.back()));
                        if(delta_radius[dri] < 0) sphere.normals.back() *= -1;
                    }
                }
            }
        }
        // create the indices
        for (unsigned int i = 0; i <= slices * 2; i+=4) {
            unsigned int br = (1u + tessellation) * i;
            unsigned int bl = (1u + tessellation) * (i+1);
            unsigned int tr = (1u + tessellation) * (i+2);
            unsigned int tl = (1u + tessellation) * (i+3);
            auto sides = std::vector<std::pair<unsigned int, unsigned int>>{{tl,bl}, {br,tr}};
            for(int s = 0; s < sides.size(); ++s) {
                for (unsigned int j = 0; j < tessellation; ++j) {
                    sphere.indices.push_back(sides[s].second + j);
                    sphere.indices.push_back(sides[s].first + j);
                    sphere.indices.push_back(sides[s].first + j + 1);
                    sphere.indices.push_back(sides[s].first + j + 1);
                    sphere.indices.push_back(sides[s].second + j + 1);
                    sphere.indices.push_back(sides[s].second + j);
                }
            }
        }
        return sphere;
    }

    mesh::mesh_template_t make_sphere_zeros(float radius, float angle_thickness, unsigned int slices, unsigned int tessellation) {
        mesh::mesh_template_t sphere;
        float slice_ang_inc = (float) 2 * M_PI / (float) slices;
        float ang_inc = 2.0f * (float) M_PI / (float) tessellation;
        unsigned int start_angle_i = 3 * slices / 4;
        float depth_thickness = radius * angle_thickness;
        auto delta_angles = std::vector<float>{-angle_thickness/2, angle_thickness/2};
        auto delta_radius = std::vector<float>{0, -depth_thickness};
        for (unsigned int i = start_angle_i; i <= start_angle_i + (slices/2); ++i) {
            for(int dai = 0; dai < delta_angles.size(); ++dai) {
                float alpha = ((slice_ang_inc * (float) i) + delta_angles[dai] - 2 * M_PI) * 0.9;
                float y = radius * std::sin(alpha);
                for(int dri = 0; dri < delta_radius.size(); ++dri) {
                    float slice_radius = (radius * std::cos(alpha)) + delta_radius[dri];
                    for (unsigned int j = 0; j <= tessellation; ++j) {
                        float beta = ang_inc * (float) j;
                        float z = slice_radius * std::cos(beta);
                        float x = slice_radius * std::sin(beta);
                        sphere.positions.emplace_back(x, y, z);
                        if(delta_angles[dai] > 0) sphere.normals.emplace_back(0,1,0);
                        else sphere.normals.emplace_back(0,-1,0);
                    }
                }
            }
        }
        // create the indices
        for (unsigned int i = 0; i <= slices * 2; i+=4) {
            unsigned int br = (1u + tessellation) * i;
            unsigned int bl = (1u + tessellation) * (i+1);
            unsigned int tr = (1u + tessellation) * (i+2);
            unsigned int tl = (1u + tessellation) * (i+3);
            auto sides = std::vector<std::pair<unsigned int, unsigned int>>{{bl,br}, {tr,tl}};
            for(int s = 0; s < sides.size(); ++s) {
                for (unsigned int j = 0; j < tessellation; ++j) {
                    sphere.indices.push_back(sides[s].second + j);
                    sphere.indices.push_back(sides[s].first + j);
                    sphere.indices.push_back(sides[s].first + j + 1);
                    sphere.indices.push_back(sides[s].first + j + 1);
                    sphere.indices.push_back(sides[s].second + j + 1);
                    sphere.indices.push_back(sides[s].second + j);
                }
            }
        }
        return sphere;
    }

    mesh::mesh_template_t make_sphere_skeleton(float radius, float angle_thickness, unsigned int slices, unsigned int tessellation) {
        auto sphere = mesh::mesh_template_t{};
        auto sphere_zeros = make_sphere_zeros(radius, angle_thickness, slices, tessellation);
        auto sphere_rings = make_sphere_rings(radius, angle_thickness, slices, tessellation);
        auto thickness = radius * angle_thickness;
        auto meshes = std::vector<mesh::mesh_template_t>{sphere_zeros, sphere_rings};
        auto rect_circle = make_rect_circle(radius, thickness, tessellation);
        for(int i = 0; i < slices/2; ++i) {
            auto vert_circle = rect_circle;
            auto rot = glm::rotate(glm::mat4(1.0f), (float)(i * 2.0f * M_PI / (float) slices), glm::vec3(0,1,0));
            for(auto& pos: vert_circle.positions) pos = glm::vec3(rot * glm::vec4(pos,1));
            for(auto& norm: vert_circle.normals) norm = glm::normalize(glm::vec3(rot * glm::vec4(norm, 1)));
            meshes.push_back(vert_circle);
        }
        for(int i = 0; i < meshes.size(); ++i) {
            auto size = sphere.positions.size();
            sphere.positions.insert(sphere.positions.end(), meshes[i].positions.begin(), meshes[i].positions.end());
            sphere.normals.insert(sphere.normals.end(), meshes[i].normals.begin(), meshes[i].normals.end());
            for(auto idx : meshes[i].indices) sphere.indices.push_back(size + idx);
        }
        for(auto p : sphere.positions) sphere.colors.push_back(glm::normalize(radius + p));
        return sphere;
    }

    mesh::mesh_template_t make_sphere(float radius, unsigned int tessellation) {
        mesh::mesh_template_t sphere;

        float ang_inc = 2.0f * (float) M_PI / (float) tessellation;
        unsigned int stacks = tessellation / 2;
        unsigned int start_angle_i = 3 * tessellation / 4;
        for (unsigned int i = start_angle_i; i <= start_angle_i + stacks; ++i) {
            float alpha = ang_inc * (float) i;
            float y = radius * std::sin(alpha);
            float slice_radius = radius * std::cos(alpha);
            for (unsigned int j = 0; j <= tessellation; ++j) {
                float beta = ang_inc * (float) j;
                float z = slice_radius * std::cos(beta);
                float x = slice_radius * std::sin(beta);
                sphere.positions.emplace_back(x, y, z);
                sphere.tex_coords.emplace_back((float) j * 1.0f / (float) tessellation,
                                               (float) (i - start_angle_i) * 2.0f / (float) tessellation);
                sphere.normals.emplace_back(glm::normalize(sphere.positions.back()));
                sphere.colors.push_back(glm::normalize(sphere.positions.back() + radius));
            }
        }
        // create the indices
        for (unsigned int i = 1; i <= tessellation / 2; ++i) {
            unsigned int prev = (1u + tessellation) * (i - 1);
            unsigned int curr = (1u + tessellation) * i;
            for (unsigned int j = 0; j < tessellation; ++j) {
                sphere.indices.push_back(curr + j);
                sphere.indices.push_back(prev + j);
                sphere.indices.push_back(prev + j + 1);
                sphere.indices.push_back(prev + j + 1);
                sphere.indices.push_back(curr + j + 1);
                sphere.indices.push_back(curr + j);
            }
        }

        return sphere;
    }

    mesh::mesh_template_t make_wgmi_face(float radius, unsigned int tessellation) {
        mesh::mesh_template_t sphere;

        float ang_inc = 2.0f * (float) M_PI / (float) tessellation;
        unsigned int stacks = tessellation / 2;
        unsigned int start_angle_i = 3 * tessellation / 4;
        for (unsigned int i = start_angle_i; i <= start_angle_i + stacks; ++i) {
            float alpha = ang_inc * (float) i;
            float y = radius * std::sin(alpha);
            float slice_radius = radius * std::cos(alpha);
            for (unsigned int j = 0; j <= tessellation; ++j) {
                float beta = ang_inc * (float) j;
                float z = slice_radius * std::cos(beta);
                float x = slice_radius * std::sin(beta);
                sphere.positions.emplace_back(x, y, z);
                sphere.tex_coords.emplace_back(1 - ((float) j * 1.0f / (float) tessellation),
                                               (float) (i - start_angle_i) * 2.0f / (float) tessellation);
                sphere.normals.emplace_back(glm::normalize(sphere.positions.back()));
                sphere.colors.push_back(glm::normalize(sphere.positions.back() + radius));
            }
        }
        float slice = tessellation/8;
        float start_slice = slice * 3;
        float end_slice = slice * 5;
        float start_stack = stacks/2 + 1;
        float end_stack = stacks - slice + 1;
//        float tex_y_inc = 1.0f / (end_stack - start_stack);
//        float tex_x_inc = 1.0f / (end_slice - start_slice);
        // create the indices
        for (unsigned int i = start_stack; i < end_stack; ++i) {
            unsigned int prev = (1u + tessellation) * (i - 1);
            unsigned int curr = (1u + tessellation) * i;
            for (unsigned int j = start_slice; j < end_slice; ++j) {
                sphere.indices.push_back(prev + j);
                sphere.indices.push_back(curr + j);
                sphere.indices.push_back(curr + j + 1);
                sphere.indices.push_back(curr + j + 1);
                sphere.indices.push_back(prev + j + 1);
                sphere.indices.push_back(prev + j);
            }
        }

        return sphere;
    }

    mesh::mesh_template_t make_torus(float radius, float thickness, int tessellation) {
        mesh::mesh_template_t torus;
        int stacks = std::ceil(radius / thickness) * tessellation;
        std::vector<glm::vec3> circle;
        float angle_inc = 2.0 * M_PI / tessellation;
        for (int i = 0; i <= tessellation; ++i) {
            float alpha = angle_inc * (float) i;
            glm::vec3 v;
            v.x = radius + (thickness * std::cos(alpha));
            v.y = thickness * std::sin(alpha);
            v.z = 0.0;
            circle.push_back(v);
        }
        auto center = glm::vec3(radius, 0, 0);
        angle_inc = 2.0 * M_PI / stacks;
        for (auto i = 0u; i <= stacks; ++i) {
            float alpha = angle_inc * (float) i;
            glm::mat4 rot = glm::rotate(glm::mat4(1.0), -alpha, glm::vec3(0, 1, 0));
            auto stack_center = glm::vec3(rot * glm::vec4(center,1));
            for (auto j = 0u; j < circle.size(); ++j) {
                torus.positions.push_back(glm::vec3(rot * glm::vec4(circle[j], 1)));
                torus.tex_coords.emplace_back((float) 4 * i / (float) tessellation - 0.5f,
                                              (float) 12 * j / (float) stacks - 0.5f);
                torus.normals.push_back(torus.positions.back() - stack_center);
                torus.colors.push_back(glm::normalize(torus.positions.back()));
            }
        }
        for (int i = 1; i <= stacks; ++i) {
            int prev = circle.size() * (i - 1);
            int curr = circle.size() * i;
            for (int j = 0; j < circle.size() - 1; ++j) {
                torus.indices.push_back(curr + j);
                torus.indices.push_back(prev + j);
                torus.indices.push_back(prev + j + 1);
                torus.indices.push_back(prev + j + 1);
                torus.indices.push_back(curr + j + 1);
                torus.indices.push_back(curr + j);
            }
        }
        return torus;
    }

    mesh::mesh_template_t make_cube(float width) {
        mesh::mesh_template_t cube;
        float hw = width / 2;
        cube.positions = {
                // front square
                {-hw, hw,  hw}, // top-left 0
                {-hw, -hw, hw}, // bottom-left 1
                {hw,  -hw, hw}, // bottom-right 2
                {hw,  hw,  hw}, // top-right 3
                // back square
                {-hw, hw,  -hw}, // top-left 4
                {-hw, -hw, -hw}, // bottom-left 5
                {hw,  -hw, -hw}, // bottom-right 6
                {hw,  hw,  -hw}, // top-right 7
        };

        cube.indices = {
                // front
                0,
                1,
                2,
                2,
                3,
                0,
                // back
                4,
                7,
                6,
                6,
                5,
                4,
                // top
                4,
                0,
                3,
                3,
                7,
                4,
                // bottom
                5,
                6,
                2,
                2,
                1,
                5,
                // left
                0,
                4,
                5,
                5,
                1,
                0,
                // right
                3,
                2,
                6,
                6,
                7,
                3,
        };
        return cube;
    }

    mesh::mesh_template_t make_plane(int width, int height) {
        mesh::mesh_template_t mesh_template;
        float hw = (float) width / 2.0f;
        float hh = (float) height / 2.0f;
        for (auto i = size_t{0}; i <= width; ++i) {
            for (auto j = size_t{0}; j <= height; ++j) {
                mesh_template.positions.emplace_back(-hw + (float) i, -hh + (float) j, 0);
                mesh_template.tex_coords.emplace_back((float) i / width, (float) j / height);
            }
        }
        for (auto i = size_t{0}; i < width; ++i) {
            auto curr = i * (height + 1);
            auto next = (i + 1) * (height + 1);
            for (auto j = size_t{0}; j < height; ++j) {
                mesh_template.indices.push_back(curr + j);
                mesh_template.indices.push_back(next + j);
                mesh_template.indices.push_back(next + j + 1);
                mesh_template.indices.push_back(next + j + 1);
                mesh_template.indices.push_back(curr + j + 1);
                mesh_template.indices.push_back(curr + j);
            }
        }
        return mesh_template;
    }

    mesh::mesh_template_t make_circle(float radius, int tessellation) {
        mesh::mesh_template_t circle;
        float angle_inc = 2.0 * M_PI / tessellation;
        for (int i = 0; i <= tessellation; ++i) {
            float angle = i * angle_inc;
            float x = radius * std::cos(angle);
            float y = radius * std::sin(angle);
            circle.positions.emplace_back(x, y, 0);
            float tx = (std::cos(angle) / 2.0f) + 0.5f;
            float ty = (std::sin(angle) / 2.0f) + 0.5f;
            circle.tex_coords.emplace_back(tx, ty);
        }
        circle.positions.emplace_back(0, 0, 0);
        circle.tex_coords.emplace_back(0.5f, 0.5f);
        for (auto i = size_t{0}; i < tessellation; ++i) {
            circle.indices.push_back(circle.positions.size() - 1);
            circle.indices.push_back(i);
            circle.indices.push_back(i + 1);
        }
        return circle;
    }

    mesh::mesh_template_t make_cylinder(float radius, float length, int tessellation) {
        mesh::mesh_template_t cylinder;
        float angle_inc = 2.0 * M_PI / tessellation;
        for (int i = 0; i <= tessellation; ++i) {
            float angle = i * angle_inc;
            float x = radius * std::cos(angle);
            float y = radius * std::sin(angle);
            cylinder.positions.emplace_back(x, y, -length / 2.0f);
            float tx = (std::cos(angle) / 2.0f) + 0.5f;
            float ty = (std::sin(angle) / 2.0f) + 0.5f;
            cylinder.tex_coords.emplace_back(tx, ty);
        }
        for (int i = 0; i <= tessellation; ++i) {
            float angle = i * angle_inc;
            float x = radius * std::cos(angle);
            float y = radius * std::sin(angle);
            cylinder.positions.emplace_back(x, y, length / 2.0f);
            float tx = (std::cos(angle) / 2.0f) + 0.5f;
            float ty = (std::sin(angle) / 2.0f) + 0.5f;
            cylinder.tex_coords.emplace_back(tx, ty);
        }
        int curr = tessellation + 1;
        int prev = 0;
        for (auto i = size_t{0}; i < tessellation; ++i) {
            cylinder.indices.push_back(curr + i);
            cylinder.indices.push_back(prev + i);
            cylinder.indices.push_back(prev + i + 1);
            cylinder.indices.push_back(prev + i + 1);
            cylinder.indices.push_back(curr + i + 1);
            cylinder.indices.push_back(curr + i);
        }
        return cylinder;
    }

    mesh::mesh_template_t make_ndc_cube() {
        mesh::mesh_template_t cube;
    }

} // namespace shapes
