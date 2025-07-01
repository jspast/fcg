#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <memory>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "matrices.hpp"

#define EPSILON (std::numeric_limits<float>::epsilon())
#define SQRT_EPSILON (0.000345)

template<typename Base, typename T>
inline bool instanceof(const T *ptr);

// Abstract class that defines a camera
class Camera {
    protected:
        // The up vector is fixed to the global y
        glm::vec4 up_vector = glm::vec4(0.0f,1.0f,0.0f,0.0f);

        // Phi is limited to (-pi/2, +pi/2) to prevent camera flips
        constexpr static const float phimax = M_PI_2 - 4 * SQRT_EPSILON;
        constexpr static const float phimin = -phimax;

        // The camera position
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        // The camera angles
        float theta = 0.0f;
        float phi   = 0.0f;

        // Used to define the frustum
        float nearplane_distance = 0.01f;
        float farplane_distance  = 100.0f;

        // Perspective projection is used by default
        bool use_perspective_projection = true;

        float fov          = M_PI / 3.0f;
        float aspect_ratio = 1.0f;

        // Zoom is simulated in orthographic projection by changing the size of the projection plane
        float orthographic_zoom     = 1.5f;
        float orthographic_zoom_min = 1.1f;
        float orthographic_zoom_max = 10.0f;

        // Function called after a theta or phi change
        // Designed to be used when an update in other camera atributes is necessary
        virtual void update() = 0;

    public:
        // Returns the unnormalized view vector
        virtual glm::vec4 get_view_vector() = 0;

        glm::mat4 get_view_matrix();
        glm::mat4 get_projection_matrix();

        void set_position(float x, float y, float z);
        void set_position(glm::vec4 position);
        glm::vec4 get_position();

        // Adds theta_increment to theta and phi_increment to phi
        // Should be used when both angles are updated at the same time to avoid unnecessary updates
        void adjust_angles(float theta_increment, float phi_increment);

        // Should be used when both angles are updated at the same time to avoid unnecessary updates
        void set_angles(float theta, float phi);

        // Adds increment to theta
        void adjust_theta(float increment);

        // Adds increment to phi
        void adjust_phi(float increment);

        void set_theta(float theta);
        void set_phi(float phi);
        float get_theta();
        float get_phi();

        void set_nearplane_distance(float distance);
        void set_farplane_distance(float distance);
        float get_nearplane_distance();
        float get_farplane_distance();

        // When boolean is true, perspective projection is used
        // When boolean is false, orthographic projection is used
        void toggle_perspective_projection(bool boolean);

        bool is_projection_perspective();

        void set_fov(float fov);
        float get_fov();

        void set_aspect_ratio(float aspect_ratio);
        float get_aspect_ratio();

        // Adds increment to orthographic zoom
        void adjust_orthographic_zoom(float increment);

        void set_orthographic_zoom(float zoom);
        float get_orthographic_zoom();

        void set_orthographic_zoom_min(float min);
        float get_orthographic_zoom_min();

        void set_orthographic_zoom_max(float max);
        float get_orthographic_zoom_max();
};

// A Look-At implementation of a camera
class LookAtCamera: public Camera {
    protected:
        // The camera distance relative to the target position
        // Should never be 0, which would cause division by zero problems
        float distance = 3.0f;

        // The target point position
        float target_x = 1.0f;
        float target_y = 0.0f;
        float target_z = 0.0f;

        void update();

    public:
        void set_target_position(float x, float y, float z);
        void set_target_position(glm::vec4 position);
        glm::vec4 get_target_position();

        glm::vec4 get_view_vector();

        // Adds increment to distance
        void adjust_distance(float increment);

        void set_distance(float distance);
        float get_distance();
};

// An implementation of a free camera
class FreeCamera: public Camera {
    protected:
        // Vectors used to define the camera direction
        glm::vec4 w_vector = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec4 u_vector = crossproduct(up_vector, w_vector) / norm(crossproduct(up_vector, w_vector));
        glm::vec4 v_vector = crossproduct(w_vector, u_vector);

        void update();

    public:
        glm::vec4 get_view_vector();

        // Moves the camera relative to the view_vector
        void move(float forward_displacement, float left_displacement);
};

std::shared_ptr<FreeCamera> build_free_camera(std::shared_ptr<Camera> camera);

std::shared_ptr<LookAtCamera> build_lookat_camera(std::shared_ptr<Camera> camera, float distance = 1.0f);
std::shared_ptr<LookAtCamera> build_lookat_camera(std::shared_ptr<Camera> camera, glm::vec4 target_position, float distance);
