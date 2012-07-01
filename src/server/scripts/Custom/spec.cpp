#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"
#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "World.h"
#include "BattlegroundMgr.h"
#include "Battleground.h"
#include "ArenaTeam.h"
#include "ArenaTeamMgr.h"
#include "Define.h"
#include "ArenaTeamMgr.h"
#include "World.h"
#include "Log.h"
#include "DatabaseEnv.h"
#include "Language.h"
#include "ObjectAccessor.h"
#include "float.h"
#include "Chat.h"

class spec_npc : CreatureScript
{
public:
    spec_npc() : CreatureScript("spec_npc") { }
	
	bool OnGossipHello(Player* player, Creature* creature)
	{
		GossipHelloMenu(player, creature);
		return true;
	}
	bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
	{
		if(GossipSelectMenu(player, creature, sender, action) == false)
			if(GossipSelectSpec(player, creature, sender, action) == false) { }
		return true;
	}

	void GossipHelloSpec(Player* player, Creature* creature, uint32 arenaType)
	{
		sLog->outString("1.");
		player->PlayerTalkClass->ClearMenus();

		int32 menuMenuItem = 0;
		int32 maxMenuItems = GOSSIP_MAX_MENU_ITEMS - 2;

		int32 refreshMenuItem = GetMenuIdByArenaType(arenaType);
		if(refreshMenuItem == NULL)
			return;
		sLog->outString("2.");
	    player->ADD_GOSSIP_ITEM(4, "<< Menu", GOSSIP_SENDER_MAIN, menuMenuItem);
        player->ADD_GOSSIP_ITEM(4, "Refresh", GOSSIP_SENDER_MAIN, refreshMenuItem);

		int32 maxCount = maxMenuItems;
		for (uint32 i = BATTLEGROUND_TYPE_NONE; i < MAX_BATTLEGROUND_TYPE_ID; ++i)
	    {	
          for (BattlegroundSet::iterator itr = sBattlegroundMgr->m_Battlegrounds[i].begin(); itr != sBattlegroundMgr->m_Battlegrounds[i].end(); itr++)
          {
			sLog->outString("4.");
			Battleground* bg = itr->second;

			if(!CanSpectat(bg))
				continue;
			sLog->outString("5.");
			if(maxCount == 0)
				continue;
			sLog->outString("6.");
			if(bg->GetArenaType() != arenaType)
				continue;
			sLog->outString("7.");

			ArenaTeam* goldTeam = NULL;
			ArenaTeam* greenTeam = NULL;

			for (Battleground::BattlegroundPlayerMap::const_iterator itr2 = bg->GetPlayers().begin(); itr2 != bg->GetPlayers().end(); ++itr2)
            {
			  if (Player *target = ObjectAccessor::FindPlayer(itr2->first))
			  {
				  int32 targetArenaTeamId = target->GetArenaTeamIdInvited(); // << Hibalehetőség
				  sLog->outString("7.1.");
				  if(target->GetTeam() == ALLIANCE) // ALLIANCE
					  goldTeam = sArenaTeamMgr->GetArenaTeamById(targetArenaTeamId);

				  else
					  greenTeam = sArenaTeamMgr->GetArenaTeamById(targetArenaTeamId);
			  }
			}
			sLog->outString("8.");
			if(goldTeam == NULL || greenTeam == NULL || !goldTeam || !greenTeam)
				continue;
			sLog->outString("9.");
			std::string outPutGoldTeam = "[" + GetString(goldTeam->GetRating()) + "] ";
			std::string outPutGreenTeam = "[" + GetString(greenTeam->GetRating()) + "] ";

			int32 guid = 0;
			for (Battleground::BattlegroundPlayerMap::const_iterator itr2 = bg->GetPlayers().begin(); itr2 != bg->GetPlayers().end(); ++itr2)
            {
			  if (Player *target = ObjectAccessor::FindPlayer(itr2->first))
			  {
				if(target->GetBGTeam() == 1)
				   outPutGoldTeam += GetType(target) + " ";
	
				else
				   outPutGreenTeam += GetType(target) + " ";

				guid = player->GetGUID();
			  }
			}
			sLog->outString("10.");
			if(guid == 0)
			   continue;
			sLog->outString("10.");
			std::string outPut = outPutGoldTeam + "VS " + outPutGreenTeam;
			player->ADD_GOSSIP_ITEM(4, outPut, GOSSIP_SENDER_MAIN, guid);

		   maxCount--;
		  }
        }

		player->PlayerTalkClass->SendGossipMenu(907, creature->GetGUID());
	}
	bool GossipSelectSpec(Player* player, Creature* creature, uint32 sender, uint32 action)
	{
		Player* target = ObjectAccessor::FindPlayer(action);
		if(!target)
			return false;

		Battleground* bg = target->GetBattleground();
		if(!CanSpectat(bg))
			return false;

		SpectatePlayer(player, target, bg);
		return true;
	}

	void GossipHelloMenu(Player* player, Creature* creature)
	{
		player->PlayerTalkClass->ClearMenus();

		player->ADD_GOSSIP_ITEM(4, "[2v2] Rated", GOSSIP_SENDER_MAIN, -2);
        player->ADD_GOSSIP_ITEM(4, "[3v3] Rated", GOSSIP_SENDER_MAIN, -3);
        player->ADD_GOSSIP_ITEM(4, "[5v5] Rated", GOSSIP_SENDER_MAIN, -5);
		player->ADD_GOSSIP_ITEM(7, "Spectate By Name", GOSSIP_SENDER_MAIN, -1);
        player->PlayerTalkClass->SendGossipMenu(907, creature->GetGUID());
	}
	bool GossipSelectMenu(Player* player, Creature* creature, uint32 sender, uint32 action)
	{
		switch(action)
		{
		case 0:
			GossipHelloMenu(player, creature);
			return true;
		case -1:
			// Név szerinti keresés
			return true;
		case -2:
			GossipHelloSpec(player, creature, ARENA_TYPE_2v2);
			return true;
		case -3:
			GossipHelloSpec(player, creature, ARENA_TYPE_3v3);
			return true;
		case -5:
			GossipHelloSpec(player, creature, ARENA_TYPE_5v5);
			return true;
		}
		return false;
	}

	void SpectatePlayer(Player* player, Player* target, Battleground* bg)
	{
		if(!player || !target || !CanSpectat(bg))
			return;

		player->SaveRecallPosition();
        player->SetBattlegroundId(target->GetBattlegroundId(), target->GetBattlegroundTypeId());
		player->SetBattlegroundEntryPoint();

		player->m_deathState = DEAD;
		player->SetPhaseMask(target->GetPhaseMask(), false);
		player->SetVisible(false);
		player->m_ExtraFlags |= PLAYER_EXTRA_GM_ON;

		bg->HandlePlayerUnderMap(player);
		player->SetSpeed(MOVE_RUN, 4, true);
	}
	bool CanSpectat(Battleground* bg)
	{
		if(!bg)
			return false;

		if(bg->isArena() && bg->isRated() && bg->GetStatus() == STATUS_IN_PROGRESS)
			return true;

		return false;
	}
	std::string GetType(Player* player)
	{
		switch(player->getClass())
        {
                case CLASS_WARRIOR:
                    return "War";
                case CLASS_PALADIN:
                    return "Pal";
                case CLASS_ROGUE:
                    return "Rog";
                case CLASS_PRIEST:
                    return "Pri";
                case CLASS_SHAMAN:
                    return "Sha";
                case CLASS_MAGE:
                    return "Mag";
                case CLASS_WARLOCK:
                    return "Loc";
                case CLASS_DRUID:
                    return "Dru";
                case CLASS_HUNTER:
                    return "Hun";
				default:
					return "***";
        }
	}
	std::string GetString(int number)
	{
	    std::stringstream ss;
	    ss << number;
	    return ss.str();
	}	
	Battleground* GetArena(int guid)
	{
		Player* player = ObjectAccessor::FindPlayer(guid);
		if(!player)
			return NULL;
		
		Battleground* bg = player->GetBattleground();
		if(!CanSpectat(bg))
			return bg;

		return NULL;
	}
	int32 GetMenuIdByArenaType(uint32 arenaType)
	{
		switch(arenaType)
		{
		case ARENA_TYPE_2v2:
			return -2;
		case ARENA_TYPE_3v3:
			return -3;
		case ARENA_TYPE_5v5:
			return -5;
		}

		return NULL;
	}
};
class spec_comm : public CommandScript
{
public:
    spec_comm() : CommandScript("spec_comm") { }

	ChatCommand* GetCommands() const
    {
        static ChatCommand IngameCommandTable[] =
        {
			{ "leave", SEC_PLAYER, false, &HandleLeaveCommand, "", NULL },
            { NULL,    0,          false, NULL,                "", NULL }
        }; 
		 return IngameCommandTable;
    }

	static bool HandleLeaveCommand(ChatHandler * handler, const char * args)
    {
        Player * player = handler->GetSession()->GetPlayer();
		if(!player)
			return true;

		if(player->IsVisible() == false && player->m_deathState == DEAD && player->GetSpeed(MOVE_RUN) == 4)
		{
			player->TeleportToBGEntryPoint();
			player->SetBGTeam(0);
			player->SetBattlegroundId(0, BATTLEGROUND_TYPE_NONE);
			player->m_deathState = ALIVE;
			player->SetVisible(true);
			player->m_ExtraFlags &= ~ PLAYER_EXTRA_GM_ON;
			player->SetSpeed(MOVE_RUN, 1, true);
		}

		return true;
    }
};
class spec_login : public PlayerScript
{
    public:
		spec_login() : PlayerScript("spec_login") { }

	void OnLogin(Player* player)
	{
		if(player->IsVisible() == false && player->m_deathState == DEAD && player->GetSpeed(MOVE_RUN) == 4)
		{
			player->TeleportToBGEntryPoint();
			player->SetBGTeam(0);
			player->SetBattlegroundId(0, BATTLEGROUND_TYPE_NONE);
			player->m_deathState = ALIVE;
			player->SetVisible(true);
			player->m_ExtraFlags &= ~ PLAYER_EXTRA_GM_ON;
			player->SetSpeed(MOVE_RUN, 1, true);
		}
	}
};

void AddSC_spec()
	{
		new spec_login();
        new spec_comm();
		new spec_npc();
	}