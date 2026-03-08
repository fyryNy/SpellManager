namespace GOTHIC_NAMESPACE 
{
    void __fastcall oCSpawnManager_CheckRemoveNpc(::Union::Registers& reg)
    {
        auto npc = reinterpret_cast<oCNpc*>(reg.ecx);

        if (!npc)
        {
            return;
        }

        if (npc->HasBodyStateModifier(BS_MOD_CONTROLLING))
        {
            reg.eip = zSwitch(0x006d0dd0, 0x0070793c, 0x00719c40, 0x00779679);
        }
    }
    auto PartialHook__oCSpawnManager_CheckRemoveNpc = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x006d0aa3, 0x007075eb, 0x00719913, 0x0077930b)), &oCSpawnManager_CheckRemoveNpc);
}