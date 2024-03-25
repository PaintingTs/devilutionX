#pragma once
#include <cstdint>

#include "missiles.h"
#include "monster.h"

namespace devilution {

const uint8_t* GetSummonTrnOrNull(const Monster &monster, int lightTableIndex);
void AddSkeletonSummon(Missile &missile, AddMissileParameter &parameter);

}