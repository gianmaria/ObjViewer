#include "camera.h"

void update(Camera *cam);

Camera init(const glm::vec3 &position, const glm::vec3 &up, float yaw, float pitch)
{
    Camera cam;

    cam.position = position;

    cam.front = glm::vec3(0.0f, 0.0f, -1.0f);
    
    cam.world_up = up;

    cam.yaw = yaw;
    cam.pitch = pitch;

    cam.fov = 45.0f;
    cam.speed = 5.5f;

    cam.flying = true;

    update(&cam);

    return cam;
}

glm::mat4 get_view_matrix(Camera *cam)
{
    glm::vec3 center = cam->position + cam->front; // target ?

    glm::mat4 view_matrix = glm::lookAt(cam->position, // eye
                                        center,        // center
                                        cam->up);      // up

    return view_matrix;
}

void move(Camera *cam, const glm::vec3 &new_pos, float dt)
{
    cam->prev_y_pos = cam->position.y;

    float velocity = cam->speed * dt;

#if 1
    cam->position += cam->right * velocity * new_pos.x;
    cam->position += cam->front * velocity * new_pos.z;
#else
    if (new_pos.x != 0.0f)
    {
        if (new_pos.x > 0.0f)
        {
            cam->position += cam->right * velocity;
        }
        else
        {
            cam->position -= cam->right * velocity;
        }
    }
    else if (new_pos.z != 0.0f)
    {
        if (new_pos.z > 0.0f)
        {
            cam->position += cam->front * velocity;
        }
        else
        {
            cam->position -= cam->front * velocity;
        }
    }
#endif

    if (!cam->flying) cam->position.y = cam->prev_y_pos;
}

void rotate(Camera *cam, float yaw, float pitch)
{
    cam->yaw   += yaw;
    cam->pitch += pitch;

    if (cam->pitch > 89.0f) cam->pitch = 89.0f;
    else if (cam->pitch < -89.0f) cam->pitch = -89.0f;

    update(cam);
}

void zoom(Camera *cam, float zoom)
{
    cam->fov -= zoom;

    if (cam->fov > 45.0f) cam->fov = 45.0f;
    else if (cam->fov < 1.0f) cam->fov = 1.0f;
}

void update(Camera *cam)
{
    glm::vec3 new_front;
    new_front.x = glm::cos(glm::radians(cam->yaw)) * glm::cos(glm::radians(cam->pitch));
    new_front.y = glm::sin(glm::radians(cam->pitch));
    new_front.z = glm::sin(glm::radians(cam->yaw)) * glm::cos(glm::radians(cam->pitch));

    cam->front = glm::normalize(new_front);
    cam->right = glm::normalize(glm::cross(cam->world_up, cam->front));
    cam->up    = glm::normalize(glm::cross(cam->right, cam->front));
}