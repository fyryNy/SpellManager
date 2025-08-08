namespace GOTHIC_NAMESPACE 
{
    void __fastcall oCSpawnManager_CheckRemoveNpc(::Union::Registers& reg)
    {
    #if ENGINE == Engine_G1A
        auto npc = reinterpret_cast<oCNpc*>(reg.edi);   
    #else
        auto npc = reinterpret_cast<oCNpc*>(reg.esi);
    #endif

        if(npc->HasBodyStateModifier(BS_MOD_CONTROLLING))
        {
            reg.eip = zSwitch(0x006d0dd0, 0x0070793c, 0x00719c40, 0x00779679);
        }
    }
    auto PartialHook__oCSpawnManager_CheckRemoveNpc = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x006d0a9e, 0x007075e6, 0x0071990e, 0x00779306)), &oCSpawnManager_CheckRemoveNpc);
}