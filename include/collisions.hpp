#pragma once

#include <glm/vec4.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

glm::vec4 cursor_to_ray(glm::vec2 cursor_pos,
                        glm::vec2 window_size,
                        glm::mat4 projection_matrix,
                        glm::mat4 view_matrix);

glm::vec4 ray_plane_intersection(glm::vec4 ray_position,
                             glm::vec4 ray_direction,
                             glm::vec4 plane_point,
                             glm::vec4 plane_normal);
