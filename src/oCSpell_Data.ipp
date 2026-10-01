#pragma once

#include "oCSpell_Data.h"

namespace GOTHIC_NAMESPACE 
{
    oCSpell_Data::oCSpell_Data()
    {
        this->pd.spellId = -1;
        this->pd.spellType = oCSpell_Data::oCSpell_Type::SPELL_TYPE_PROJECTILE;
        this->pd.spellEnergyType = NPC_ATR_MANA;
        this->pd.spellIsInvestSpell = 0;

        this->instance = -1;
    }

    void oCSpell_Data::SetInstance(int inst)
    {
        int type, ele;
        this->instance = inst;
        this->name = parser->GetSymbolInfo(inst, type, ele);
    }
    
    void oCSpell_Data::RestoreParserInstance()
    {
        parser->SetInstance(this->instance, &this->pd);
    }

    int oCSpell_Data::GetDataSize()
    {
        return sizeof(this->pd);
    }

    void* oCSpell_Data::GetDataAdr()
    {
        return &this->pd;
    }

    int oCSpell_Data::GetEnergyType() const
    {
        return this->pd.spellEnergyType;
    }

    int oCSpell_Data::GetId() const
    {
        return this->pd.spellId;
    }

    int oCSpell_Data::GetType() const
    {
        return this->pd.spellType;
    }

    int oCSpell_Data::GetIsInvestSpell() const
    {
        return this->pd.spellIsInvestSpell;
    }
}