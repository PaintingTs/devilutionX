#pragma once

#include "engine/surface.hpp"

namespace devilution {

extern bool PerksBoxIsOpen;

void DrawPerksBox(const Surface &out);
void CheckPerkBoxClick();

}