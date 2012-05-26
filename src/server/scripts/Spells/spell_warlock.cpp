/*
 * Copyright (C) 2010 - 2012 ProjectSkyfire <http://www.projectskyfire.org/>
 *
 * Copyright (C) 2011 - 2012 ArkCORE <http://www.arkania.net/>
 * Copyright (C) 2008 - 2012 TrinityCore <http://www.trinitycore.org/>
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

/*
 * Scripts for spells with SPELLFAMILY_WARLOCK and SPELLFAMILY_GENERIC spells used by warlock players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_warl_".
 */

#include "ScriptPCH.h"
#include "Spell.h"
#include "SpellAuraEffects.h"

enum WarlockSpells
{
    WARLOCK_DEMONIC_EMPOWERMENT_SUCCUBUS = 54435,
    WARLOCK_DEMONIC_EMPOWERMENT_VOIDWALKER = 54443,
    WARLOCK_DEMONIC_EMPOWERMENT_FELGUARD = 54508,
    WARLOCK_DEMONIC_EMPOWERMENT_FELHUNTER = 54509,
    WARLOCK_DEMONIC_EMPOWERMENT_IMP = 54444,
    WARLOCK_DARK_INTENT_EFFECT = 85767,
    //WARLOCK_IMPROVED_HEALTHSTONE_R1         = 18692,
    //WARLOCK_IMPROVED_HEALTHSTONE_R2         = 18693,
    WARLOCK_FELHUNTER_SHADOWBITE_R1 = 54049,
    WARLOCK_DEMONIC_PACT_SPELL = 53646
};

class spell_warl_banish : public SpellScriptLoader
{
public:
    spell_warl_banish() : SpellScriptLoader("spell_warl_banish") { }

    class spell_warl_banish_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warl_banish_SpellScript);

        bool Load()
        {
            _removed = false;
            return true;
        }

        void HandleBanish()
        {
            if (Unit* target = GetHitUnit())
            {
                if (target->GetAuraEffect(SPELL_AURA_SCHOOL_IMMUNITY, SPELLFAMILY_WARLOCK, 0, 0x08000000, 0))
                {
                    //No need to remove old aura since its removed due to not stack by current Banish aura
                    PreventHitDefaultEffect(EFFECT_0);
                    PreventHitDefaultEffect(EFFECT_1);
                    PreventHitDefaultEffect(EFFECT_2);
                    _removed = true;
                }
            }
        }

        void RemoveAura()
        {
            if (_removed)
                PreventHitAura();
        }

        void Register()
        {
            BeforeHit += SpellHitFn(spell_warl_banish_SpellScript::HandleBanish);
            AfterHit += SpellHitFn(spell_warl_banish_SpellScript::RemoveAura);
        }

        bool _removed;
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_warl_banish_SpellScript();
    }
};

// 47236 - Demonic Pact
class spell_warl_demonic_pact: public SpellScriptLoader {
public:
    spell_warl_demonic_pact() : SpellScriptLoader("spell_warl_demonic_pact") {}

    class spell_warl_demonic_pact_AuraScript: public AuraScript 
    {
        PrepareAuraScript(spell_warl_demonic_pact_AuraScript);

        bool Validate(SpellEntry const * /*spellEntry*/) 
        {
            if (!sSpellStore.LookupEntry(WARLOCK_DEMONIC_PACT_SPELL))
                return false;

            return true;
        }

        void HandleEffectApply(AuraEffect const * aurEff, AuraEffectHandleModes /*mode*/) 
        {
            Unit* target = GetTarget();

            if (target->ToPlayer())
                return;

            if (Unit *caster = aurEff->GetBase()->GetCaster()->GetOwner())
                if (caster->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_WARLOCK, 3220, 0))
                    if (target->isPet())
                        target->CastSpell(target, WARLOCK_DEMONIC_PACT_SPELL, true, NULL, aurEff);
        }

        void Register() 
        {
            OnEffectApply += AuraEffectApplyFn(spell_warl_demonic_pact_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript *GetAuraScript() const {
        return new spell_warl_demonic_pact_AuraScript();
    }
};

// 77799 Fel Flame
class spell_warl_fel_flame: public SpellScriptLoader {
public:
    spell_warl_fel_flame() : SpellScriptLoader("spell_warl_fel_flame") {}

    class spell_warl_fel_flame_SpellScript: public SpellScript {
        PrepareSpellScript(spell_warl_fel_flame_SpellScript)

        void HandleOnHit() 
        {
            Unit* unitTarget = GetHitUnit();
            Unit* caster = GetCaster();

            // Immolate
            if (Aura* aur = unitTarget->GetOwnedAura(348, caster->GetGUID()))
            {
                if (aur->GetDuration() + 6000 > 15000)
                    aur->SetDuration(15000);
                else
                    aur->SetDuration(aur->GetDuration() + 6000);
            }

            //Unstable Affliction
            if (Aura* aur = unitTarget->GetOwnedAura(30108, caster->GetGUID()))
            {
                if (aur->GetDuration() + 6000 > 15000)
                    aur->SetDuration(15000);
                else
                    aur->SetDuration(aur->GetDuration() + 6000);
            }
        }

        void Register() {
            OnHit += SpellHitFn(spell_warl_fel_flame_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const {
        return new spell_warl_fel_flame_SpellScript();
    }
};

// 47193 Demonic Empowerment
class spell_warl_demonic_empowerment: public SpellScriptLoader
{
public:
    spell_warl_demonic_empowerment () :
            SpellScriptLoader("spell_warl_demonic_empowerment")
    {
    }

    class spell_warl_demonic_empowerment_SpellScript: public SpellScript
    {
        PrepareSpellScript(spell_warl_demonic_empowerment_SpellScript)
        bool Validate (SpellEntry const * /*spellEntry*/)
        {
            if (!sSpellStore.LookupEntry(WARLOCK_DEMONIC_EMPOWERMENT_SUCCUBUS))
                return false;
            if (!sSpellStore.LookupEntry(WARLOCK_DEMONIC_EMPOWERMENT_VOIDWALKER))
                return false;
            if (!sSpellStore.LookupEntry(WARLOCK_DEMONIC_EMPOWERMENT_FELGUARD))
                return false;
            if (!sSpellStore.LookupEntry(WARLOCK_DEMONIC_EMPOWERMENT_FELHUNTER))
                return false;
            if (!sSpellStore.LookupEntry(WARLOCK_DEMONIC_EMPOWERMENT_IMP))
                return false;
            return true;
        }

        void HandleScriptEffect (SpellEffIndex /*effIndex*/)
        {
            if (Creature* targetCreature = GetHitCreature())
            {
                if (targetCreature->isPet())
                {
                    CreatureInfo const * ci = ObjectMgr::GetCreatureTemplate(targetCreature->GetEntry());
                    switch (ci->family)
                    {
                    case CREATURE_FAMILY_SUCCUBUS:
                        targetCreature->CastSpell(targetCreature, WARLOCK_DEMONIC_EMPOWERMENT_SUCCUBUS, true);
                        break;
                    case CREATURE_FAMILY_VOIDWALKER:
                    {
                        SpellEntry const* spellInfo = sSpellStore.LookupEntry(WARLOCK_DEMONIC_EMPOWERMENT_VOIDWALKER);
                        int32 hp = int32(targetCreature->CountPctFromMaxHealth(GetCaster()->CalculateSpellDamage(targetCreature, spellInfo, 0)));
                        targetCreature->CastCustomSpell(targetCreature, WARLOCK_DEMONIC_EMPOWERMENT_VOIDWALKER, &hp, NULL, NULL, true);
                        //unitTarget->CastSpell(unitTarget, 54441, true);
                        break;
                    }
                    case CREATURE_FAMILY_FELGUARD:
                        targetCreature->CastSpell(targetCreature, WARLOCK_DEMONIC_EMPOWERMENT_FELGUARD, true);
                        break;
                    case CREATURE_FAMILY_FELHUNTER:
                        targetCreature->CastSpell(targetCreature, WARLOCK_DEMONIC_EMPOWERMENT_FELHUNTER, true);
                        break;
                    case CREATURE_FAMILY_IMP:
                        targetCreature->CastSpell(targetCreature, WARLOCK_DEMONIC_EMPOWERMENT_IMP, true);
                        break;
                    }
                }
            }
        }

        void Register ()
        {
            OnEffectHit += SpellEffectFn(spell_warl_demonic_empowerment_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript () const
    {
        return new spell_warl_demonic_empowerment_SpellScript();
    }
};

// 47422 Everlasting Affliction
class spell_warl_everlasting_affliction: public SpellScriptLoader
{
public:
    spell_warl_everlasting_affliction () :
            SpellScriptLoader("spell_warl_everlasting_affliction")
    {
    }

    class spell_warl_everlasting_affliction_SpellScript: public SpellScript
    {
        PrepareSpellScript(spell_warl_everlasting_affliction_SpellScript)
        void HandleScriptEffect (SpellEffIndex /*effIndex*/)
        {
            if (Unit* unitTarget = GetHitUnit())
                // Refresh corruption on target
                if (AuraEffect* aur = unitTarget->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_WARLOCK, 0x2, 0, 0, GetCaster()->GetGUID()))
                    aur->GetBase()->RefreshDuration();
        }

        void Register ()
        {
            OnEffectHit += SpellEffectFn(spell_warl_everlasting_affliction_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript () const
    {
        return new spell_warl_everlasting_affliction_SpellScript();
    }
};

// 6201 Create Healthstone (and ranks)
class spell_warl_create_healthstone: public SpellScriptLoader
{
public:
    spell_warl_create_healthstone () :
            SpellScriptLoader("spell_warl_create_healthstone")
    {
    }

    class spell_warl_create_healthstone_SpellScript: public SpellScript
    {
        PrepareSpellScript(spell_warl_create_healthstone_SpellScript)
        static uint32 const iTypes[8][3];

        void HandleScriptEffect (SpellEffIndex effIndex)
        {
            if (Unit* unitTarget = GetHitUnit())
            {
                uint32 rank = 0;

                uint8 spellRank = sSpellMgr->GetSpellRank(GetSpellInfo()->Id);
                if (spellRank > 0 && spellRank <= 8)
                    CreateItem(effIndex, iTypes[spellRank - 1][rank]);
            }
        }

        void Register ()
        {
            OnEffectHit += SpellEffectFn(spell_warl_create_healthstone_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript () const
    {
        return new spell_warl_create_healthstone_SpellScript();
    }
};

uint32 const spell_warl_create_healthstone::spell_warl_create_healthstone_SpellScript::iTypes[8][3] =
{
{ 5512, 19004, 19005 },          // Minor Healthstone
{ 5511, 19006, 19007 },          // Lesser Healthstone
{ 5509, 19008, 19009 },          // Healthstone
{ 5510, 19010, 19011 },          // Greater Healthstone
{ 9421, 19012, 19013 },          // Major Healthstone
{ 22103, 22104, 22105 },          // Master Healthstone
{ 36889, 36890, 36891 },          // Demonic Healthstone
{ 36892, 36893, 36894 }          // Fel Healthstone
};

// 1120 Drain Soul
 class spell_warl_drain_soul: public SpellScriptLoader {
 public:
    spell_warl_drain_soul() :
            SpellScriptLoader("spell_warl_drain_soul") {} 

    class spell_warl_drain_soul_AuraScript: public AuraScript {
        PrepareAuraScript(spell_warl_drain_soul_AuraScript)

        void OnPeriodic(AuraEffect const* /*aurEff*/) 
        {
            // Pandemic
            if (AuraEffect* aurEff = GetCaster()->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELLFAMILY_WARLOCK, 4554, 1))
            {
                SpellEntry const* spellproto = aurEff->GetSpellProto();
                if (GetTarget()->HealthBelowPct(25))
                    if (roll_chance_i(spellproto->EffectBasePoints[0]))
                        if (AuraEffect* aur = GetTarget()->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_WARLOCK, 0, 0x100, 0, GetCaster()->GetGUID()))
                           aur->GetBase()->RefreshDuration();
            }
         }

        void OnRemove(AuraEffect const * aurEff, AuraEffectHandleModes /*mode*/) 
        {
            if (Unit* caster = aurEff->GetBase()->GetCaster())
                if (!GetTarget()->isAlive())
                    caster->CastSpell(caster, 79264, true);
        }

        void Register() {
            OnEffectRemove += AuraEffectRemoveFn(spell_warl_drain_soul_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_drain_soul_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        }
    };

    AuraScript* GetAuraScript() const {
        return new spell_warl_drain_soul_AuraScript();
    }
};

//80398 Dark Intent
class spell_warlock_dark_intent: public SpellScriptLoader
{
public:
    spell_warlock_dark_intent () :
            SpellScriptLoader("spell_warlock_dark_intent")
    {
    }

    class spell_warlock_dark_intent_SpellScript: public SpellScript
    {
        PrepareSpellScript(spell_warlock_dark_intent_SpellScript)

        void HandleScriptEffect (SpellEffIndex effIndex)
        {
            Unit* caster = GetCaster();
            Unit* target = GetHitUnit();

            if (!caster || !target)
                return;

            caster->CastSpell(target, WARLOCK_DARK_INTENT_EFFECT, true);
            target->CastSpell(caster, WARLOCK_DARK_INTENT_EFFECT, true);
        }

        void Register ()
        {
            OnEffectHit += SpellEffectFn(spell_warlock_dark_intent_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_TRIGGER_SPELL);
        }
    };

    SpellScript* GetSpellScript () const
    {
        return new spell_warlock_dark_intent_SpellScript();
    }
};

class spell_warl_immolate: public SpellScriptLoader
{
public:
    spell_warl_immolate () :
            SpellScriptLoader("spell_warl_immolate")
    {
    }

    class spell_warl_immolate_SpellScript: public SpellScript
    {
        PrepareSpellScript(spell_warl_immolate_SpellScript)

        bool Validate (SpellEntry const * /*spellEntry*/)
        {
            return true;
        }

        void HandleDummy (SpellEffIndex /*effIndex*/)
        {
            if (Unit * caster = GetCaster())
            {
                if (caster->GetTypeId() != TYPEID_PLAYER)
                    return;

                caster->ToPlayer()->KilledMonsterCredit(44175, 0);
            }
        }

        void Register ()
        {
            OnEffectHit += SpellEffectFn(spell_warl_immolate_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript () const
    {
        return new spell_warl_immolate_SpellScript();
    }
};

//27285 Seed of Corruption
class spell_warl_seed_of_corruption: public SpellScriptLoader
{
public:
    spell_warl_seed_of_corruption () :
            SpellScriptLoader("spell_warl_seed_of_corruption")
    {
    }

    class spell_warl_seed_of_corruption_SpellScript: public SpellScript
    {
        PrepareSpellScript(spell_warl_seed_of_corruption_SpellScript)
        ;

        void FilterTargets (std::list<Unit*>& unitList)
        {
            unitList.remove(GetTargetUnit());
        }

        void Register ()
        {
            OnUnitTargetSelect += SpellUnitTargetFn(spell_warl_seed_of_corruption_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_AREA_ENEMY_DST);
        }
    };

    SpellScript *GetSpellScript () const
    {
        return new spell_warl_seed_of_corruption_SpellScript();
    }
};

// 54049 Shadow Bite
class spell_warl_shadow_bite: public SpellScriptLoader
{
public:
    spell_warl_shadow_bite () :
            SpellScriptLoader("spell_warl_shadow_bite")
    {
    }

    class spell_warl_shadow_bite_SpellScript: public SpellScript
    {
        PrepareSpellScript(spell_warl_shadow_bite_SpellScript)
        bool Validate (SpellEntry const * /*spellEntry*/)
        {
            if (!sSpellStore.LookupEntry(WARLOCK_FELHUNTER_SHADOWBITE_R1))
                return false;
            return true;
        }

        void HandleScriptEffect (SpellEffIndex /*effIndex*/)
        {
            //Unit *caster = GetCaster();
            // Get DoTs on target by owner (15% increase by dot)
            // need to get this here from SpellEffects.cpp ?
            //damage *= float(100.f + 15.f * caster->getVictim()->GetDoTsByCaster(caster->GetOwnerGUID())) / 100.f;
        }

        // Improved Felhunter parts commented--deprecated. removed in cataclysm
        // For Improved Felhunter
        void HandleAfterHitEffect ()
        {
            Unit *caster = GetCaster();
            if (!caster)
            {
                return;
            };

            // break if our caster is not a pet
            if (!(caster->GetTypeId() == TYPEID_UNIT && caster->ToCreature()->isPet()))
            {
                return;
            };

            // break if pet has no owner and/or owner is not a player
            Unit *owner = caster->GetOwner();
            if (!(owner && (owner->GetTypeId() == TYPEID_PLAYER)))
            {
                return;
            };

            /*int32 amount;
             // rank 1 - 4%
             if (owner->HasAura(WARLOCK_IMPROVED_FELHUNTER_R1)) { amount = 5; };*/

            /*// rank 2 - 8%
             if (owner->HasAura(WARLOCK_IMPROVED_FELHUNTER_R2)) { amount = 9; };*/

            // Finally return the Mana to our Caster
            /*if (AuraEffect * aurEff = owner->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELLFAMILY_WARLOCK, 214, 0))
             caster->CastCustomSpell(caster, WARLOCK_IMPROVED_FELHUNTER_EFFECT, &amount, NULL, NULL, true, NULL, aurEff, caster->GetGUID());*/
        }

        void Register ()
        {
            //OnEffect += SpellEffectFn(spell_warl_shadow_bite_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            AfterHit += SpellHitFn(spell_warl_shadow_bite_SpellScript::HandleAfterHitEffect);
        }
    };

    SpellScript* GetSpellScript () const
    {
        return new spell_warl_shadow_bite_SpellScript();
    }
};

// DrainLife 
class spell_warl_drain_life: public SpellScriptLoader
{
public:
    spell_warl_drain_life () :
            SpellScriptLoader("spell_warl_drain_life")
    {
    }
    class spell_warl_drain_life_AuraScript: public AuraScript
    {
        PrepareAuraScript(spell_warl_drain_life_AuraScript)
        ;
        void OnPeriodic (AuraEffect const* /*aurEff*/)
        {
            int32 bp = 2;          // Normal, restore 2% of health
            // Check for Death's Embrace
            if (AuraEffect const* aurEff = GetCaster()->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_WARLOCK, 3223, 0))
                if (GetCaster()->HealthBelowPct(25))
                    bp += int32(aurEff->GetAmount());
            GetCaster()->CastCustomSpell(GetCaster(), 89653, &bp, NULL, NULL, true);
        }
        void Register ()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_drain_life_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        }
    };
    AuraScript* GetAuraScript () const
    {
        return new spell_warl_drain_life_AuraScript();
    }
};

// Life Tap
// Spell Id: 1454
class spell_warl_life_tap : public SpellScriptLoader
{
public:
    spell_warl_life_tap() : SpellScriptLoader("spell_warl_life_tap") { }

    class spell_warl_life_tap_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warl_life_tap_SpellScript);

        SpellCastResult CheckCast()
        {
            if (Unit* caster = GetCaster())
            {
                if (caster->CountPctFromMaxHealth(GetSpellInfo()->Effects[EFFECT_2].CalcValue()) >= caster->GetHealth()) // You cant kill yourself with this
                    return SPELL_FAILED_FIZZLE;

                return SPELL_CAST_OK;
            }
            return SPELL_FAILED_DONT_REPORT;
        }

        void HandleDummy(SpellEffIndex /*EffIndex*/)
        {
            if (Unit* caster = GetCaster())
            {
                int32 damage = int32(caster->CountPctFromMaxHealth(GetSpellInfo()->Effects[EFFECT_2].CalcValue()));
                int32 mana = 0;

                float multiplier = 1.2f;

                // Should not appear in combat log
                caster->ModifyHealth(-damage);

                // Improved Life Tap mod
                if (AuraEffect const* aurEff = caster->GetDummyAuraEffect(SPELLFAMILY_WARLOCK, 208, 0))
                    multiplier += int32(aurEff->GetAmount() / 100);

                mana = int32(damage * multiplier);
                caster->CastCustomSpell(caster, 31818, &mana, NULL, NULL, false);

                // Mana Feed
                int32 manaFeedVal = 0;
                if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELLFAMILY_WARLOCK, 1982, 0))
                    manaFeedVal = aurEff->GetAmount();

                if (manaFeedVal > 0)
                {
                    manaFeedVal = int32(mana * manaFeedVal / 100);
                    caster->CastCustomSpell(caster, 32553, &manaFeedVal, NULL, NULL, true, NULL);
                }
            }
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_warl_life_tap_SpellScript::CheckCast);
            OnEffectHitTarget += SpellEffectFn(spell_warl_life_tap_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_warl_life_tap_SpellScript();
    }
};

// Fear
// Spell Id: 5782
class spell_warl_fear : public SpellScriptLoader
{
    public:
        spell_warl_fear() : SpellScriptLoader("spell_warl_fear") { }

        class spell_warl_fear_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_fear_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                // Check for Improved Fear
                if (AuraEffect* aurEff = GetCaster()->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_WARLOCK, 98, 0))
                {
                    uint32 spellId = 0;
                    switch (aurEff->GetId())
                    {
                        case 53759:
                            spellId = 60947;
                            break;
                        case 53754:
                            spellId = 60946;
                            break;
                    }
                    if (spellId)
                        GetCaster()->CastSpell(GetTarget(), spellId, true);
                }
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_warl_fear_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_FEAR, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warl_fear_AuraScript();
        }
};

void AddSC_warlock_spell_scripts ()
{
    new spell_warl_banish();
    new spell_warl_demonic_empowerment();
    new spell_warl_demonic_pact();
    new spell_warl_everlasting_affliction();
    new spell_warl_create_healthstone();
    new spell_warl_drain_soul();
    new spell_warl_life_tap();
    new spell_warlock_dark_intent();
    new spell_warl_immolate();
    new spell_warl_seed_of_corruption();
    new spell_warl_shadow_bite();
    new spell_warl_drain_life();
    new spell_warl_fear();
}