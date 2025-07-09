#pragma once

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

class AABB {
    public:
        glm::vec3 min = glm::vec3(INFINITY);
        glm::vec3 max = glm::vec3(-INFINITY);

        AABB() = default;
        AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

        // FONTE: ChatGPT
        // Overload de operação de multiplicação para AABB
        AABB operator*(float scalar) const {
            return AABB(min * scalar, max * scalar);
        }
};

glm::vec4 cursor_to_ray(glm::vec2 cursor_pos,
                        glm::vec2 window_size,
                        glm::mat4 projection_matrix,
                        glm::mat4 view_matrix);

glm::vec4 ray_plane_intersection(glm::vec4 ray_position,
                                 glm::vec4 ray_direction,
                                 glm::vec4 plane_point,
                                 glm::vec4 plane_normal);

float sphere_aabbs_intersection_with_movement(glm::vec4 sphere_pos,
                                              float sphere_radius,
                                              glm::vec4 sphere_mov,
                                              const std::vector<std::pair<glm::vec4, AABB>>& aabbs);

bool aabb_aabb_intersection(glm::vec4 pos1,
                            AABB aabb1,
                            glm::vec4 pos2,
                            AABB aabb2);
