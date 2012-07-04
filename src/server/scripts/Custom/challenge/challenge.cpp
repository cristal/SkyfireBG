/*
 * Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
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

#include "ScriptPCH.h"
#include "challenge.h"

Challenge::Challenge()
{
};

bool Challenge::AddDuelGroups(uint64 mover, uint64 opponent)
{
    push_back(std::pair<uint64, uint64>(mover, opponent));
    return true;
}

UnitPairlist::iterator Challenge::FindPlayerPair(uint64 val)
{
    for (iterator itr = begin(); itr != end(); ++itr)
    {
        if ((*itr).first == val || (*itr).second == val)
            return itr;
    }

    return end();
}

uint64 Challenge::GetOpponentFor(uint64 mover)
{
    UnitPairlist::iterator itr = FindPlayerPair(mover);
    ASSERT (itr!=end());
    return (*itr).first == mover ? (*itr).second: (*itr).first;
}