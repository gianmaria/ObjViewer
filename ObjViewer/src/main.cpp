#include <stdio.h>

#include <glad\glad.h> 
#include <GLFW\glfw3.h>

void log(const char* fmt, ...)
{
    static FILE *log_file;

    log_file = fopen("log.txt", "a");

    if (log_file)
    {
        char msg[4096] = {0};

        va_list args;
        va_start(args, fmt);

        vsnprintf(msg, sizeof(msg), fmt, args);

        va_end(args);
        
        fprintf(log_file, msg);
        fprintf(log_file, "\n");

        fclose(log_file);

        log_file = nullptr;
    }
}



void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    printf("Framebuffer W: %d, H: %d\n", width, height);

    glViewport(0, 0, width, height);
}

void window_size_callback(GLFWwindow *window, int width, int height)
{
    printf("Window W: %d, H: %d\n", width, height);
}

void window_content_scale_callback(GLFWwindow* window, float xscale, float yscale)
{
    printf("Content scale xscale: %f, yscale: %f\n", xscale, yscale);
}

void error_callback(int error_code, const char *error_str)
{
    log("GLFW error #%d: %s", error_code, error_str);
}

const char *vertex_shader_source = R"FOO(
#version 330 core
layout (location=0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)FOO";

const char *fragment_shader_source = R"FOO(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)FOO";

bool draw_wireframe = false;

void process_input(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        draw_wireframe = !draw_wireframe;
    }
}

int main(void)
{
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(800, 600, "ObjViewer", NULL, NULL);

    if (!window)
    {
        log("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    int glad_load_success = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    if (!glad_load_success)
    {
        log("Failed to initialize GLAD");
        return -1;
    }

    glViewport(0, 0, 800, 600);

    glfwSetWindowContentScaleCallback(window, window_content_scale_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetErrorCallback(error_callback);

    unsigned int vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    int success;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        char buffer[1024] = {};
        glGetShaderInfoLog(vertex_shader, sizeof(buffer), NULL, buffer);

        log(buffer);
        return -1;
    }

    unsigned int fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        char buffer[1024] = {};
        glGetShaderInfoLog(fragment_shader, sizeof(buffer), NULL, buffer);

        log(buffer);
        return -1;
    }

    unsigned int shader_program;
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        char buffer[1024] = {};
        glGetProgramInfoLog(shader_program, sizeof(buffer), NULL, buffer);

        log(buffer);
        return -1;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    float vertices[] = {
        0.5f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f,
       -0.5f,  0.5f, 0.0f
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    glEnableVertexAttribArray(0);
    
    // unbind the buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        process_input(window);

        if (draw_wireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();

        Sleep(50);
    }


    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();

    return 0;
}