#pragma once
#include <cstdint>

#include "missiles.h"
#include "monster.h"

namespace devilution {

constexpr _monster_id SKELETON_MINION_TYPE = MT_RSKELAX;

size_t MaxSummonsCount();

void InitSkeletonSummons();
void DeltaLoadSummon(Monster& monster);
const uint8_t* GetSummonTRNOrNull(const Monster &monster, int lightTableIndex);
void AddSkeletonSummon(Missile &missile, AddMissileParameter &parameter);
size_t GetSummonOwnerId(size_t summonId);
void SkeletonSummonAi(Monster &skeleton);

}