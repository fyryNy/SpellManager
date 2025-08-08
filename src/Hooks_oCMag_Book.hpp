namespace GOTHIC_NAMESPACE 
{
    void __fastcall oCMag_Book_Spell_Stop(::Union::Registers& reg)
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

        if(!magBook->spellitems[spellNr]->MultiSlot() || CurrentTelStep < 4)
        {
            return;
        }

        if(--magBook->spellitems[spellNr]->amount > 0)
        {
            return;
        }

        auto item = ownerNpc->RemoveFromInv(magBook->spellitems[spellNr], 0);
        magBook->wld->RemoveVob(item);
        if(magBook->spells.GetNum() == 0)
        {
            auto msg = zNEW(oCMsgWeapon)(oCMsgWeapon::EV_REMOVEWEAPON, 0, 0);
            ownerNpc->GetEM()->OnMessage(msg, ownerNpc);
        }
        else
        {
            magBook->spellnr = 0;
            magBook->Spell_Setup(magBook->spellnr, ownerNpc, NULL);
            auto spell = magBook->GetSelectedSpell();
            if(spell)
            {
                ogame->GetTextView()->Printwin(spell->GetName());
            }
        }
    }
    auto PartialHook__oCMag_Book_Spell_Stop = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x004712ec, 0x0047a278, 0x00476c0a, 0x00477faa)), &oCMag_Book_Spell_Stop);
}