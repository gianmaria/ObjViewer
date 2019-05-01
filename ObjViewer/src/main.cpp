#include <stdio.h>
#include <math.h>

#include <glad\glad.h> 

#include <GLFW\glfw3.h>

#include "stb_image.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "log.h"
#include "shader.h"
#include "camera.h"

#define ArrayCount(a) (sizeof(a) / sizeof(a[0]))

u32 screen_width = 800;
u32 screen_height = 600;

bool draw_wireframe = false;

float delta_time;
float last_frame;

Camera cam;
float mouse_last_x = screen_width / 2.0f;
float mouse_last_y = screen_height / 2.0f;

float mouse_sensitivity = 0.05f;


void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void window_size_callback(GLFWwindow *window, int width, int height);

void window_content_scale_callback(GLFWwindow* window, float xscale, float yscale);

void error_callback(int error_code, const char *error_str);

void process_input(GLFWwindow *window);

void mouse_callback(GLFWwindow *window, double mouse_x, double mouse_y);

void scroll_callback(GLFWwindow *window, double x_offset, double y_offset);



int main(void)
{
    init_logger();

    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(screen_width, screen_height, "ObjViewer", NULL, NULL);

    if (!window)
    {
        LOG_E("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    int glad_load = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    if (!glad_load)
    {
        LOG_E("Failed to initialize GLAD");
        return -1;
    }

    glfwSetWindowContentScaleCallback(window, window_content_scale_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetErrorCallback(error_callback);

    int nr_attributes = 0;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nr_attributes);

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, 800, 600);
    
    cam = init(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

    Shader shader;
    init(&shader, "shader\\vertex_shader.vert", "shader\\fragment_shader.frag");
    
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 2.0f,
        0.5f,  0.5f, -0.5f,   2.0f, 2.0f,
        0.5f,  0.5f,  0.5f,   2.0f, 0.0f,
        0.5f,  0.5f,  0.5f,   2.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 2.0f
    };

    glm::vec3 cube_positions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };
        
    unsigned int cube_VAO, cube_VBO;
    glGenVertexArrays(1, &cube_VAO);
    glGenBuffers(1, &cube_VBO);
    
    glBindVertexArray(cube_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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


    use(&shader);
    set_int(&shader, "texture_container", 0);
    set_int(&shader, "texture_awesomeface", 1);

    // --- TEXTURE ---


    //glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 3.0f);
    //glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f);
    //glm::vec3 camera_direction = glm::normalize(camera_pos - camera_target); // Z axis
    //glm::vec3 camera_right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), camera_direction)); // X axis
    //glm::vec3 camera_up = glm::cross(camera_direction, camera_up); // Y axis



    while (!glfwWindowShouldClose(window))
    {
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        fprintf(stderr, "elapsed: %.3fs  dt: %.4f  ms/frame: %.4f  FPS: %.1f"
                "  Flying cam: %3s"
                "  Cam.pos: [%.3f %.3f %.3f]  Cam.up: [%.3f %.3f %.3f] \r", 
               current_frame, 
               delta_time,
               delta_time * 1000.0f,
               1.0f / delta_time,
                cam.flying ? "ON" : "OFF",
                (float)cam.position.x, (float)cam.position.y, (float)cam.position.z,
                (float)cam.up.x,(float)cam.up.y,(float)cam.up.z);

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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_container);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_awesomeface);




        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(cam.fov),
                                      (float)screen_width / (float)screen_height,
                                      0.1f, 100.0f);

        set_mat4(&shader, "view", get_view_matrix(&cam));
        set_mat4(&shader, "projection", projection);

        glBindVertexArray(cube_VAO);
        for (u32 index = 0; 
             index < ArrayCount(cube_positions);
             ++index)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cube_positions[index]);
            float angle = glfwGetTime();
            if (index % 2 == 0) angle = -angle;
            model = glm::rotate(model, glm::radians(angle*20.0f), glm::vec3(1.0f, 0.3f, 0.5f));

            set_mat4(&shader, "model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        Sleep(10);
    }

    glDeleteVertexArrays(1, &cube_VAO);
    glDeleteBuffers(1, &cube_VBO);

    glfwTerminate();
    return 0;
}


void process_input(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
    {
        static double last_time = 0.0;
        double current_time = glfwGetTime();
        if ((current_time - last_time) > 0.05)
        {
            draw_wireframe = !draw_wireframe;
        }
        last_time = current_time;

    }
    
    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
    {
        static double last_time = 0.0;

        double current_time = glfwGetTime();
        if ((current_time - last_time) > 0.05)
        {
            cam.flying = !cam.flying;
        }
        last_time = current_time;
    }

    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
    {
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
    {
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        move(&cam, glm::vec3(0.0f, 0.0f, 1.0f), delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        move(&cam, glm::vec3(0.0f, 0.0f, -1.0f), delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        move(&cam, glm::vec3(-1.0f, 0.0f, 0.0f), delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        move(&cam, glm::vec3(1.0f, 0.0f, 0.0f), delta_time);
    }
}

void mouse_callback(GLFWwindow *window, double mouse_x, double mouse_y)
{
    static bool first_mouse = true;
    if (first_mouse) // to avoid big jump on first callback
    {
        mouse_last_x = mouse_x;
        mouse_last_y = mouse_y;
        first_mouse = false;
    }

    float mouse_x_offset = mouse_x - mouse_last_x;
    float mouse_y_offset = mouse_last_y - mouse_y;

    mouse_last_x = mouse_x;
    mouse_last_y = mouse_y;
    
    mouse_x_offset *= mouse_sensitivity;
    mouse_y_offset *= mouse_sensitivity;

    rotate(&cam, -mouse_x_offset, -mouse_y_offset);
}

void scroll_callback(GLFWwindow *window, double x_offset, double y_offset)
{
    zoom(&cam, y_offset);
}




void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    printf("Framebuffer W: %d, H: %d\n", width, height);
    screen_width = width;
    screen_height = height;
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