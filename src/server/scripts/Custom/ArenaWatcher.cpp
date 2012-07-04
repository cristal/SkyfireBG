/*######
## Arena Watcher
######*/

#include "Player.h"
#include "BattlegroundMgr.h"
#include "Battleground.h"
#include "ArenaTeamMgr.h"
#include "ArenaTeam.h"

enum WatcherData
{
    GOSSIP_OFFSET = GOSSIP_ACTION_INFO_DEF + 10,
};

class npc_arena_watcher : public CreatureScript
{
public:
    npc_arena_watcher() : CreatureScript("npc_arena_watcher") {}
    
    bool OnGossipHello(Player* player, Creature* creature)
    {
        // Count all arenas
        BattlegroundSet arenasSet = sBattlegroundMgr->GetAllBattlegroundsWithTypeId(BATTLEGROUND_AA);
        uint32 arenasQty[3] = {0, 0, 0};
        for (BattlegroundSet::const_iterator itr = arenasSet.begin(); itr != arenasSet.end(); ++itr)
            if (Battleground* bg = itr->second)
                switch (bg->GetArenaType())
                {
                    case ARENA_TYPE_2v2:  arenasQty[0]++; break;
                    case ARENA_TYPE_3v3:  arenasQty[1]++; break;
                    case ARENA_TYPE_5v5:  arenasQty[2]++; break;
                }

        std::stringstream gossip2;
        std::stringstream gossip3;
        std::stringstream gossip5;
        gossip2 << "Observe a 2v2 match. (" << arenasQty[0] << " match(s) in progress)";
        gossip3 << "Observe a 3v3 match. (" << arenasQty[1] << " match(s) in progress)";
        gossip5 << "Observe a 5v5 match. (" << arenasQty[2] << " match(s) in progress)";

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, gossip2.str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, gossip3.str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, gossip5.str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "Observe a player.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4, "", 0, true);
        
        player->PlayerTalkClass->SendGossipMenu(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        uint8 mode = ARENA_TYPE_2v2;
        if (action == (GOSSIP_ACTION_INFO_DEF + 3)) // 3v3
            mode = ARENA_TYPE_3v3;
        if (action == (GOSSIP_ACTION_INFO_DEF + 5)) // 5v5
            mode = ARENA_TYPE_5v5;

        if (action <= GOSSIP_OFFSET)
        {
            BattlegroundSet arenasSet = sBattlegroundMgr->GetAllBattlegroundsWithTypeId(BATTLEGROUND_AA);

            // Check for matches of chosen type
            bool bracketMatchs = false;
            for (BattlegroundSet::const_iterator itr = arenasSet.begin(); itr != arenasSet.end(); ++itr)
            {
                if (Battleground* bg = itr->second)
                {
                    if (bg->GetArenaType() == mode)
                    {
                        bracketMatchs = true;
                        break;
                    }
                }
            }

            if (!bracketMatchs)
            {
                std::stringstream errMsg;
                errMsg << "Sorry " << player->GetName() << ", There are no current matches of the type you selected.";
                creature->MonsterWhisper(errMsg.str().c_str(), player->GetGUID());
                player->PlayerTalkClass->ClearMenus();
                player->CLOSE_GOSSIP_MENU();
            }
            else
            {
                // team 1 and 2!
                for (BattlegroundSet::const_iterator itr = arenasSet.begin(); itr != arenasSet.end(); ++itr)
                {
                    if (Battleground* bg = itr->second)
                    {
                        ArenaTeam* teamOne = sArenaTeamMgr->GetArenaTeamById(bg->GetArenaTeamIdByIndex(0));
                        ArenaTeam* teamTwo = sArenaTeamMgr->GetArenaTeamById(bg->GetArenaTeamIdByIndex(1));

                        if (teamOne && teamTwo)
                        {
                            std::stringstream gossipItem;
                            gossipItem << teamOne->GetName() << " (";
                            gossipItem << teamOne->GetRating() << ") VS ";
                            gossipItem << teamTwo->GetName() << " (";
                            gossipItem << teamTwo->GetRating() << ")";
                            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, gossipItem.str(), GOSSIP_SENDER_MAIN + 1, itr->first + GOSSIP_OFFSET);
                        }
                    }
                }
                player->PlayerTalkClass->SendGossipMenu(player->GetGossipTextId(creature), creature->GetGUID());
            }
        }
        else
        {
            uint32 arenaId = action - GOSSIP_OFFSET;
            // Don't really bother about WPE injection here, we are allowing pretty much any arena selection
            BattlegroundSet arenasSet = sBattlegroundMgr->GetAllBattlegroundsWithTypeId(BATTLEGROUND_AA);

            if (arenasSet[arenaId] != NULL)
            {
                Battleground* arenaChosen = arenasSet[arenaId];

                // spectator crap
                if (arenaChosen->GetStatus() != STATUS_NONE && arenaChosen->GetStatus() != STATUS_IN_PROGRESS)
                {
                    std::stringstream errMsg;
                    errMsg << "Sorry " << player->GetName() << ", the chosen arena has ended";
                    creature->MonsterWhisper(errMsg.str().c_str(), player->GetGUID());
                    player->PlayerTalkClass->ClearMenus();
                    player->CLOSE_GOSSIP_MENU();
                    return false;
                }

                // OK. In the case of a selected arena, we teleport in the center of the arena.
                player->SetBattlegroundId(arenaChosen->GetInstanceID(), arenaChosen->GetTypeID());
                player->SetBattlegroundEntryPoint();
                float x, y, z;
                switch (arenaChosen->GetMapId())
                {
                    case 617: x = 1299.046f;    y = 784.825f;     z = 9.338f;     break; // Dalaran Sewers
                    case 618: x = 763.5f;       y = -284;         z = 28.276f;    break; // Ring of Valor
                    case 572: x = 1285.810547f; y = 1667.896851f; z = 39.957642f; break; // Ruins of Lordearon
                    case 562: x = 6238.930176f; y = 262.963470f;  z = 0.889519f;  break; // Blade's Edge Arena
                    case 559: x = 4055.504395f; y = 2919.660645f; z = 13.611241f; break; // Nagrand Arena
                }
                player->SetGMVisible(false); // Make the player invisible. TODO: Use a custom spell in `spell_dbc`
                player->TeleportTo(arenaChosen->GetMapId(), x, y, z, player->GetOrientation());
            }
        }
        return true;
    }

    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 uiSender, uint32 uiAction, const char* code)
    {
        player->PlayerTalkClass->ClearMenus();
        player->CLOSE_GOSSIP_MENU();
        if (uiSender == GOSSIP_SENDER_MAIN)
        {
            switch (uiAction)
            {
                case GOSSIP_ACTION_INFO_DEF + 4: // choosing a player
                    // incase WPE check everything
                    const char* plrName = code;
                    if (Player* target = sObjectAccessor->FindPlayerByName(plrName))
                    {
                        if (!target->IsInWorld())
                        {
                            creature->MonsterWhisper("Sorry, I can not allow you to teleport to that player. they are currently porting or being ported", player->GetGUID());
                            return true;
                        }

                        if (!target->InArena())
                        {
                            creature->MonsterWhisper("Sorry, this player is not in arena. Nice try!", player->GetGUID());
                            return true;
                        }

                        if (target->isGameMaster())
                        {
                            creature->MonsterWhisper("Sorry! We do not teleport to GMs!", player->GetGUID());
                            return true;
                        }

                        // target is in arena and not GM
                        player->SetBattlegroundId(target->GetBattleground()->GetInstanceID(), target->GetBattleground()->GetTypeID());
                        player->SetBattlegroundEntryPoint();
                        float x, y, z;
                        target->GetContactPoint(player, x, y, z);
                        player->TeleportTo(target->GetMapId(), x, y, z, player->GetAngle(target));
                        player->SetGMVisible(false); // Set invisible. TODO: Use a custom spell in `spell_dbc`
                    }
                    return true;
            }
        }

        return false;
    }
};

void AddSC_ArenaWatcher()
{
    new npc_arena_watcher();
}
