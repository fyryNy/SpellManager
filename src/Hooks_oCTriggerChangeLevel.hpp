namespace GOTHIC_NAMESPACE 
{
    void __fastcall oCTriggerChangeLevel_TriggerTarget(::Union::Registers& reg)
    {
        auto spellNode = player->activeSpells.GetNextInList();
        while (spellNode)
        {
            auto spell = spellNode->GetData();
            spellNode = spellNode->GetNextInList();
            if(!spell)
            {
                continue;
            }

            auto spellData = sdManager->GetSpellData(spell->GetSpellID());
            if(!spellData)
            {
                continue;
            }

            if(spellData->GetType() != oCSpell_Data::oCSpell_Type::SPELL_TYPE_TRANSFORM)
            {
                continue;
            }

            spell->EndTimedEffect();
            reg.eip = zSwitch(0x0043867d, 0x0043d05f, 0x0043bae6, 0x0043c116);
            break;
        }
    }
	auto PartialHook__oCTriggerChangeLevel_TriggerTarget = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x004383b8, 0x0043cd58, 0x0043b834, 0x0043be64)), &oCTriggerChangeLevel_TriggerTarget);
}