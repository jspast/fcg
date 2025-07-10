#include <glm/vec4.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include "matrices.hpp"
#include "collisions.hpp"
#include "math_constants.hpp"

// Método baseado em: https://antongerdelan.net/opengl/raycasting.html
glm::vec4 cursor_to_ray(glm::vec2 cursor_pos,
                        glm::vec2 window_size,
                        glm::mat4 projection_matrix,
                        glm::mat4 view_matrix)
{
    glm::vec4 ray((2.0f * cursor_pos.x) / window_size.x - 1.0f,
                   1.0f - (2.0f * cursor_pos.y) / window_size.y,
                   -1.0f,
                   1.0f);

    ray = glm::inverse(projection_matrix) * ray;
    ray.z = -1.0f;
    ray.w = 0.0f;

    ray = glm::inverse(view_matrix) * ray;

    ray = ray / norm(ray);

    return ray;
}

// Slide 11 Aula 24
glm::vec4 ray_plane_intersection(glm::vec4 ray_position,
                                 glm::vec4 ray_direction,
                                 glm::vec4 plane_point,
                                 glm::vec4 plane_normal)
{
    glm::vec4 a = ray_position;
    glm::vec4 c = plane_point;
    glm::vec4 n = plane_normal;

    float t = dotproduct((c - a), n) / dotproduct(ray_direction, n);

    return a + t * ray_direction;
}

// For the following functions, "pos" considered the center of the object

// Returns the ammount of sphere_mov that can happen before a collision
// Examples:
// 1.0 -> No collision in the movement, can happen integrally
// 0.5 -> Collision in the exact middle of the movement, can only do half
// 0.0 -> Collision on movement start
float sphere_aabbs_intersection_with_movement(glm::vec4 sphere_pos,
                                              float sphere_radius,
                                              glm::vec4 sphere_mov,
                                              const std::vector<std::pair<glm::vec4, AABB>>& aabbs)
{
    glm::vec3 end_pos = glm::vec3(sphere_pos) + glm::vec3(sphere_mov);

    float move_ammount = 1.0f;

    for (const auto& [aabb_pos, aabb] : aabbs) {
        glm::vec3 aabb_min = aabb.min + glm::vec3(aabb_pos) - glm::vec3(sphere_radius);
        glm::vec3 aabb_max = aabb.max + glm::vec3(aabb_pos) + glm::vec3(sphere_radius);

        // Allow movement if it ends outside aabb
        // Avoids getting stuck
        if (end_pos.x < aabb_min.x || end_pos.x > aabb_max.x ||
            end_pos.y < aabb_min.y || end_pos.y > aabb_max.y ||
            end_pos.z < aabb_min.z || end_pos.z > aabb_max.z) {
            continue;
        }

        float tmin = 0.0f;
        bool hit = true;

        for (int i = 0; i < 3; i++) {
            // Avoid division by zero
            if (glm::abs(sphere_mov[i]) < EPSILON) {
                if (sphere_pos[i] < aabb_min[i] || sphere_pos[i] > aabb_max[i]) {
                    hit = false;
                    break;
                }
            }
            else {
                float inv_dir = 1.0f / sphere_mov[i];
                // Test both min and max limits
                float t1 = (aabb_min[i] - sphere_pos[i]) * inv_dir;
                float t2 = (aabb_max[i] - sphere_pos[i]) * inv_dir;

                tmin = glm::max(tmin, glm::min(t1, t2));
            }
        }

        if (hit && tmin < move_ammount) {
            move_ammount = tmin;
        }
    }

    return glm::clamp(move_ammount, 0.0f, 1.0f);
}

bool aabb_aabb_intersection(glm::vec4 pos1,
                            AABB aabb1,
                            glm::vec4 pos2,
                            AABB aabb2)
{
    glm::vec3 a_min = glm::vec3(pos1) + aabb1.min;
    glm::vec3 a_max = glm::vec3(pos1) + aabb1.max;

    glm::vec3 b_min = glm::vec3(pos2) + aabb2.min;
    glm::vec3 b_max = glm::vec3(pos2) + aabb2.max;

    bool collision_x = a_min.x <= b_max.x && a_max.x >= b_min.x;
    bool collision_y = a_min.y <= b_max.y && a_max.y >= b_min.y;
    bool collision_z = a_min.z <= b_max.z && a_max.z >= b_min.z;

    return collision_x && collision_y && collision_z;
    // return collision.x && collision.y && collision.z;
}
