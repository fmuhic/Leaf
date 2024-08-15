#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "renderer.h"
#include "input.h"
#include "config.h"
#include "game.h"
#include "examples/slider.h"
#include "examples/thumbler.h"
#include "examples/stacking.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processKeyboardInput(GLFWwindow *window, Input &input);
void processMouseInput(GLFWwindow *window, Input &input);
glm::vec3 screenToWorld(glm::vec3 p, Scene *scene, f32 width, f32 height);

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define GAME_UPDATE_INTERVAL_SEC 1.0f / 60.0f
#define ENTITY_COUNT 1000

using std::cout;
using std::endl;
using glm::vec3;

Renderer* renderer;
EntitySystem* entitySystem;
Leaf* leaf;
Scene* scene;
Game *game;
DebugConfig *debugConfig;

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

    debugConfig = new DebugConfig();

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

    leaf = new Leaf();
    entitySystem = new EntitySystem(leaf);
    game = new Game(leaf, entitySystem);
    game->changeScene(new StackingExample(entitySystem));

    Input input;

    f64 previous = glfwGetTime();
    f64 lag = 0.0;
    while (!glfwWindowShouldClose(window)) {
        f64 current = glfwGetTime();
        f64 elapsed = current - previous;
        lag += elapsed;
        previous = current;

        while (lag > GAME_UPDATE_INTERVAL_SEC) {
            processKeyboardInput(window, input);
            processMouseInput(window, input);
            game->update(GAME_UPDATE_INTERVAL_SEC, glfwGetTime(), input);
            lag -= GAME_UPDATE_INTERVAL_SEC;
        }

        renderer->draw(*scene, *entitySystem, *debugConfig);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processKeyboardInput(GLFWwindow *window, Input& input) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
        input.setKeyState(GLFW_KEY_D, InputAction::RELEASE);
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        input.setKeyState(GLFW_KEY_D, InputAction::PRESS);
    }
    if (input.pressed(GLFW_KEY_D)) {
        debugConfig->showContactPoints = !debugConfig->showContactPoints;
        debugConfig->showDynamicTreeGrid = !debugConfig->showDynamicTreeGrid;
}

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) {
        input.setKeyState(GLFW_KEY_W, InputAction::RELEASE);
    }
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        input.setKeyState(GLFW_KEY_W, InputAction::PRESS);
    }
    if (input.pressed(GLFW_KEY_W)) {
        debugConfig->showWiredEntities = !debugConfig->showWiredEntities;
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
        input.setKeyState(GLFW_KEY_1, InputAction::RELEASE);
    }
    else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        input.setKeyState(GLFW_KEY_1, InputAction::PRESS);
    }
    if (input.pressed(GLFW_KEY_1)) {
        game->changeScene(new StackingExample(entitySystem));
    }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
        input.setKeyState(GLFW_KEY_2, InputAction::RELEASE);
    }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        input.setKeyState(GLFW_KEY_2, InputAction::PRESS);
    }
    if (input.pressed(GLFW_KEY_2)) {
        game->changeScene(new Thumbler(entitySystem, leaf));
    }

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) {
        input.setKeyState(GLFW_KEY_3, InputAction::RELEASE);
    }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        input.setKeyState(GLFW_KEY_3, InputAction::PRESS);
    }
    if (input.pressed(GLFW_KEY_3)) {
        game->changeScene(new SliderExample(entitySystem));
    }
}

void processMouseInput(GLFWwindow *window, Input &input) {
    int leftMouse = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    int rightMouse = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

    if (leftMouse == GLFW_RELEASE)
        input.setMouseState(MouseButton::LEFT, InputAction::RELEASE);
    else if (leftMouse == GLFW_PRESS)
        input.setMouseState(MouseButton::LEFT, InputAction::PRESS);

    if (rightMouse == GLFW_RELEASE)
        input.setMouseState(MouseButton::RIGHT, InputAction::RELEASE);
    else if (rightMouse == GLFW_PRESS)
        input.setMouseState(MouseButton::RIGHT, InputAction::PRESS);

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
