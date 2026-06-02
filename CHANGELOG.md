[1.0.5]
- Added missing addresses for Gothic Sequel and Gothic 2 Classic in `oCVisualFX::InitEffect` hook
- Fixed wrong register for Gothic 1 in `oCSpell::EndTimedEffect` hook

[1.0.4.1]
- Small fix for a while loop in `oCAIHuman::CheckActiveSpells`

[1.0.4]
- Moved `oCSpell:DeleteCaster` hook to the beginning of the func

[1.0.3]
- Fixed wrong register usage in `oCSpawnManager_CheckRemoveNpc`

[1.0.2]
- Fixed some addresses
- Added new field to `C_SPELL_DATA` class: `spellIsInvestSpell`
  - For spells like telekinesis, spell scroll will be removed from inventory after investing mana

[1.0.1]
- A bit of cleaning in the code
- Fix for removing telekinesis scroll

[1.0.0]
- Initial release