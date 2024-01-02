#pragma once

#include <glm/glm.hpp>
#include <map>

#include "types.h"

enum class MouseButton {
    LEFT,
    RIGHT
};

enum class MouseAction {
    PRESS,
    RELEASE
};

struct MouseState {
    MouseAction current = MouseAction::RELEASE;
    MouseAction previous = MouseAction::RELEASE;
};

struct MouseInput {
    glm::vec3 position;
    
    void setState(MouseButton button, MouseAction action);
    bool clicked(MouseButton button);

    private:

    std::map<MouseButton, MouseState> input;
};
