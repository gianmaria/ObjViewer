#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera
{
    glm::vec3 position;

    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    
    glm::vec3 world_up;

    float yaw;
    float pitch;
    
    float fov;
    float speed;

    bool flying;
    float prev_y_pos;
};

Camera init(const glm::vec3 &position, const glm::vec3 &up, float yaw, float pitch);

glm::mat4 get_view_matrix(Camera *cam);

void move(Camera *cam, const glm::vec3 &new_pos, float dt);

void rotate(Camera *cam, float yaw, float pitch);

void zoom(Camera *cam, float zoom);

