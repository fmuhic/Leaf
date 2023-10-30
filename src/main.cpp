#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assert.h>
#include <string>
#include <unordered_map>
#include <iostream>

#include "glm/ext/matrix_float4x4.hpp"
#include "helpers.h"
#include "loader.h"
#include "state.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

using std::unordered_map;
using std::string;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    unordered_map<string, string> vertexSrc;
    vertexSrc.insert({"simple", "./src/shaders/simple.vert"});

    unordered_map<string, string> fragmentSrc;
    fragmentSrc.insert({"simple", "./src/shaders/simple.frag"});

    auto vertexShaders = compileShaders(vertexSrc, GL_VERTEX_SHADER);
    auto fragmentShaders = compileShaders(fragmentSrc, GL_FRAGMENT_SHADER);

    Renderer renderer = {};

    createShaderPrograms(&renderer, vertexShaders, fragmentShaders);

    for (auto &it: vertexShaders) {
        glDeleteShader(it.second);
    }
    for (auto &it: fragmentShaders) {
        glDeleteShader(it.second);
    }

    createVertexObjects(&renderer);

    // int windowWidth_ = 1920;
    // int windowHeight_ = 1080;
    // float aspectRatio = 16.0f / 9.0f;
    // glViewport(0, 0, (int) ((float) windowHeight_ * aspectRatio), windowHeight_);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        uint32 shaderProgram = renderer.shaderPrograms[toIndex(ShaderProgram::REGULAR)];
        glUseProgram(shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 position = glm::vec3(1.5f, 1.5f, 0.0f);
        // glm::mat4 camera = glm::lookAt(
        //     glm::vec3(position.x, position.y, 0.05f),
        //     glm::vec3(position.x, position.y, 0.0f),
        //     glm::vec3(0.0f, 1.0f, 0.0f)
        // );

        float metersToPixels = 100;

        // model = glm::translate(model, position * metersToPixels);
        // model = glm::rotate(model, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
        // model = glm::scale(model, glm::vec3(1.0f, 1.0f, 0.0f));
        // model = glm::translate(model, glm::vec3(0.5f, -0.5f, 0.0f));
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

        GLint modelMatrix = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(model));

        // GLint viewMatrix = glGetUniformLocation(shaderProgram, "view");
        // glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, glm::value_ptr(camera * metersToPixels));
        //
        // glm::mat4 projection = glm::ortho(-960.0f, 960.0f, -540.0f, 540.0f, -10.0f, 10.0f);
        // GLint projectionMatrix = glGetUniformLocation(shaderProgram, "projection");
        // glUniformMatrix4fv(projectionMatrix, 1, GL_FALSE, glm::value_ptr(projection));






        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer.ebo[toIndex(EBO::QUAD)]);
        glBindVertexArray(renderer.vao[toIndex(VAO::QUAD)]); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        // glBindVertexArray(0); // no need to unbind it every time 
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &renderer.vao[toIndex(VAO::QUAD)]);
    glDeleteBuffers(1, &renderer.vbo[toIndex(VBO::QUAD)]);
    glDeleteBuffers(1, &renderer.ebo[toIndex(EBO::QUAD)]);
    glDeleteProgram(renderer.shaderPrograms[toIndex(ShaderProgram::REGULAR)]);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
