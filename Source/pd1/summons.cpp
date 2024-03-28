#include "pd1/summons.h"

#include <algorithm>
#include <array>
#include <memory>

#include "engine/path.h"
#include "engine/random.hpp"
#include "engine/trn.hpp"
#include "monstdat.h"
#include "player.h"

#include "utils/log.hpp"

#define MAX_SKELETONS 3
#define SKELETON_TYPE_INDEX 1

namespace devilution {

// TODO: temporary solution to not bother with resource files
constexpr std::array<uint8_t, 256> SkeletonTrn = []() constexpr
{
    std::array<uint8_t, 256> arr{0};
    for (unsigned i = 0; i < 256; ++i)
        arr[i] = static_cast<uint8_t>(i);

    for (unsigned i = 0xA1; i <= 0xAE; ++i) // A1-AF => F0-FE
        arr[i] += 0x50;

    for (unsigned i = 0xD0; i <= 0xDF; ++i) // D0-DF => C0-CF probably not needed
        arr[i] -= 0x10;

    for (unsigned i = 0xE0; i <= 0xED; ++i) // E0-EF => F0-FF probably not needed
        arr[i] += 0x10 + 1;
    
    arr[0xEF] = arr[0xFF] = arr[0xAF] = 0;
    return arr;
}();


size_t GetSummonOwnerId(size_t summonId) // example: g0 g1 g2 g3 | s0 s0 s0 s1 s1 s1 s2 s2 s2 s3 s3 s3 | L0 L1 L2 L3
{
	size_t summonIndex = summonId;
	if (summonId < MAX_PLRS)
		return summonId;

	summonIndex -= MAX_PLRS;

	if (summonIndex < MAX_PLRS * MAX_SKELETONS)
		return summonIndex / MAX_SKELETONS;

	summonIndex -= MAX_PLRS * MAX_SKELETONS;

	assert("Unreachable");
	return 0;
}

void InitSkeletonSummons()
{
	for (int i = 0; i < MAX_PLRS * MAX_SKELETONS; i++) 
		AddMonster(GolemHoldingCell, Direction::South, SKELETON_TYPE_INDEX, false); 

} 

const uint8_t* GetSummonTRNOrNull(const Monster &monster, int lightTableIndex)
{
	if (!monster.isPlayerMinion())
		return nullptr;

	if (lightTableIndex > 7)
		return GetPauseTRN();

    if (monster.type().type == SKELETON_MINION_TYPE)
        return SkeletonTrn.data();
    
    return nullptr;
}

//NOTICE: looks like all the corpses of the same type are references to single item in Corpses array
//		  for unique monsters there are additional slots, but total Corpses length must be <= 31, i.e.
//		  monster_types_on_level + uniques_on_level <= 31.
//
//Solution: we can (present Summons as unique monsters OR add another monster types for summons) AND increase Corpses array size to accomondate this

int SkeletonSpawningSlot[MAX_PLRS] = {0};


void SpawnSkeletonSummon(Player &player, Monster &skel, Point position, Missile &missile)
{
	// TODO: BUG: spawning a skeleton on a corpse causes grame to break
	skel.occupyTile(position, false);
	skel.position.tile = position;
	skel.position.future = position;
	skel.position.old = position;
	skel.position.last = position;
	skel.pathCount = 0;
	skel.maxHitPoints = 2 * (100 * missile._mispllvl + player._pMaxMana / 3);
	skel.hitPoints = skel.maxHitPoints;
	skel.armorClass = 25;
	skel.toHit = 5 * (missile._mispllvl + 8) + 2 * player.getCharacterLevel();
	skel.minDamage = 1 * (missile._mispllvl + 4);
	skel.maxDamage = 1 * (missile._mispllvl + 8);
	skel.flags |= MFLAG_GOLEM;
	skel.flags |= MFLAG_TARGETS_MONSTER;

	// visual apperiance (might be usfull later)
	// skel.uniqueMonsterTRN = std::make_unique<uint8_t[]>(256); //TODO: this code should be done only once when initializing a monster
	// std::copy(SkeletonTrn.begin(), SkeletonTrn.end(), skel.uniqueMonsterTRN.get());

	// AI
	skel.ai = MonsterAIID::SkeletonSummon; // TODO: add custom AI here

    StartSpecialStand(skel, static_cast<Direction>((SkeletonSpawningSlot[player.getId()] + 1) % 8)); // scrolling through directions for next spawned skeleton
	UpdateEnemy(skel);

	if (&player == MyPlayer) {
		/* NetSendCmdGolem(             //TODO: differnt type of command
		    skel.position.tile.x,
		    skel.position.tile.y,
		    skel.direction,
		    skel.enemy,
		    skel.hitPoints,
		    GetLevelForMultiplayer(player)); */
	}
}

void KillMySummonedSkeleton(Monster& skel)
{
	delta_kill_monster(skel, skel.position.tile, *MyPlayer);
	NetSendCmdLoc(MyPlayerId, false, CMD_MONSTDEATH, skel.position.tile);
	M_StartKill(skel, *MyPlayer);
}

Monster& GetSkeletonSummon(int playerId) // skeleton index among skeleton summons
{
	int skeletonIndex = SkeletonSpawningSlot[playerId];
    int monsterIndex = MAX_PLRS + playerId * MAX_SKELETONS + skeletonIndex; // +MAX_PLRS to skip golems 
    Monster& skeleton = Monsters[monsterIndex];
    return skeleton;
}


void AddSkeletonSummon(Missile &missile, AddMissileParameter &parameter)
{
	missile._miDelFlag = true;
	int playerId = missile._misource;
	Player &player = Players[playerId];

    int maxSkeletons =  std::min(missile._mispllvl + 1, MAX_SKELETONS); // TODO: to balance team

    std::optional<Point> spawnPosition = FindClosestValidPosition(
		[start = missile.position.start](Point target) {
			return !IsTileOccupied(target) && LineClearMissile(start, target);
		},
		parameter.dst, 0, 5);


	if (spawnPosition) {
        Monster &skeleton = GetSkeletonSummon(playerId);

		Log("!! Spawn position x:{}, y:{}", (*spawnPosition).x, (*spawnPosition).y);

        if (skeleton.position.tile != GolemHoldingCell && &player == MyPlayer)
		    KillMySummonedSkeleton(skeleton);

		// TODO: we can have +1 skeleton to represent dying/death one. So in this line we can increment SkeletonSpawningSlot
		// Notice: For Golem spell, second cast de-spawn the golem. It might be useful, but we don't have an option to de-spawn all skeletons right now
		//		   Although we can manually kill them using attacking spells or melee damage(?)	 

		if (skeleton.position.tile == GolemHoldingCell) {
			SpawnSkeletonSummon(player, skeleton, *spawnPosition, missile);

			if (++SkeletonSpawningSlot[playerId] == maxSkeletons)
				SkeletonSpawningSlot[playerId] = 0;
		}

	}
}


void SkeletonSummonAi(Monster &skeleton)
{
	if (skeleton.position.tile.x == 1 && skeleton.position.tile.y == 0) {
		return;
	}

	if (IsAnyOf(skeleton.mode, MonsterMode::Death, MonsterMode::SpecialStand) || skeleton.isWalking()) {
		return;
	}

	if ((skeleton.flags & MFLAG_TARGETS_MONSTER) == 0)
		UpdateEnemy(skeleton);

	if (skeleton.mode == MonsterMode::MeleeAttack) {
		return;
	}

	const Player &owner = Players[GetSummonOwnerId(skeleton.getId())];
	const int ownerDist = owner.position.tile.WalkingDistance(skeleton.position.tile);

	if (ownerDist >= 12)
		skeleton.flags |= MFLAG_SUMMON_RETURNS;
	if (ownerDist <= 5)
		skeleton.flags &= ~MFLAG_SUMMON_RETURNS;

	if ((skeleton.flags & MFLAG_SUMMON_RETURNS) != 0) {
		Direction ownerDirection = GetDirection(skeleton.position.tile, owner.position.tile);
		if (RandomWalk2(skeleton, ownerDirection))
		return;
	}

	SkeletonAi(skeleton);

	if ((skeleton.flags & MFLAG_NO_ENEMY) != 0) {
		Direction md = GetDirection(skeleton.position.tile, skeleton.position.last);
		skeleton.direction = md;
		if (static_cast<MonsterMode>(skeleton.var1) == MonsterMode::Delay || (GenerateRnd(100) >= 35 - 4 * skeleton.intelligence)) {
			RandomWalk2(skeleton, md);
		} else {
			AiDelay(skeleton, 15 - 2 * skeleton.intelligence + GenerateRnd(10));
		}
	}
}

}