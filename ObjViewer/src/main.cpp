#include <stdio.h>
#include <math.h>

#include <glad\glad.h> 
#include <GLFW\glfw3.h>
#include "stb_image.h"

#include "log.h"
#include "shader.h"


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
    LOG_E("GLFW error #%d: %s", error_code, error_str);
}


bool draw_wireframe = false;
float offset = 0.0f;
int up_or_down = 1;
float mix_value = 0.2;

void process_input(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        draw_wireframe = !draw_wireframe;
    }
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        offset += 0.05f;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        offset -= 0.05f;
    }
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        mix_value = mix_value < 1.0f ? (mix_value + 0.005f) : 1.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        mix_value = mix_value > 0.0f ? (mix_value - 0.005f) : 0.0f;
    }
}

int main(void)
{
    init_logger();

    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(800, 600, "ObjViewer", NULL, NULL);

    if (!window)
    {
        LOG_E("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    int glad_load_success = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    if (!glad_load_success)
    {
        LOG_E("Failed to initialize GLAD");
        return -1;
    }

    glfwSetWindowContentScaleCallback(window, window_content_scale_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetErrorCallback(error_callback);

    int nr_attributes = 0;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nr_attributes);
    //LOG_I("num attributes shader: %d\n", nr_attributes);

    glViewport(0, 0, 800, 600);
    
    Shader shader;
    init(&shader, "shader\\vertex_shader.vert", "shader\\fragment_shader.frag");
    
    float vertices[] = {
        // positions         // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left  
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

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // --- TEXTURE ---

    u32 texture_container, texture_awesomeface;
    glGenTextures(1, &texture_container);
    glBindTexture(GL_TEXTURE_2D, texture_container);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    s32 width, height, n_channels;
    const char *texture_filename = "texture\\container.jpg";
    stbi_set_flip_vertically_on_load(true);
    u8 *data = stbi_load(texture_filename, &width, &height, &n_channels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else
    {
        LOG_E("Cannot load texture '%s'", texture_filename);
    }


    glGenTextures(1, &texture_awesomeface);
    glBindTexture(GL_TEXTURE_2D, texture_awesomeface);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    texture_filename = "texture\\awesomeface_alpha.png";
    data = stbi_load(texture_filename, &width, &height, &n_channels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else
    {
        LOG_E("Cannot load texture '%s'", texture_filename);
    }

    // --- TEXTURE ---

    use(&shader);
    set_int(&shader, "texture_container", 0);
    set_int(&shader, "texture_awesomeface", 1);

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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_container);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_awesomeface);
        
        use(&shader);
        //set_float(&shader, "offset", offset);
        //set_int(&shader, "up_or_down", up_or_down);
        set_float(&shader, "mix_value", mix_value);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();

        Sleep(10);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}