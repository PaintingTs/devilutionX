#pragma once
#include <cstdint>

#include "missiles.h"
#include "monster.h"

namespace devilution {

constexpr _monster_id SKELETON_MINION_TYPE = MT_RSKELAX;

void InitSkeletonSummons();
const uint8_t* GetSummonTrnOrNull(const Monster &monster, int lightTableIndex);
void AddSkeletonSummon(Missile &missile, AddMissileParameter &parameter);
size_t GetSummonOwnerId(size_t summonId);

}