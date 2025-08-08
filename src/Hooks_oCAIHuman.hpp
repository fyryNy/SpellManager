namespace GOTHIC_NAMESPACE 
{
    void __fastcall oCAIHuman_CheckActiveSpells(::Union::Registers& reg)
    {
        auto npc = *reinterpret_cast<oCNpc**>(reg.edi + 0x12c);
        
        if(!npc)
        {
            return;
        }

        auto spellNode = npc->activeSpells.GetNextInList();
        while (spellNode)
        {
            auto spell = spellNode->GetData();
            spellNode = spellNode->GetNextInList();

            if(!spell)
            {
                continue;
            }

            auto spellData = sdManager->GetSpellData(spell->spellID);
            if(!spellData)
            {
                return;
            }

            if(spellData->GetType() == oCSpell_Data::oCSpell_Type::SPELL_TYPE_CONTROL
            || spellData->GetType() == oCSpell_Data::oCSpell_Type::SPELL_TYPE_TRANSFORM)
            {
                spell->EndTimedEffect();
                reg.eip = zSwitch(0x0046c2e0, 0x004746b0, 0x00471002, 0x00472152);
                return;
            }
        }
    }
    auto PartialHook__oCAIHuman_CheckActiveSpells = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x0046c2d0, 0x00474680, 0x00470fd8, 0x00472128)), &oCAIHuman_CheckActiveSpells);
}