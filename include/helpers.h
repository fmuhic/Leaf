#pragma once

#include "types.h"

template <typename T>
ui32 toIndex(T value) {
    return (ui32) value;
}

ui32 pickRand(ui32 min, ui32 max);
