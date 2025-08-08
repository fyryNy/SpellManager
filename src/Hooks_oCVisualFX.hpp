namespace GOTHIC_NAMESPACE 
{
    void __fastcall oCVisualFX_SetCollisionEnabled(::Union::Registers& reg)
    {
        auto vfx = reinterpret_cast<oCVisualFX*>(reg.esi);
        if(!vfx)
        {
            return;
        }

        auto spellId = vfx->GetSpellType();

        auto spellData = sdManager->GetSpellData(spellId);
        if(!spellData)
        {
            return;
        }

        if(spellData->GetType() == oCSpell_Data::oCSpell_Type::SPELL_TYPE_PROJECTILE)
        {
            vfx->SetIsProjectile(TRUE);
            vfx->SetCollisionClass(zCCollObjectProjectile::S_GetCollObjClass());
        }
        else if(spellData->GetType() == oCSpell_Data::oCSpell_Type::SPELL_TYPE_SPREAD)
        {
            if(vfx->visName_S.Search("_SPREAD", 1U) != -1)
            {
                vfx->SetCollisionClass(zCCollObjectBoxPassThrough::S_GetCollObjClass());
            }
            else
            {
                vfx->SetIsProjectile(TRUE);
                vfx->SetCollisionClass(zCCollObjectProjectile::S_GetCollObjClass());
            }
        }
        else
        {
            vfx->SetCollisionClass(zCCollObjectBoxPassThrough::S_GetCollObjClass());
        }

        reg.eip = zSwitch(0x00485741, 0x004908cc, 0x0048bac3, 0x0048d433);
    }
	auto PartialHook__oCVisualFX_SetCollisionEnabled = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x004856d4, 0x0049087e, 0x0048ba54, 0x0048d3c4)), &oCVisualFX_SetCollisionEnabled);
}