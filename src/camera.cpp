#include "camera.hpp"

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
    return dynamic_cast<const Base*>(ptr) != nullptr;
}

void Camera::set_position(float nx, float ny, float nz)
{
    x = nx;
    y = ny;
    z = nz;
}

void Camera::set_position(glm::vec4 p)
{
    x = p.x;
    y = p.y;
    z = p.z;
}

glm::vec4 Camera::get_position()
{
    return glm::vec4(x, y, z, 1.0f);
}

glm::mat4 Camera::get_view_matrix()
{
    return Matrix_Camera_View(get_position(), get_view_vector(), up_vector);
}

glm::mat4 Camera::get_projection_matrix()
{
    glm::mat4 projection;

    if (use_perspective_projection)
    {
        projection = Matrix_Perspective(fov, aspect_ratio, -nearplane_distance, -farplane_distance);
    }
    else
    {
        // Simulates zoom by changing the size of the projection plane
        float t = 1.5f / log2(orthographic_zoom);
        float b = -t;
        float r = t * aspect_ratio;
        float l = -r;
        projection = Matrix_Orthographic(l, r, b, t, -nearplane_distance, -farplane_distance);
    }

    return projection;
}

void Camera::adjust_angles(float ti, float pi)
{
    theta += ti;
    phi = glm::clamp(phi + pi, phimin, phimax);
    update();
}

void Camera::set_angles(float t, float p)
{
    theta = t;
    phi   = glm::clamp(p, phimin, phimax);
    update();
}

void Camera::adjust_theta(float increment)
{
    theta += increment;
    update();
}

void Camera::set_theta(float t)
{
    theta = t;
    update();
}

void Camera::adjust_phi(float increment)
{
    phi = glm::clamp(phi + increment, phimin, phimax);
    update();
}

void Camera::set_phi(float p)
{
    phi = glm::clamp(p, phimin, phimax);
    update();
}

float Camera::get_theta(){
    return theta;
}

float Camera::get_phi()
{
    return phi;
}

void Camera::set_nearplane_distance(float d)
{
    nearplane_distance = d;
}

void Camera::set_farplane_distance(float d)
{
    farplane_distance = d;
}

float Camera::get_nearplane_distance()
{
    return nearplane_distance;
}

float Camera::get_farplane_distance()
{
    return farplane_distance;
}

void Camera::toggle_perspective_projection(bool b)
{
    use_perspective_projection = b;
}

bool Camera::is_projection_perspective()
{
    return use_perspective_projection;
}

void Camera::set_fov(float f)
{
    fov = f;
}

float Camera::get_fov()
{
    return fov;
}

void Camera::set_aspect_ratio(float a)
{
    aspect_ratio = a;
}

float Camera::get_aspect_ratio()
{
    return aspect_ratio;
}

void Camera::adjust_orthographic_zoom(float z)
{
    orthographic_zoom = glm::clamp(orthographic_zoom + z, orthographic_zoom_min, orthographic_zoom_max);
}

void Camera::set_orthographic_zoom(float z)
{
    orthographic_zoom = glm::clamp(z, orthographic_zoom_min, orthographic_zoom_max);
}

float Camera::get_orthographic_zoom()
{
    return orthographic_zoom;
}

void Camera::set_orthographic_zoom_min(float m)
{
    orthographic_zoom_min = m;
}

float Camera::get_orthographic_zoom_min()
{
    return orthographic_zoom_min;
}

void Camera::set_orthographic_zoom_max(float m)
{
    orthographic_zoom_max = m;
}

float Camera::get_orthographic_zoom_max()
{
    return orthographic_zoom_max;
}

// Update camera position based on the camera angles
void LookAtCamera::update()
{
    y = distance * sinf(phi) + target_y;
    z = distance * cosf(phi) * cosf(theta) + target_z;
    x = distance * cosf(phi) * sinf(theta) + target_x;
}

void LookAtCamera::set_target_position(float nx, float ny, float nz)
{
    target_x = nx;
    target_y = ny;
    target_z = nz;
}

void LookAtCamera::set_target_position(glm::vec4 p)
{
    target_x = p.x;
    target_y = p.y;
    target_z = p.z;
}

glm::vec4 LookAtCamera::get_target_position()
{
    return glm::vec4(target_x, target_y, target_z, 1.0f);
}

// Computes the view vector based on the camera and target positions
glm::vec4 LookAtCamera::get_view_vector()
{
    return get_target_position() - get_position();
}

void LookAtCamera::adjust_distance(float increment)
{
    // Avoids target and camera positions being the same, which would cause division by zero problems
    distance = fmax(SQRT_EPSILON, distance + increment);
    update();
}

void LookAtCamera::set_distance(float d)
{
    // Avoids target and camera positions being the same, which would cause division by zero problems
    distance = fmax(SQRT_EPSILON, d);
    update();
}

float LookAtCamera::get_distance()
{
    return distance;
}

// Update camera vectors based on the camera angles
void FreeCamera::update()
{
    w_vector = get_view_vector() / norm(get_view_vector());
    u_vector = crossproduct(up_vector, w_vector) / norm(crossproduct(up_vector, w_vector));
    v_vector = crossproduct(w_vector, u_vector);
}

// Computes the view vector based on the camera angles
glm::vec4 FreeCamera::get_view_vector()
{
    return glm::vec4(cos(phi) * sin(theta), -sin(phi), cos(phi) * cos(theta), 0.0f);
}

void FreeCamera::move(float forward_displacement, float left_displacement)
{
    set_position(get_position() + w_vector * forward_displacement + u_vector * left_displacement);
}

FreeCamera build_free_camera(Camera* camera)
{
    // Do nothing if camera is already a FreeCamera
    if (instanceof<FreeCamera>(camera))
        return *static_cast<FreeCamera*>(camera);

    FreeCamera fc;

    // Update camera angles
    fc.set_angles(camera->get_theta() + M_PIf, camera->get_phi());

    // Copy other camera properties
    fc.set_position(camera->get_position());
    fc.set_nearplane_distance(camera->get_nearplane_distance());
    fc.set_farplane_distance(camera->get_farplane_distance());
    fc.toggle_perspective_projection(camera->is_projection_perspective());
    fc.set_aspect_ratio(camera->get_aspect_ratio());
    fc.set_fov(camera->get_fov());
    fc.set_orthographic_zoom(camera->get_orthographic_zoom());
    fc.set_orthographic_zoom_min(camera->get_orthographic_zoom_min());
    fc.set_orthographic_zoom_max(camera->get_orthographic_zoom_max());

    return fc;
}

LookAtCamera build_lookat_camera(Camera *camera, float distance)
{
    // Compute the target position keeping camera position and orientation based on the desired distance
    glm::vec4 target_position = camera->get_position() + distance * camera->get_view_vector() / norm(camera->get_view_vector());

    return build_lookat_camera(camera, target_position, distance);
}

LookAtCamera build_lookat_camera(Camera *camera, glm::vec4 target_position, float distance)
{
    // Do nothing if camera is already a LookAtCamera
    if (instanceof<LookAtCamera>(camera))
        return *static_cast<LookAtCamera*>(camera);

    LookAtCamera lc;

    // Set new camera properties
    lc.set_target_position(target_position);
    lc.set_distance(distance);

    // Update camera angles
    lc.set_angles(camera->get_theta() + M_PIf, camera->get_phi());

    // Copy other camera properties
    lc.set_position(camera->get_position());
    lc.set_nearplane_distance(camera->get_nearplane_distance());
    lc.set_farplane_distance(camera->get_farplane_distance());
    lc.toggle_perspective_projection(camera->is_projection_perspective());
    lc.set_aspect_ratio(camera->get_aspect_ratio());
    lc.set_fov(camera->get_fov());
    lc.set_orthographic_zoom(camera->get_orthographic_zoom());
    lc.set_orthographic_zoom_min(camera->get_orthographic_zoom_min());
    lc.set_orthographic_zoom_max(camera->get_orthographic_zoom_max());

    return lc;
}
