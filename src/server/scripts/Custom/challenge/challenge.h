#ifndef CHALLENGE_H
#define CHALLENGE_H
#pragma once
typedef std::list<std::pair<uint64, uint64> > UnitPairlist;

class Challenge: public UnitPairlist
{
public:
    static Challenge& Instance()
    {
        // lazy init
        static Challenge m_Instance;
        return m_Instance;
    }
    bool AddDuelGroups(uint64 mover, uint64 opponent);
    uint64 GetOpponentFor(uint64 mover);
    UnitPairlist::iterator FindPlayerPair(uint64 val);
private:
    Challenge();
};
#endif