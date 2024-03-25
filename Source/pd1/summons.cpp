#include "pd1/summons.h"

#include <algorithm>
#include <array>

#include "engine/path.h"
#include "engine/trn.hpp"
#include "monstdat.h"
#include "player.h"

#define MAX_SKELETONS 3

namespace devilution {

// TODO: temporary solution to not bother with resource files
constexpr std::array<uint8_t, 256> SkeletonTrn = []() constexpr
{
    std::array<uint8_t, 256> arr{0};
    for (unsigned i = 0; i < 256; ++i)
        arr[i] = static_cast<uint8_t>(i);

    for (unsigned i = 0xA1; i <= 0xAF; ++i) // A1-AF => F0-FE
        arr[i] += 0x50 - 1;

    for (unsigned i = 0xD0; i <= 0xDF; ++i) // D0-DF => C0-CF probably not needed
        arr[i] -= 0x10;

    for (unsigned i = 0xE0; i <= 0xEF; ++i) // E0-EF => F0-FF probably not needed
        arr[i] += 0x10;
    
    arr[0xEF] = arr[0xFF] = 0;
    return arr;
}();


const uint8_t* GetSummonTrnOrNull(const Monster &monster, int lightTableIndex)
{
    if (monster.isPlayerMinion() && monster.type().type == MT_RSKELAX) {
        if (lightTableIndex > 6)
            return GetPauseTRN();

        return SkeletonTrn.data();
    }
    
    return nullptr;
}


int SkeletonSpawningSlot[MAX_PLRS] = {0};


void SpawnSkeletonSummon(Player &player, Monster &skel, Point position, Missile &missile)
{
	skel.occupyTile(position, false);
	skel.position.tile = position;
	skel.position.future = position;
	skel.position.old = position;
	skel.pathCount = 0;
	skel.maxHitPoints = 2 * (320 * missile._mispllvl + player._pMaxMana / 3);
	skel.hitPoints = skel.maxHitPoints;
	skel.armorClass = 25;
	skel.toHit = 5 * (missile._mispllvl + 8) + 2 * player.getCharacterLevel();
	skel.minDamage = 2 * (missile._mispllvl + 4);
	skel.maxDamage = 2 * (missile._mispllvl + 8);
	skel.flags |= MFLAG_GOLEM;

	skel.ai = MonsterAIID::SkeletonMelee; // TODO: add custom AI here
	// skel.ai = MonsterAIID::Golem;

	// TODO: I have a feeling that after being hit, skeleton starts to use GolumAI - check this theory.
	// 		 Actualy if we'll have a lash for golem, we can use it's AI for melee summons.
	//		 It will not work for ranged summons though.

    OnSummonSpawn(skel, static_cast<Direction>((SkeletonSpawningSlot[player.getId()] + 1) % 8)); // scrolling through directions for next spawned skeleton

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

Monster& GetSkeletonSummon(int skeletonIndex, int playerId) // skeleton index among skeleton summons
{
    int monsterIndex = playerId * (1 + MAX_SKELETONS) + 1 + skeletonIndex;  // + 1 to skip golems
    Monster& skeleton =  Monsters[monsterIndex];
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
        Monster &skeleton = GetSkeletonSummon(SkeletonSpawningSlot[playerId], playerId);

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

}