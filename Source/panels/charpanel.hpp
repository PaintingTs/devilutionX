#pragma once

#include "engine/clx_sprite.hpp"
#include "engine/surface.hpp"

namespace devilution {

constexpr Rectangle perksBtnRelativeRect = {{ 16, 106 }, { 95, 22 }};

extern OptionalOwnedClxSpriteList pChrButtons;

void DrawChr(const Surface &);
void LoadCharPanel();
void FreeCharPanel();

} // namespace devilution
