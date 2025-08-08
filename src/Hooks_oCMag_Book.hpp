namespace GOTHIC_NAMESPACE 
{
    void __fastcall oCMag_Book_Spell_Stop(::Union::Registers& reg)
    {
        auto _this = reinterpret_cast<oCMag_Book*>(reg.esi);

        auto ownerNpc = static_cast<oCNpc*>(_this->owner);
        auto spellNr = _this->spellnr;

        auto spellData = sdManager->GetSpellData(_this->spells[spellNr]->spellID);
        if(!spellData)
        {
            return;
        }

        if(spellData->GetType() != oCSpell_Data::oCSpell_Type::SPELL_TYPE_TELEKINESIS)
        {
            return;
        }

        if(!_this->spellitems[spellNr]->MultiSlot() || CurrentTelStep < 4)
        {
            return;
        }

        if(--_this->spellitems[spellNr]->amount > 0)
        {
            return;
        }

        auto item = ownerNpc->RemoveFromInv(_this->spellitems[spellNr], 0);
        _this->wld->RemoveVob(item);
        if(_this->spells.GetNum() == 0)
        {
            auto msg = zNEW(oCMsgWeapon)(oCMsgWeapon::EV_REMOVEWEAPON, 0, 0);
            ownerNpc->GetEM()->OnMessage(msg, ownerNpc);
        }
        else
        {
            _this->spellnr = 0;
            _this->Spell_Setup(_this->spellnr, ownerNpc, NULL);
            auto spell = _this->GetSelectedSpell();
            if(spell)
            {
                ogame->GetTextView()->Printwin(spell->GetName());
            }
        }
    }
    auto PartialHook__oCMag_Book_Spell_Stop = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x004712ec, 0x0047a278, 0x00476c0a, 0x00477faa)), &oCMag_Book_Spell_Stop);
}