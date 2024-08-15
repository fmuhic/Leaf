#pragma once

#include <glm/glm.hpp>
#include <map>
#include "types.h"

enum class MouseButton {
    LEFT,
    RIGHT
};

enum class InputAction {
    PRESS,
    RELEASE
};

struct InputState {
    InputAction current = InputAction::RELEASE;
    InputAction previous = InputAction::RELEASE;
};

struct Input {
    glm::vec3 position;
    
    void setMouseState(MouseButton button, InputAction action);
    bool clicked(MouseButton button);

    void setKeyState(i32 key, InputAction action);
    bool pressed(i32 key);

    private:

    std::map<MouseButton, InputState> mouseInput;
    std::map<i32, InputState> keyboardInput;
};
