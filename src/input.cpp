#include "input.h"

void MouseInput::setState(MouseButton button, MouseAction action) {
    input[button].previous = input[button].current;
    input[button].current = action;
}

bool MouseInput::clicked(MouseButton button) {
    return input[button].previous == MouseAction::PRESS &&
        input[button].current == MouseAction::RELEASE;
}
