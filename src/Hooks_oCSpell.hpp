namespace GOTHIC_NAMESPACE 
{
    void __fastcall oCSpell_CastSpecificSpell(::Union::Registers& reg)
    {
    #if ENGINE == Engine_G1A
        auto spell = reinterpret_cast<oCSpell*>(reg.ebp);
    #else
        auto spell = reinterpret_cast<oCSpell*>(reg.esi);
    #endif

        auto spellData = sdManager->GetSpellData(spell->spellID);
        if(!spellData)
        {
            return;
        }

        if(spellData->GetType() == oCSpell_Data::oCSpell_Type::SPELL_TYPE_CONTROL)
        {

            if(spell->CastControl_Union())
            {
                reg.eip = zSwitch(0x0047f312, 0x00489e3a, 0x004857b9, 0x00486dd9); // return 1
            }
            else
            {
                reg.eip = zSwitch(0x0047f30e, 0x00489e36, 0x004857cc, 0x00486dec); // return 0
            }
            
            return;
        }

        if(spellData->GetType() == oCSpell_Data::oCSpell_Type::SPELL_TYPE_TRANSFORM)
        {
            reg.eip = zSwitch(0x0047eefd, 0x00489986, 0x00485422, 0x00486a42);
        }
    }
	auto PartialHook__oCSpell_CastSpecificSpell = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x0047ed3d, 0x00489787, 0x00485413, 0x00486a33)), &oCSpell_CastSpecificSpell);

    void __fastcall oCSpell_DeleteCaster(::Union::Registers& reg)
    {
        auto spellId = *reinterpret_cast<int*>(reg.ecx + 0x54);

        auto spellData = sdManager->GetSpellData(spellId);
        if(!spellData)
        {
            return;
        }

        if(spellData->GetType() != oCSpell_Data::oCSpell_Type::SPELL_TYPE_TRANSFORM)
        {
            return;
        }

        reg.eip = zSwitch(0x0047fb65, 0x0048a765, 0x00485d0d, 0x0048732d);
    }
	auto PartialHook__oCSpell_DeleteCaster = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x0047fb53, 0x0048a753, 0x00485d03, 0x00487323)), &oCSpell_DeleteCaster);

    void __fastcall oCSpell_EndTimedEffect(::Union::Registers& reg)
    {
    #if ENGINE == Engine_G1A
        auto spell = reinterpret_cast<oCSpell*>(reg.ebp);
    #else
        auto spell = reinterpret_cast<oCSpell*>(reg.esi);
    #endif

        auto spellData = sdManager->GetSpellData(spell->spellID);
        if(!spellData)
        {
            return;
        }

        if(spellData->GetType() == oCSpell_Data::oCSpell_Type::SPELL_TYPE_CONTROL)
        {
            spell->EndControl_Union();
            return;
        }

        if(spellData->GetType() == oCSpell_Data::oCSpell_Type::SPELL_TYPE_TRANSFORM)
        {
            reg.eip = zSwitch(0x0047f510, 0x0048a047, 0x004858ca, 0x00486eea);
        }
    }
	auto PartialHook__oCSpell_EndTimedEffect = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x0047f3fc, 0x00489f39, 0x004858bb, 0x00486edb)), &oCSpell_EndTimedEffect);

    void __fastcall oCSpell_InitValues(::Union::Registers& reg)
    {
        auto spell = reinterpret_cast<oCSpell*>(reg.esi);

        auto spellData = sdManager->GetSpellData(spell->spellID);
        if(!spellData)
        {
            return;
        }

        spell->spellEnergy = spellData->GetEnergyType();
    }
	auto PartialHook__oCSpell_InitValues = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x0047bd1a, 0x004863a8, 0x00482aea, 0x0048412a)), &oCSpell_InitValues);

    void __fastcall oCSpell_DoTimedEffect(::Union::Registers& reg)
    {
        auto spell = reinterpret_cast<oCSpell*>(reg.esi);

        auto spellData = sdManager->GetSpellData(spell->spellID);
        if(!spellData)
        {
            return;
        }

        if(spellData->GetType() == oCSpell_Data::oCSpell_Type::SPELL_TYPE_CONTROL)
        {
            spell->CheckControl_Union();
            reg.eip = zSwitch(0x0047fb23, 0x0048a726 ,0x00485ce4, 0x00487304);
        }
    }
    auto PartialHook__oCSpell_DoTimedEffect = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x0047f948, 0x0048a50b, 0x00485ca3, 0x004872c3)), &oCSpell_DoTimedEffect);

    void __fastcall oCSpell_IsValidTarget(::Union::Registers& reg)
    {
    #if ENGINE == Engine_G1
        auto spell = reinterpret_cast<oCSpell*>(reg.ebp);
        auto vob = reinterpret_cast<zCVob*>(reg.esi);
        auto npc = reinterpret_cast<oCNpc*>(reg.edi);
    #elif ENGINE == Engine_G1A
        auto spell = reinterpret_cast<oCSpell*>(reg.edi);
        auto vob = reinterpret_cast<zCVob*>(reg.esi);
        auto npc = *reinterpret_cast<oCNpc**>(reg.edi + 0x34);
    #else
        auto spell = reinterpret_cast<oCSpell*>(reg.ebp);
        auto vob = reinterpret_cast<zCVob*>(reg.edi);
        auto npc = reinterpret_cast<oCNpc*>(reg.esi);
    #endif

        auto spellData = sdManager->GetSpellData(spell->spellID);
        if(!spellData)
        {
            return;
        }

        if(spellData->GetType() != oCSpell_Data::oCSpell_Type::SPELL_TYPE_TELEKINESIS)
        {
            return;
        }
 
        auto item = zDYNAMIC_CAST<oCItem>(vob);
        if(!item/* || spell->spellCasterNpc->GetDistanceToVob(*vob) <= 250.0f*/)
        {
            spell->spellStatus = SPL_STATUS_DONTINVEST;
            reg.eip = zSwitch(0x0047e151, 0x00488a77, 0x00484e37, 0x00486457);
        }
    }
    auto PartialHook__oCSpell_IsValidTarget = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x0047e0fe, 0x00488a21, 0x00484df7, 0x00486417)), &oCSpell_IsValidTarget);

    void oCSpell::StopTargetEffects_Union(zCVob* vob)
    {
        if(!vob)
        {
            return;
        }

        auto spellData = sdManager->GetSpellData(this->spellID);
        if(!spellData)
        {
            return;
        }

        if(spellData->GetType() != oCSpell_Data::oCSpell_Type::SPELL_TYPE_TELEKINESIS)
        {
            return;
        }

        this->StopTelekinesis_Union(vob);
    }

    void __fastcall oCSpell_StopTargetEffects_Inline_Setup(::Union::Registers& reg)
    {
    #if ENGINE <= Engine_G1A
        auto spell = reinterpret_cast<oCSpell*>(reg.esi);
    #else
        auto spell = reinterpret_cast<oCSpell*>(reg.ebp);
    #endif

        spell->StopTargetEffects_Union(spell->spellTarget);
    }
    auto PartialHook__oCSpell_StopTargetEffects_Inline_Setup = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x0047c6f1, 0x00486e8d, 0x00483417, 0x00484a77)), &oCSpell_StopTargetEffects_Inline_Setup);

    void __fastcall oCSpell_StopTargetEffects_Inline_Cast(::Union::Registers& reg)
    {
    #if ENGINE == Engine_G1A
        auto spell = reinterpret_cast<oCSpell*>(reg.ebp);
    #else
        auto spell = reinterpret_cast<oCSpell*>(reg.esi);
    #endif

        spell->StopTargetEffects_Union(spell->spellTarget);
    }
    auto PartialHook__oCSpell_StopTargetEffects_Inline_Cast = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x0047d096, 0x0048788c, 0x00483e21, 0x00485481)), &oCSpell_StopTargetEffects_Inline_Cast);

    void __fastcall oCSpell_StopTargetEffects_Inline_Stop(::Union::Registers& reg)
    {
        auto spell = reinterpret_cast<oCSpell*>(reg.ebp);

        spell->StopTargetEffects_Union(spell->spellTarget);
    }
    auto PartialHook__oCSpell_StopTargetEffects_Inline_Stop = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x0047d23b, 0x00487a4e, 0x00484050, 0x004856b1)), &oCSpell_StopTargetEffects_Inline_Stop);

    void __fastcall oCSpell_StopTargetEffects_Inline_Kill(::Union::Registers& reg)
    {
    #if ENGINE == Engine_G2A
        auto spell = reinterpret_cast<oCSpell*>(reg.esi);
    #elif ENGINE <= Engine_G2
        auto spell = reinterpret_cast<oCSpell*>(reg.ebp);
    #endif 

        spell->StopTargetEffects_Union(spell->spellTarget);
    }
    auto PartialHook__oCSpell_StopTargetEffects_Inline_Kill = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x0047d3a7, 0x00487bca, 0x00484235, 0x00485841)), &oCSpell_StopTargetEffects_Inline_Kill);

    void oCSpell::DoLogicInvestEffect_Union()
    {
        auto spellData = sdManager->GetSpellData(this->spellID);
        if(!spellData)
        {
            return;
        }

        if(spellData->GetType() != oCSpell_Data::oCSpell_Type::SPELL_TYPE_TELEKINESIS)
        {
            return;
        }

        this->InvestTelekinesis_Union();
    }

    void __fastcall oCSpell_DoLogicInvestEffect_Inline_Invest(::Union::Registers& reg)
    {
        auto spell = reinterpret_cast<oCSpell*>(reg.ebp);

        int manaLeft = 0;
        if(spell->spellCasterNpc)
        {
            manaLeft = spell->spellCasterNpc->GetAttribute(spell->spellEnergy);
        }

        if(manaLeft > 0)
        {
            spell->DoLogicInvestEffect_Union();
        }
    }
    auto PartialHook__oCSpell_DoLogicInvestEffect_Inline_Invest = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x0047ce5e, 0x0048764f, 0x00483b95, 0x004851ec)), &oCSpell_DoLogicInvestEffect_Inline_Invest);

    void __fastcall oCSpell_Setup_SetPrio(::Union::Registers& reg)
    {
        if(!oCNpcFocus::focuslist[FOCUS_MAGIC])
        {
            return;
        }

    #if ENGINE <= Engine_G1A
        auto spell = reinterpret_cast<oCSpell*>(reg.esi);
    #else
        auto spell = reinterpret_cast<oCSpell*>(reg.ebp);
    #endif

        static int npcPrioBackup = -666;
        static int itemPrioBackup = -666;
        static int mobPrioBackup = -666;
        if(npcPrioBackup == -666 || itemPrioBackup == -666 || mobPrioBackup == -666)
        {
            npcPrioBackup = oCNpcFocus::focuslist[FOCUS_MAGIC]->n_prio;
            itemPrioBackup = oCNpcFocus::focuslist[FOCUS_MAGIC]->i_prio ;
            mobPrioBackup = oCNpcFocus::focuslist[FOCUS_MAGIC]->m_prio;
        }

        auto spellData = sdManager->GetSpellData(spell->spellID);

        if(spellData && spellData->GetType() == oCSpell_Data::oCSpell_Type::SPELL_TYPE_TELEKINESIS)
        {
            oCNpcFocus::focuslist[FOCUS_MAGIC]->n_prio = -1;
            oCNpcFocus::focuslist[FOCUS_MAGIC]->i_prio = 1;
            oCNpcFocus::focuslist[FOCUS_MAGIC]->m_prio = -1;
        }
        else if(spellData && spellData->GetType() == oCSpell_Data::oCSpell_Type::SPELL_TYPE_CONTROL)
        {
            oCNpcFocus::focuslist[FOCUS_MAGIC]->n_prio = 1;
            oCNpcFocus::focuslist[FOCUS_MAGIC]->i_prio = -1;
            oCNpcFocus::focuslist[FOCUS_MAGIC]->m_prio = -1;
        }
        else
        {
            oCNpcFocus::focuslist[FOCUS_MAGIC]->n_prio = npcPrioBackup;
            oCNpcFocus::focuslist[FOCUS_MAGIC]->i_prio = itemPrioBackup;
            oCNpcFocus::focuslist[FOCUS_MAGIC]->m_prio = mobPrioBackup;
        }

        CurrentTelStep = 0;
    }
    auto PartialHook__oCSpell_Setup_SetPrio = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x0047c66e, 0x00486e0b, 0x0048338e, 0x004849ee)), &oCSpell_Setup_SetPrio);
}