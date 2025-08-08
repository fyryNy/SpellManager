namespace GOTHIC_NAMESPACE
{
    oCSpell* oCNpc::GetActiveSpellControl_Union()
    {
        auto spellNode = this->activeSpells.GetNextInList();
        while(spellNode)
        {
            auto spell = spellNode->GetData();
            spellNode = spellNode->GetNextInList();

            auto spellData = sdManager->GetSpellData(spell->spellID);
            if(spellData && spellData->GetType() == oCSpell_Data::oCSpell_Type::SPELL_TYPE_CONTROL)
            {
                return spell;
            }
        }

        return NULL;
    }

    zBOOL oCSpell::CastControl_Union()
    {
        if(!this->spellTargetNpc || !this->spellCasterNpc)
        {
            return TRUE;
        }

        if(this->saveNpc)
        {
            zRELEASE(this->saveNpc);
        }
        
        this->saveNpc = this->spellCasterNpc;
        zADDREF(this->saveNpc);

        this->spellCasterNpc->ModifyBodyState(BS_MOD_CONTROLLING, 0);

        /*if(!this->spellTargetNpc->GetAnictrl())
        {
            return TRUE;
        }*/

        this->spellTargetNpc->GetAnictrl()->SearchStandAni(FALSE);
        this->spellTargetNpc->SetAsPlayer();
        this->spellTargetNpc->SetBodyStateModifier(BS_MOD_CONTROLLED);

        this->spellTargetNpc->InsertActiveSpell(this);

        this->canBeDeleted = FALSE;

        return FALSE;
    }

    void oCSpell::EndControl_Union()
    {
        if(!this->saveNpc || !this->spellTargetNpc)
        {
            return;
        }

        this->saveNpc->SetAsPlayer();
        this->saveNpc->ModifyBodyState(0, BS_MOD_CONTROLLING);

        this->spellTargetNpc->state.EndCurrentState();
        this->spellTargetNpc->state.StartAIState("ZS_CONTROLLED", FALSE, 0, 0.0f, FALSE);

        this->saveNpc->state.ClearAIState();
        this->saveNpc->StandUp(FALSE, TRUE);
        this->saveNpc->SetSleeping(FALSE);

        if(this->controlWarnFX)
        {
            this->controlWarnFX->Stop(TRUE);
            zRELEASE(this->controlWarnFX);
        }

        this->effect = this->CreateEffect();
        this->effect->Init(this->saveNpc, NULL);
        this->effect->SetSpellTargetTypes(this->targetCollectType);
        this->effect->Cast(TRUE);
        zRELEASE(effect);

        this->spellCasterNpc->GetEM()->OnMessage(zNEW(oCMsgWeapon)(oCMsgWeapon::EV_REMOVEWEAPON, 0, FALSE), this->spellCasterNpc);
    }

    void oCSpell::CheckControl_Union()
    {
        if(!this->saveNpc || !this->spellTargetNpc)
        {
            return;
        }
        
        if(this->spellTargetNpc->attribute[NPC_ATR_HITPOINTS] <= 0 || this->spellTargetNpc->GetDistanceToVob2(*this->saveNpc) >= (4000.0f * 4000.0f))
        {
            this->canBeDeleted = TRUE;
            this->EndTimedEffect();
        }
        else if(this->spellTargetNpc->GetDistanceToVob2(*this->saveNpc) >= (3500.0f * 3500.0f))
        {
            if(!this->controlWarnFX)
            {
                this->controlWarnFX = oCVisualFX::CreateAndPlay("CONTROL_LEAVERANGEFX", this->spellTargetNpc->GetPositionWorld(), nullptr, 1, 0.0f, 0, FALSE);
            }
        }
        else if(this->controlWarnFX)
        {
            this->controlWarnFX->Stop(TRUE);
            zRELEASE(this->controlWarnFX);
        }
    }

    void __fastcall oCNpc_OnDamage_Events_Control(::Union::Registers& reg)
    {
    #if ENGINE <= Engine_G1A
        auto npc = reinterpret_cast<oCNpc*>(reg.ebx);
    #else
        auto npc = reinterpret_cast<oCNpc*>(reg.edi);
    #endif

        if(!npc)
        {
            return;
        }

    #if ENGINE == Engine_G1
        auto& descDamage = *reinterpret_cast<oCNpc::oSDamageDescriptor*>(reg.edi);
    #elif ENGINE == Engine_G1A
        auto& descDamage = *reinterpret_cast<oCNpc::oSDamageDescriptor*>(reg.edx);
    #else
        auto& descDamage = *reinterpret_cast<oCNpc::oSDamageDescriptor*>(reg.ebp);
    #endif

        oCSpell* spell = nullptr;
        oCNpc* npcTarget = nullptr;

        if(descDamage.pNpcAttacker)
        {
            spell = descDamage.pNpcAttacker->GetActiveSpellControl_Union();
            npcTarget = spell ? spell->spellTargetNpc : nullptr;
        }

        if(spell && npcTarget && npc->attribute[NPC_ATR_HITPOINTS] <= 0)
        {
            spell->EndTimedEffect();
        }

        zBOOL isControlling = npc->HasBodyStateModifier(BS_MOD_CONTROLLING);

        if(isControlling)
        {
            spell = player->GetActiveSpellControl_Union();
            spell->EndTimedEffect();
        }
    }
    auto PartialHook__oCNpc_OnDamage_Events_Control = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x00747610, 0x00788421, 0x00792414, 0x0067b7c4)), &oCNpc_OnDamage_Events_Control);
}