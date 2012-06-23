/*
 * Copyright (C) 2011-2012 Project SkyFire <http://www.projectskyfire.org/>
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
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
 * Scripts for spells with SPELLFAMILY_SHAMAN and SPELLFAMILY_GENERIC spells used by shaman players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_sha_".
 */

#include "ScriptPCH.h"
#include "SpellAuraEffects.h"

enum ShamanSpells
{
    SHAMAN_SPELL_GLYPH_OF_MANA_TIDE     = 55441,
    SHAMAN_SPELL_MANA_TIDE_TOTEM        = 16191,
    SHAMAN_SPELL_FIRE_NOVA_R1           = 1535,
    SHAMAN_SPELL_FIRE_NOVA_TRIGGERED_R1 = 8349,
    SHAMAN_SPELL_SATED                  = 57724,
    SHAMAN_SPELL_EXHAUSTION             = 57723,

    //For Earthen Power
    SHAMAN_TOTEM_SPELL_EARTHBIND_TOTEM  = 6474, //Spell casted by totem
    SHAMAN_TOTEM_SPELL_EARTHEN_POWER    = 59566, //Spell witch remove snare effect


    SHAMAN_SPELL_EARTH_SHOCK = 8042,
    SHAMAN_SPELL_FULMINATION = 88766,
    SHAMAN_SPELL_FULMINATION_TRIGGERED = 88767,
    SHAMAN_SPELL_FULMINATION_INFO = 95774,
    SHAMAN_SPELL_LIGHTNING_SHIELD_PROC = 26364,
    SHAMAN_TOTEM_SPELL_EARTHS_GRASP = 51485,
    SHAMAN_TOTEM_SPELL_EARTHGRAB = 64695,

    SHAMAN_TOTEM_SPELL_TOTEMIC_WRATH = 77746,
    SHAMAN_TOTEM_SPELL_TOTEMIC_WRATH_AURA = 77747,
    SHAMAN_SPELL_UNLEASH_ELEMENTS = 73680,

    SHAMAN_SPELL_EARTHQUAKE_KNOCKDOWN       = 77505,
    SHAMAN_SPELL_SEARING_FLAMES = 77661,


    SHAMAN_SPELL_CLEANSING_WATERS       = 86962,
    SHAMAN_SPELL_IMPROVED_CLEANSE_SPIRIT = 77130,
    SHAMAN_SPELL_EARTH_SHIELD           = 974,
    SHAMAN_SPELL_GLYPH_OF_EARTH_SHIELD  = 64261,
    SHAMAN_SPELL_NATURES_BLESSING       = 30869,
    SHAMAN_SPELL_IMPROVED_SHIELDS       = 51881,

};
class spell_sha_unleash_elements : public SpellScriptLoader
{
public:
    spell_sha_unleash_elements() : SpellScriptLoader("spell_sha_unleash_elements") { }

    class spell_sha_unleash_elements_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_sha_unleash_elements_SpellScript)
        bool Validate(SpellEntry const * spellEntry)
        {
            if (!sSpellStore.LookupEntry(SHAMAN_SPELL_UNLEASH_ELEMENTS))
                return false;
           return true;
        }
        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            if(!caster)
                return;
            Player* plr = caster->ToPlayer();
            if(!plr)
                return;

			if(!GetExplTargetUnit())
               return;

            Item *weapons[2];
            weapons[0] = plr->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
            weapons[1] = plr->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            for(int i = 0; i < 2; i++)
            {
                if(!weapons[i])
                    continue;

                uint32 unleashSpell = 0;
				Unit *target = GetExplTargetUnit();
                bool hostileTarget = plr->IsHostileTo(target);
                bool hostileSpell = true;

                switch (weapons[i]->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT))
                {
                    case 3345: // Earthliving Weapon
                        unleashSpell = 73685; //Unleash Life
                        hostileSpell = false;
                        break;
                    case 5: // Flametongue Weapon
                        unleashSpell = 73683; // Unleash Flame
                        break;
                    case 2: // Frostbrand Weapon
                        unleashSpell = 73682; // Unleash Frost
                        break;
                    case 3021: // Rockbiter Weapon
                        unleashSpell = 73684; // Unleash Earth
                        break;
                    case 283: // Windfury Weapon
                        unleashSpell = 73681; // Unleash Wind
                        break;
                }
                if(hostileSpell && !hostileTarget)
                    return; // don't allow to attack non-hostile targets. TODO: check this before cast

                if(!hostileSpell && hostileTarget)
                    target = plr;   // heal ourselves instead of the enemy

                if(unleashSpell)
                {
                    plr->CastSpell(target, unleashSpell, true);
                }
            }
        }

        void Register()
        {
			OnEffectHit += SpellEffectFn(spell_sha_unleash_elements_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_sha_unleash_elements_SpellScript();
    }
};

// 77478 - Earthquake
class spell_sha_earthquake : public SpellScriptLoader
{
    public:
        spell_sha_earthquake() : SpellScriptLoader("spell_sha_earthquake") { }

        class spell_sha_earthquake_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_earthquake_SpellScript);

            bool Validate(SpellEntry const* /*spellInfo*/)
            {
                if (!sSpellStore.LookupEntry(SHAMAN_SPELL_EARTHQUAKE_KNOCKDOWN))
                    return false;
                return true;
            }

            void OnQuake()
            {
				int32 chance = GetSpellInfo()->Effects[EFFECT_1].CalcValue();
			    Unit* target = GetHitUnit();
				if (roll_chance_i(chance))
				    GetCaster()->CastSpell(target, SHAMAN_SPELL_EARTHQUAKE_KNOCKDOWN, true);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_sha_earthquake_SpellScript::OnQuake);
            }
        };
        
        SpellScript* GetSpellScript() const
        {
            return new spell_sha_earthquake_SpellScript();
        }
};

// 16191 - Mana Tide
class spell_sha_mana_tide : public SpellScriptLoader
{
    public:
        spell_sha_mana_tide() : SpellScriptLoader("spell_sha_mana_tide") { }

        class spell_sha_mana_tide_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_mana_tide_AuraScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SHAMAN_SPELL_MANA_TIDE_TOTEM))
                    return false;
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 &amount, bool & /*canBeRecalculated*/)
            {
                // 400% of caster's spirit
                // Caster is totem, we need owner
                if (Unit* owner = GetCaster()->GetOwner())
                    amount = int32(owner->GetStat(STAT_SPIRIT) * 4.0f);
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_mana_tide_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_STAT);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_mana_tide_AuraScript();
        }
};

// 51474 - Astral shift
class spell_sha_astral_shift : public SpellScriptLoader
{
    public:
        spell_sha_astral_shift() : SpellScriptLoader("spell_sha_astral_shift") { }

        class spell_sha_astral_shift_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_astral_shift_AuraScript);

            uint32 absorbPct;

            bool Load()
            {
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
            {
                // Set absorbtion amount to unlimited
                amount = -1;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                // reduces all damage taken while stun, fear or silence
                if (GetTarget()->GetUInt32Value(UNIT_FIELD_FLAGS) & (UNIT_FLAG_FLEEING | UNIT_FLAG_SILENCED) || (GetTarget()->GetUInt32Value(UNIT_FIELD_FLAGS) & (UNIT_FLAG_STUNNED) && GetTarget()->HasAuraWithMechanic(1<<MECHANIC_STUN)))
                    absorbAmount = CalculatePctN(dmgInfo.GetDamage(), absorbPct);
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_astral_shift_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_sha_astral_shift_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_astral_shift_AuraScript();
        }
};

// 1535 Fire Nova
class spell_sha_fire_nova : public SpellScriptLoader
{
    public:
        spell_sha_fire_nova() : SpellScriptLoader("spell_sha_fire_nova") { }

        class spell_sha_fire_nova_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_fire_nova_SpellScript);

            bool Validate(SpellInfo const* spellEntry)
            {
                if (!sSpellMgr->GetSpellInfo(SHAMAN_SPELL_FIRE_NOVA_R1))
                    return false;
                if (sSpellMgr->GetFirstSpellInChain(SHAMAN_SPELL_FIRE_NOVA_R1) != sSpellMgr->GetFirstSpellInChain(spellEntry->Id))
                    return false;

                uint8 rank = sSpellMgr->GetSpellRank(spellEntry->Id);
                if (!sSpellMgr->GetSpellWithRank(SHAMAN_SPELL_FIRE_NOVA_TRIGGERED_R1, rank, true))
                    return false;
                return true;
            }

            SpellCastResult CheckFireTotem()
            {
                // fire totem
                if (!GetCaster()->m_SummonSlot[1])
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_HAVE_FIRE_TOTEM);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                uint8 rank = sSpellMgr->GetSpellRank(GetSpellInfo()->Id);
                if (uint32 spellId = sSpellMgr->GetSpellWithRank(SHAMAN_SPELL_FIRE_NOVA_TRIGGERED_R1, rank))
                {
                    Creature* totem = caster->GetMap()->GetCreature(caster->m_SummonSlot[1]);
                    if (totem && totem->isTotem())
                        totem->CastSpell(totem, spellId, true);
                }
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_sha_fire_nova_SpellScript::CheckFireTotem);
                OnEffectHitTarget += SpellEffectFn(spell_sha_fire_nova_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_fire_nova_SpellScript();
        }
};

// 39610 Mana Tide Totem
class spell_sha_mana_tide_totem : public SpellScriptLoader
{
    public:
        spell_sha_mana_tide_totem() : SpellScriptLoader("spell_sha_mana_tide_totem") { }

        class spell_sha_mana_tide_totem_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_mana_tide_totem_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SHAMAN_SPELL_GLYPH_OF_MANA_TIDE) || !sSpellMgr->GetSpellInfo(SHAMAN_SPELL_MANA_TIDE_TOTEM))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                    if (Unit* unitTarget = GetHitUnit())
                    {
                        if (unitTarget->getPowerType() == POWER_MANA)
                        {
                            int32 effValue = GetEffectValue();
                            // Glyph of Mana Tide
                            if (Unit* owner = caster->GetOwner())
                                if (AuraEffect* dummy = owner->GetAuraEffect(SHAMAN_SPELL_GLYPH_OF_MANA_TIDE, 0))
                                    effValue += dummy->GetAmount();
                            // Regenerate 6% of Total Mana Every 3 secs
                            int32 effBasePoints0 = int32(CalculatePctN(unitTarget->GetMaxPower(POWER_MANA), effValue));
                            caster->CastCustomSpell(unitTarget, SHAMAN_SPELL_MANA_TIDE_TOTEM, &effBasePoints0, NULL, NULL, true, NULL, NULL, GetOriginalCaster()->GetGUID());
                        }
                    }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_sha_mana_tide_totem_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_mana_tide_totem_SpellScript();
        }
};

// 6474 - Earthbind Totem - Fix Talent:Earthen Power
class spell_sha_earthbind_totem : public SpellScriptLoader
{
    public:
        spell_sha_earthbind_totem() : SpellScriptLoader("spell_sha_earthbind_totem") { }

        class spell_sha_earthbind_totem_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_earthbind_totem_AuraScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SHAMAN_TOTEM_SPELL_EARTHBIND_TOTEM))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SHAMAN_TOTEM_SPELL_EARTHEN_POWER))
                    return false;
                return true;
            }

            void HandleEffectPeriodic(AuraEffect const* aurEff)
            {
                Unit* target = GetTarget();
                if (Unit *caster = aurEff->GetBase()->GetCaster()->GetOwner())
                    if (AuraEffect* aur = caster->GetDummyAuraEffect(SPELLFAMILY_SHAMAN, 2289, 0))
                        if (roll_chance_i(aur->GetBaseAmount()))
                            target->CastSpell(caster, SHAMAN_TOTEM_SPELL_EARTHEN_POWER, true, NULL, aurEff);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_earthbind_totem_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_earthbind_totem_AuraScript();
        }
};

class spell_sha_bloodlust : public SpellScriptLoader
{
    public:
        spell_sha_bloodlust() : SpellScriptLoader("spell_sha_bloodlust") { }

        class spell_sha_bloodlust_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_bloodlust_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SHAMAN_SPELL_SATED))
                    return false;
                return true;
            }

            void RemoveInvalidTargets(std::list<Unit*>& targets)
            {
                targets.remove_if (SkyFire::UnitAuraCheck(true, SHAMAN_SPELL_SATED));
            }

            void ApplyDebuff()
            {
                GetHitUnit()->CastSpell(GetHitUnit(), SHAMAN_SPELL_SATED, true);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_sha_bloodlust_SpellScript::RemoveInvalidTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_sha_bloodlust_SpellScript::RemoveInvalidTargets, EFFECT_1, TARGET_UNIT_CASTER_AREA_RAID);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_sha_bloodlust_SpellScript::RemoveInvalidTargets, EFFECT_2, TARGET_UNIT_CASTER_AREA_RAID);
                AfterHit += SpellHitFn(spell_sha_bloodlust_SpellScript::ApplyDebuff);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_bloodlust_SpellScript();
        }
};

class spell_sha_heroism : public SpellScriptLoader
{
    public:
        spell_sha_heroism() : SpellScriptLoader("spell_sha_heroism") { }

        class spell_sha_heroism_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sha_heroism_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SHAMAN_SPELL_EXHAUSTION))
                    return false;
                return true;
            }

            void RemoveInvalidTargets(std::list<Unit*>& targets)
            {
                targets.remove_if (SkyFire::UnitAuraCheck(true, SHAMAN_SPELL_EXHAUSTION));
            }

            void ApplyDebuff()
            {
                GetHitUnit()->CastSpell(GetHitUnit(), SHAMAN_SPELL_EXHAUSTION, true);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_sha_heroism_SpellScript::RemoveInvalidTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_sha_heroism_SpellScript::RemoveInvalidTargets, EFFECT_1, TARGET_UNIT_CASTER_AREA_RAID);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_sha_heroism_SpellScript::RemoveInvalidTargets, EFFECT_2, TARGET_UNIT_CASTER_AREA_RAID);
                AfterHit += SpellHitFn(spell_sha_heroism_SpellScript::ApplyDebuff);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sha_heroism_SpellScript();
        }
};

// 73920 - Healing Rain
class spell_sha_healing_rain : public SpellScriptLoader
{
    public:
        spell_sha_healing_rain() : SpellScriptLoader("spell_sha_healing_rain") { }

        class spell_sha_healing_rain_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sha_healing_rain_AuraScript);

            void OnTick(AuraEffect const* aurEff)
            {
                if (DynamicObject* dynObj = GetCaster()->GetDynObject(73920))
                    GetCaster()->CastSpell(dynObj->GetPositionX(), dynObj->GetPositionY(), dynObj->GetPositionZ(), 73921, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_healing_rain_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sha_healing_rain_AuraScript();
        }
};

// 974 Earth Shield
class spell_sha_earth_shield : public SpellScriptLoader
{
public:
    spell_sha_earth_shield() : SpellScriptLoader("spell_sha_earth_shield") {}
        
    class spell_sha_earth_shield_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_sha_earth_shield_SpellScript)
        bool Validate(SpellEntry const* /*spellEntry*/)
        {
            if (!sSpellStore.LookupEntry(SHAMAN_SPELL_GLYPH_OF_EARTH_SHIELD))
            return false;

            if (!sSpellStore.LookupEntry(SHAMAN_SPELL_NATURES_BLESSING))
            return false;

            if (!sSpellStore.LookupEntry(SHAMAN_SPELL_IMPROVED_SHIELDS))
            return false;

            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            if (caster->GetTypeId() != TYPEID_PLAYER)
                return;

            Unit* Target = GetHitUnit();
            if (!Target || !Target->isAlive())
                return;

            uint8 rank = sSpellMgr->GetSpellRank(GetSpellInfo()->Id);
        }

        void Register()
        {
            OnEffect += SpellEffectFn(spell_sha_earth_shield_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_sha_earth_shield_SpellScript;
    }
};

class spell_sha_cleansing_waters : public SpellScriptLoader
{
public:
     spell_sha_cleansing_waters() : SpellScriptLoader("spell_sha_cleansing_waters") {}      

     class spell_sha_cleansing_waters_SpellScript : public SpellScript
     {
          PrepareSpellScript(spell_sha_cleansing_waters_SpellScript)

          bool Validate(SpellEntry const* /*spellEntry*/)
          {
              if (!sSpellStore.LookupEntry(SHAMAN_SPELL_IMPROVED_CLEANSE_SPIRIT))
              return false;
          }

          void HandleDummy(SpellEffIndex /*effIndex*/)
          {
              Unit* caster = GetCaster();
              if (caster->GetTypeId() != TYPEID_PLAYER)
              return;       

              if(Unit* target = GetHitUnit())
              GetCaster()->CastSpell(target, SHAMAN_SPELL_CLEANSING_WATERS, true);
          }             
     
          void Register()
          {
              OnEffect += SpellEffectFn(spell_sha_cleansing_waters_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
          }        
};

    SpellScript *GetSpellScript() const
    {
        return new spell_sha_cleansing_waters_SpellScript();
    }
};

void AddSC_shaman_spell_scripts()
{
    new spell_sha_mana_tide();
    new spell_sha_astral_shift();
    new spell_sha_fire_nova();
    new spell_sha_earthbind_totem();
    new spell_sha_bloodlust();
    new spell_sha_heroism();
    new spell_sha_healing_rain();
    new spell_sha_earthquake();
	new spell_sha_unleash_elements();
    new spell_sha_cleansing_waters();
    new spell_sha_earth_shield();
}
