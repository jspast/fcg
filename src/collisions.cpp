#include <glm/vec4.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include "matrices.hpp"

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
