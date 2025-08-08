namespace GOTHIC_NAMESPACE 
{
    void __fastcall oCGame_ChangeLevel(::Union::Registers& reg)
    {
        sdManager->RestoreParserInstances();
    }
	auto PartialHook__oCGame_ChangeLevel = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x0063d420, 0x0066408f, 0x0066ad20, 0x006c7a8f)), &oCGame_ChangeLevel);
}