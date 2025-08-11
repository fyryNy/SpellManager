namespace GOTHIC_NAMESPACE 
{
    void __fastcall oCAIHuman_CheckActiveSpells(::Union::Registers& reg)
    {
        auto npc = *reinterpret_cast<oCNpc**>(reg.edi + 0x12c);

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

    void __fastcall oCAIHuman_MagicCheckSpellStates_RemoveScroll(::Union::Registers& reg)
    {
        auto magBook = reinterpret_cast<oCMag_Book*>(reg.esi);

        auto ownerNpc = static_cast<oCNpc*>(magBook->owner);
        auto spellNr = magBook->spellnr;

        auto spellData = sdManager->GetSpellData(magBook->spells[spellNr]->spellID);
        if(!spellData)
        {
            return;
        }

        if(spellData->GetType() != oCSpell_Data::oCSpell_Type::SPELL_TYPE_TELEKINESIS)
        {
            return;
        }

        auto spellItem = magBook->spellitems[spellNr];

        if(!spellItem->MultiSlot() || CurrentTelStep < 4)
        {
            return;
        }

        if(--spellItem->amount > 0)
        {
            return;
        }

        auto item = ownerNpc->RemoveFromInv(magBook->spellitems[spellNr], 0);
        magBook->wld->RemoveVob(item);
        
        if(magBook->spells.GetNum() > 0)
        {
            magBook->spellnr = 0;
            magBook->Spell_Setup(magBook->spellnr, ownerNpc, nullptr);
            auto spell = magBook->GetSelectedSpell();
            if(spell)
            {
                ogame->GetTextView()->Printwin(spell->GetName());
            }
        }
        else
        {
            auto msg = zNEW(oCMsgWeapon)(oCMsgWeapon::EV_REMOVEWEAPON, 0, 0);
            ownerNpc->GetEM()->OnMessage(msg, ownerNpc);
        }
    }
    auto PartialHook__oCAIHuman_MagicCheckSpellStates_RemoveScroll = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x0046ced6, 0x0047533c, 0x00471bcb, 0x00472d0d)), &oCAIHuman_MagicCheckSpellStates_RemoveScroll);
}