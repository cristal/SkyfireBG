/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
Name: wp_commandscript
%Complete: 100
Comment: All wp related commands
Category: commandscripts
EndScriptData */

#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "MapManager.h"
#include "Chat.h"
#include "Group.h"
#include "BattlegroundMgr.h"
#include "Battleground.h"
#include "challenge.h"

class challenge_commandscript : public CommandScript
{
public:
    challenge_commandscript() : CommandScript("challenge_commandscript") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand ChallengeCommandTable[] =
        {
            { "player",         SEC_GAMEMASTER,     false, &HandleChallengeRequest,                "", NULL },
            { "agree",          SEC_GAMEMASTER,     false, &HandleAgreeRequest,                    "", NULL },
            { NULL,             0,                  false, NULL,                                   "", NULL }
        };
        static ChatCommand commandTable[] =
        {
            { "challenge",             SEC_GAMEMASTER,     false, NULL,                                   "", ChallengeCommandTable },
            { NULL,             0,                  false, NULL,                                   "", NULL }
        };
        return commandTable;
    }

    static bool HandleChallengeRequest(ChatHandler* handler, const char* args)
    {
		Player* target;
		uint64 target_guid;
		std::string target_name;
		if (!handler->extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
			return false;

		Player* _player = handler->GetSession()->GetPlayer();
		if (target == _player || target_guid == _player->GetGUID())
		{
			handler->SendSysMessage(12);
			handler->SetSentErrorMessage(true);
			return false;
		}

        BattlegroundBracketId bracket_id = BattlegroundBracketId(14);
        Battleground* bg_template = sBattlegroundMgr->GetBattlegroundTemplate(BATTLEGROUND_AA);
        if (!bg_template)
        {
            sLog->outError("Battleground: Update: bg template not found for custom duel arena", BATTLEGROUND_AA);
            return false;
        }

        PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketById(bg_template->GetMapId(), bracket_id);
        if (!bracketEntry)
        {
            sLog->outError("Battleground: Update: bg bracket entry not found for map %u bracket id %u", bg_template->GetMapId(), bracket_id);
            return false;
        }

        _player->setDuelState(true);
        target->setDuelState(true);
        BattlegroundQueue& bgQueue = sBattlegroundMgr->m_BattlegroundQueues[BATTLEGROUND_QUEUE_1v1];
        GroupQueueInfo* ginfo = bgQueue.AddGroup(_player, NULL, BATTLEGROUND_AA, bracketEntry, 1, false, false, 0, 0);
        GroupQueueInfo* ginfo2 = bgQueue.AddGroup(target, NULL, BATTLEGROUND_AA, bracketEntry, 1, false, false, 0, 0);
        bgQueue.BattleGroundDuelQueueUpdate(ginfo, ginfo2);
    }

    static bool HandleAgreeRequest(ChatHandler* handler, const char* args)
    {
        return true;
    }
};

void AddSC_challenge_commandscript()
{
    new challenge_commandscript();
}
