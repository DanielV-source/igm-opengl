// Copyright (C) 2021 Emilio J. Padr�n
// Released as Free Software under the X11 License
// https://spdx.org/licenses/X11.html
//
// Strongly inspired by spinnycube.cpp in OpenGL Superbible
// https://github.com/openglsuperbible

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

// GLM library to deal with matrix operations
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::perspective
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <string>
#include <iostream>

int gl_width = 640;
int gl_height = 480;

void glfw_window_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void render(double);

GLuint shader_program = 0, shader_program_texture = 0; // shader program to set render pipeline
GLuint vao = 0, vao_texture = 0; // Vertext Array Object to set input data
GLint mv_location, proj_location; // Uniforms for transformation matrices
GLuint texture = 0;


int main() {
    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }

    //  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    //  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    //  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(gl_width, gl_height, "My spinning cube", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwSetWindowSizeCallback(window, glfw_window_size_callback);
    glfwMakeContextCurrent(window);

    // start GLEW extension handler
    // glewExperimental = GL_TRUE;
    glewInit();

    // get version info
    const GLubyte* vendor = glGetString(GL_VENDOR); // get vendor string
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* glversion = glGetString(GL_VERSION); // version as a string
    const GLubyte* glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION); // version as a string
    printf("Vendor: %s\n", vendor);
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", glversion);
    printf("GLSL version supported %s\n", glslversion);
    printf("Starting viewport: (width: %d, height: %d)\n", gl_width, gl_height);

    // Enable Depth test: only draw onto a pixel if fragment closer to viewer
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS); // set a smaller value as "closer"


    // FACE WITH TEXTURE
    const char* vertex_shader_texture =
        "#version 130\n"
        "in vec4 v_pos;"
        "in vec2 tex_coord;"
        "out vec2 vs_tex_coord;"
        "uniform mat4 mv_matrix;"
        "uniform mat4 proj_matrix;"
        "void main() {"
        "  gl_Position = proj_matrix * mv_matrix * v_pos;"
        "  vs_tex_coord = tex_coord;"
        "}";

    const char* fragment_shader_texture =
        "#version 130\n"
        "out vec4 frag_col;"
        "in vec2 vs_tex_coord;"
        "uniform sampler2D theTexture;"
        "void main() {"
        "  frag_col = texture(theTexture, vs_tex_coord);"
        "}";

    GLuint vs_texture = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs_texture, 1, &vertex_shader_texture, NULL);
    glCompileShader(vs_texture);
    GLuint fs_texture = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs_texture, 1, &fragment_shader_texture, NULL);
    glCompileShader(fs_texture);

    shader_program_texture = glCreateProgram();
    glAttachShader(shader_program_texture, fs_texture);
    glAttachShader(shader_program_texture, vs_texture);
    glLinkProgram(shader_program_texture);

    glDeleteShader(vs_texture);
    glDeleteShader(fs_texture);

    glGenVertexArrays(1, &vao_texture);
    glBindVertexArray(vao_texture);

    // Cube vertices
    const GLfloat points[] = {
      -0.25f, -0.25f, -0.25f,
      -0.25f,  0.25f, -0.25f,
       0.25f, -0.25f, -0.25f,
       0.25f,  0.25f, -0.25f,
       0.25f, -0.25f, -0.25f,
      -0.25f,  0.25f, -0.25f,
    };

    float texCoords[] = {
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f
    };

    GLuint vbo_texture[2];
    glGenBuffers(1, vbo_texture);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_texture[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_texture[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width = 0, height = 0, nrChannels = 3;
    stbi_set_flip_vertically_on_load(true);
    std::string imagePath = std::string(RESOURCES_DIR) + "/texture.png";

    
    unsigned char* data = stbi_load(imagePath.c_str(), &width, &height, &nrChannels, 0);
    if (data && width > 0 && height > 0) {
        if (nrChannels >= 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }
    else {
        printf("Failed to load texture\n");
    }

    stbi_image_free(data);


    // CUBE WITHOUT FACE
    // Vertex Shader
    const char* vertex_shader =
        "#version 130\n"

        "in vec4 v_pos;"

        "out vec4 vs_color;"

        "uniform mat4 mv_matrix;"
        "uniform mat4 proj_matrix;"

        "void main() {"
        "  gl_Position = proj_matrix * mv_matrix * v_pos;"
        "  vs_color = v_pos * 2.0 + vec4(0.4, 0.4, 0.4, 0.0);"
        "}";

    // Fragment Shader
    const char* fragment_shader =
        "#version 130\n"

        "out vec4 frag_col;"

        "in vec4 vs_color;"

        "void main() {"
        "  frag_col = vs_color;"
        "}";
    // Shaders compilation
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    // Create program, attach shaders to it and link it
    shader_program = glCreateProgram();
    glAttachShader(shader_program, fs);
    glAttachShader(shader_program, vs);
    glLinkProgram(shader_program);

    // Release shader objects
    glDeleteShader(vs);
    glDeleteShader(fs);

    // Vertex Array Object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    // Cube to be rendered
    //
    //          0        3
    //       7        4 <-- top-right-near
    // bottom
    // left
    // far ---> 1        2
    //       6        5
    //
    const GLfloat vertex_positions[] = {

       0.25f, -0.25f, -0.25f, // 2
       0.25f,  0.25f, -0.25f, // 3
       0.25f, -0.25f,  0.25f, // 5

       0.25f,  0.25f,  0.25f, // 4
       0.25f, -0.25f,  0.25f, // 5
       0.25f,  0.25f, -0.25f, // 3

       0.25f, -0.25f,  0.25f, // 5
       0.25f,  0.25f,  0.25f, // 4
      -0.25f, -0.25f,  0.25f, // 6

      -0.25f,  0.25f,  0.25f, // 7
      -0.25f, -0.25f,  0.25f, // 6
       0.25f,  0.25f,  0.25f, // 4

      -0.25f, -0.25f,  0.25f, // 6
      -0.25f,  0.25f,  0.25f, // 7
      -0.25f, -0.25f, -0.25f, // 1

      -0.25f,  0.25f, -0.25f, // 0
      -0.25f, -0.25f, -0.25f, // 1
      -0.25f,  0.25f,  0.25f, // 7

       0.25f, -0.25f, -0.25f, // 2
       0.25f, -0.25f,  0.25f, // 5
      -0.25f, -0.25f, -0.25f, // 1

      -0.25f, -0.25f,  0.25f, // 6
      -0.25f, -0.25f, -0.25f, // 1
       0.25f, -0.25f,  0.25f, // 5

       0.25f,  0.25f,  0.25f, // 4
       0.25f,  0.25f, -0.25f, // 3
      -0.25f,  0.25f,  0.25f, // 7

      -0.25f,  0.25f, -0.25f, // 0
      -0.25f,  0.25f,  0.25f, // 7
       0.25f,  0.25f, -0.25f  // 3
    };


    // Vertex Buffer Object (for vertex coordinates)
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);

    // Vertex attributes
    // 0: vertex position (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // Unbind vbo (it was conveniently registered by VertexAttribPointer)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Unbind vao
    glBindVertexArray(0);


    // Uniforms
    // - Model-View matrix
    // - Projection matrix
    mv_location = glGetUniformLocation(shader_program, "mv_matrix");
    proj_location = glGetUniformLocation(shader_program, "proj_matrix");

    // Render loop
    while (!glfwWindowShouldClose(window)) {

        processInput(window);

        render(glfwGetTime());

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

void render(double currentTime) {
    float f = (float)currentTime * 0.3f;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, gl_width, gl_height);

    // Render cube
    glUseProgram(shader_program);
    glBindVertexArray(vao);

    glm::mat4 mv_matrix, proj_matrix;

    mv_matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, 0.0f, -4.0f));
    mv_matrix = glm::translate(mv_matrix,
        glm::vec3(sinf(2.1f * f) * 0.5f,
            cosf(1.7f * f) * 0.5f,
            sinf(1.3f * f) * cosf(1.5f * f) * 2.0f));

    mv_matrix = glm::rotate(mv_matrix,
        glm::radians((float)currentTime * 45.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
    mv_matrix = glm::rotate(mv_matrix,
        glm::radians((float)currentTime * 81.0f),
        glm::vec3(1.0f, 0.0f, 0.0f));

    glUniformMatrix4fv(mv_location, 1, GL_FALSE, glm::value_ptr(mv_matrix));

    proj_matrix = glm::perspective(glm::radians(50.0f),
        (float)gl_width / (float)gl_height,
        0.1f, 1000.0f);
    glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(proj_matrix));

    glDrawArrays(GL_TRIANGLES, 0, 30);

    
    // Render face
    glUseProgram(shader_program_texture);
    glBindVertexArray(vao_texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glUniformMatrix4fv(mv_location, 1, GL_FALSE, glm::value_ptr(mv_matrix));

    glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(proj_matrix));

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}

// Callback function to track window size and update viewport
void glfw_window_size_callback(GLFWwindow* window, int width, int height) {
    gl_width = width;
    gl_height = height;
    printf("New viewport: (width: %d, height: %d)\n", width, height);
}
