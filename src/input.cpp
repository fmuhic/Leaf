#include "input.h"

void Input::setMouseState(MouseButton button, InputAction action) {
    mouseInput[button].previous = mouseInput[button].current;
    mouseInput[button].current = action;
}

bool Input::clicked(MouseButton button) {
    return mouseInput[button].previous == InputAction::PRESS &&
        mouseInput[button].current == InputAction::RELEASE;
}

void Input::setKeyState(i32 key, InputAction action) {
    keyboardInput[key].previous = keyboardInput[key].current;
    keyboardInput[key].current = action;
}

bool Input::pressed(i32 key) {
    return keyboardInput[key].previous == InputAction::PRESS &&
        keyboardInput[key].current == InputAction::RELEASE;
}
