namespace GOTHIC_NAMESPACE 
{
	void Game_Init()
	{
		sdManager = std::make_unique<oCSpell_DataManager>();
	}

	void Game_Exit()
	{
		sdManager.reset();
	}

	void __fastcall Game_Init_Hook()
	{
		Game_Init();
	}
	auto PartialHook__Game_Init = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x00637BB6, 0x0065DFFA, 0x00665112, 0x006C1BFF)), &Game_Init_Hook);
#if ENGINE >= Engine_G2
	auto PartialHook__Game_Init2 = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x00000000, 0x00000000, 0x006650F6, 0x006C1C1B)), &Game_Init_Hook);
#endif

	void __fastcall Game_Exit_Hook()
	{
		Game_Exit();
	}
	auto PartialHook__Game_Exit = ::Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x00424850 + 7, 0x00427310 + 7, 0x004251A0 + 7, 0x004254E0 + 7)), &Game_Exit_Hook);
}