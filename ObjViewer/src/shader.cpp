#include <stdio.h>
#include <stdlib.h>

#include <glad\glad.h>

#include "shader.h"
#include "log.h"

struct FileContent
{
    void *data;
    u64 size;
};

local FileContent 
read_entire_file_in_memory_and_zero_terminate(const char* filename, bool zero_terminate)
{
    FileContent fc = {0};

    FILE *file = fopen(filename, "rb");

    if (file)
    {
        fseek(file, 0, SEEK_END);
        fc.size = ftell(file);
        rewind(file);

        if (zero_terminate)
        {
            ++fc.size;
        }

        fc.data = calloc(1, fc.size);

        fread(fc.data, 1, fc.size, file);

        fclose(file);
    }
    else
    {
        LOG_E("Cannot open '%s' for reading", filename);
    }

    return fc;
}

local void
delete_file_content(FileContent *fc)
{
    if (fc &&
        fc->data)
    {
        free(fc->data);
        fc->data = nullptr;

        fc->size = 0;
    }
}

s32 init(Shader *shader, const char *vertex_shader_filename, const char *fragment_shader_filename)
{
    FileContent vertex_shader_file_content = 
        read_entire_file_in_memory_and_zero_terminate(vertex_shader_filename, true);

    const char *vertex_shader_source = (const char *)vertex_shader_file_content.data;

    u32 vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    s32 success;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        char buffer[1024] = {};
        glGetShaderInfoLog(vertex_shader, sizeof(buffer), NULL, buffer);

        LOG_E("[Vertex Shader] %s", buffer);
        return -1;
    }
    
    FileContent fragment_shader_file_content = 
        read_entire_file_in_memory_and_zero_terminate(fragment_shader_filename, true);

    const char *fragment_shader_source = (const char *)fragment_shader_file_content.data;
    u32 fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        char buffer[1024] = {};
        glGetShaderInfoLog(fragment_shader, sizeof(buffer), NULL, buffer);

        LOG_E("[Fragment Shader] %s", buffer);
        return -1;
    }

    s32 shader_program;
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        char buffer[1024] = {};
        glGetProgramInfoLog(shader_program, sizeof(buffer), NULL, buffer);

        LOG_E("[Shader Program] %s", buffer);
        return -1;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    delete_file_content(&vertex_shader_file_content);
    delete_file_content(&fragment_shader_file_content);

    shader->ID = shader_program;

    return 0;
}


void use(Shader *shader)
{
    glUseProgram(shader->ID);
}

void set_bool(Shader *shader, const char *name, bool value)
{
    glUniform1i(glGetUniformLocation(shader->ID, name), (s32)value);
}

void set_int(Shader *shader, const char *name, int value)
{
    glUniform1i(glGetUniformLocation(shader->ID, name), value);
}

void set_float(Shader *shader, const char *name, float value)
{
    glUniform1f(glGetUniformLocation(shader->ID, name), value);
}
