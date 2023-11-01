#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assert.h>
#include <string>
#include <iostream>

#include "glm/ext/matrix_float4x4.hpp"
#include "helpers.h"
#include "loader.h"
#include "renderer.h"
#include "state.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define DEFAULT_METERS_TO_PIXELS 100

using std::string;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Clever title here", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Camera *camera = new Camera {
        glm::lookAt(
            glm::vec3(0.0, 0.0, 0.05f),
            glm::vec3(0.0, 0.0, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        )
    };
    Renderer *renderer = createRenderer(
        (f32) SCREEN_WIDTH,
        (f32) SCREEN_HEIGHT,
        DEFAULT_METERS_TO_PIXELS
    );

    setupScene(renderer);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        drawFrame(renderer, camera);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    rendererCleanup(renderer);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
