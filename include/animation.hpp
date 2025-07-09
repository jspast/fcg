#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

class AnimationCubicBezier {
    private:
        glm::vec4 control_point_1;
        glm::vec4 control_point_2;
        glm::vec4 control_point_3;
        glm::vec4 control_point_4;

        float total_time = 1.0;
        float time_passed = 0.0;

    public:
        AnimationCubicBezier(glm::vec4 cp1, glm::vec4 cp2, glm::vec4 cp3, glm::vec4 cp4);
        void set_total_time(float seconds);
        glm::vec4 get_point_bezier(float t_raw);
        glm::vec4 get_point_for_object(float delta_t);
        bool is_animation_over();
};

class AnimationCamera {
    private:
        float target_phi;
        float target_theta;
        float starter_phi;
        float starter_theta;
        float total_time = 1.0;
        float time_passed = 0.0;

    public:
        AnimationCamera(float s_phi, float t_phi, float s_theta, float t_theta);
        void set_total_time(float seconds);
        std::pair<float, float> get_angles_for_camera(float delta_t);
        bool is_animation_over();
};
        

