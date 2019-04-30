#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "types.h"

struct Shader
{
    u32 ID;
};

s32 init(Shader *shader, const char *vertex_shader_filename, const char *fragment_shader_filename);

void use(Shader *shader);

void set_bool(Shader *shader, const char *name, bool value);
void set_int(Shader *shader, const char *name, int value);
void set_float(Shader *shader, const char *name, float value);
void set_mat4(Shader *shader, const char *name, const glm::mat4 &value);

