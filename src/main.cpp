#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "examples/slider.h"
#include "geometry.h"
#include "helpers.h"
#include "renderer.h"
#include "input.h"
#include "game.h"
#include "examples/thumbler.h"
#include "examples/stacking.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processKeyboardInput(GLFWwindow *window);
void processMouseInput(GLFWwindow *window, MouseInput &input);
glm::vec3 screenToWorld(glm::vec3 p, Scene *scene, f32 width, f32 height);

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define GAME_UPDATE_INTERVAL_SEC 0.005f
#define ENTITY_COUNT 100

using std::cout;
using std::endl;
using glm::vec3;

Renderer* renderer;
Scene* scene;
Game *game;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Leaf", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    renderer = new Renderer(
        (f32) SCREEN_WIDTH,
        (f32) SCREEN_HEIGHT
    );

    scene = new Scene();
    scene->camera = glm::lookAt(
        vec3(0.0, 0.0, 0.05f),
        vec3(0.0, 0.0, 0.0f),
        vec3(0.0f, 1.0f, 0.0f)
    );

    framebufferSizeCallback(window, SCREEN_WIDTH, SCREEN_HEIGHT);

    game = new Game(ENTITY_COUNT);
    game->changeScene(new StackingExample());

    MouseInput mInput;

    f64 previous = glfwGetTime();
    f64 lag = 0.0;
    while (!glfwWindowShouldClose(window)) {
        f64 current = glfwGetTime();
        f64 elapsed = current - previous;
        lag += elapsed;
        previous = current;

        while (lag > GAME_UPDATE_INTERVAL_SEC) {
            processKeyboardInput(window);
            processMouseInput(window, mInput);
            game->update(GAME_UPDATE_INTERVAL_SEC, glfwGetTime(), mInput);
            lag -= GAME_UPDATE_INTERVAL_SEC;
        }

        renderer->draw(*scene, *game);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processKeyboardInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        game->changeScene(new StackingExample());

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        game->changeScene(new Thumbler());

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        game->changeScene(new SliderExample());
}

void processMouseInput(GLFWwindow *window, MouseInput &input) {
    int leftMouse = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    int rightMouse = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

    if (leftMouse == GLFW_RELEASE)
        input.setState(MouseButton::LEFT, MouseAction::RELEASE);
    else if (leftMouse == GLFW_PRESS)
        input.setState(MouseButton::LEFT, MouseAction::PRESS);

    if (rightMouse == GLFW_RELEASE)
        input.setState(MouseButton::RIGHT, MouseAction::RELEASE);
    else if (rightMouse == GLFW_PRESS)
        input.setState(MouseButton::RIGHT, MouseAction::PRESS);

    f64 x, y;
    glfwGetCursorPos(window, &x, &y);
    glm::vec3 worldCoordinates = screenToWorld(glm::vec3(x, y, 0.0f), scene, renderer->screenWidth, renderer->screenHeight);
    input.position = glm::vec3(
        worldCoordinates.x,
        worldCoordinates.y,
        0.0f
    );
}

void framebufferSizeCallback([[maybe_unused]] GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    renderer->screenWidth = width;
    renderer->screenHeight = height;
    renderer->aspectRatio = (f32) width / (f32) height;

    scene->projection = glm::ortho(
        -10.0f * renderer->aspectRatio,
        10.0f * renderer->aspectRatio,
        -10.0f,
        10.0f,
        -1.0f,
        1.0f
    );
}

glm::vec3 screenToWorld(glm::vec3 p, Scene *scene, f32 width, f32 height) {
    glm::vec3 ndc = glm::vec3(p.x / width, 1.0f - p.y / height, p.z) * 2.0f - 1.0f;
    return glm::inverse(scene->projection * scene->camera) * glm::vec4(ndc, 1);
}
