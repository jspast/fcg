
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include "animation.hpp"

AnimationCubicBezier::AnimationCubicBezier(glm::vec4 cp1, glm::vec4 cp2, glm::vec4 cp3, glm::vec4 cp4) {
    control_point_1 = cp1;
    control_point_2 = cp2;
    control_point_3 = cp3;
    control_point_4 = cp4;
}

void AnimationCubicBezier::set_total_time(float seconds)
{   
    total_time = seconds;
}

glm::vec4 AnimationCubicBezier::get_point_bezier(float t_raw)
{
    glm::vec4 control_points[4];
    control_points[0] = control_point_1;
    control_points[1] = control_point_2;
    control_points[2] = control_point_3;
    control_points[3] = control_point_4;
    float t = t_raw / total_time;

    int list_lenght = 4;
    int current_degree = 3;
    glm::vec4 new_point;

    while (list_lenght > 1) {
        for (int i=0; i<current_degree; i++){
            new_point = control_points[i] + t*(control_points[i+1] - control_points[i]);
            control_points[i] = new_point;
        }
        list_lenght -= 1;
        current_degree -= 1;
    }

    return control_points[0];
}

glm::vec4 AnimationCubicBezier::get_point_for_object(float delta_t)
{
    time_passed += delta_t;
    if (time_passed > total_time) {
        time_passed = total_time;
    }
    return get_point_bezier(time_passed);
}

bool AnimationCubicBezier::is_animation_over() 
{
    return (time_passed >= total_time);
}

AnimationCamera::AnimationCamera(float s_phi, float t_phi, float s_theta, float t_theta) {
    target_phi = t_phi;
    target_theta = t_theta;
    starter_phi = s_phi;
    starter_theta = s_theta;
}

void AnimationCamera::set_total_time(float seconds)
{   
    total_time = seconds;
}

std::pair<float, float> AnimationCamera::get_angles_for_camera(float delta_t) 
{
    time_passed += delta_t;
    if (time_passed > total_time) {
        time_passed = total_time;
    }
    float new_phi = (target_phi - starter_phi) * time_passed / total_time;
    float new_theta = (target_theta - starter_theta) * time_passed / total_time;

    return {new_phi, new_theta};
}

bool AnimationCamera::is_animation_over() 
{
    return (time_passed >= total_time);
}