# SpellManager

SpellManager is a Union plugin for Gothic 1, Gothic Sequel/Gothic 1 Addon,
Gothic 2 Classic, and Gothic 2 Night of the Raven. It lets scripts register
custom spell ids as real engine spell types instead of being limited to the
small set of ids that the original engine checks directly in C++.

The plugin is intentionally data-driven. You add normal Daedalus spell scripts,
then add one `C_SPELL_DATA` instance for every spell that needs engine support.
`C_SPELL_DATA` is the only script-facing API; SpellManager does not add new
Daedalus externals. Spell ids without `C_SPELL_DATA` keep their original spell
handling. The safeguard against removing the original controlling body also
applies to vanilla G1 Control without script changes.

## What It Fixes

Vanilla Gothic scripts can define additional `SPL_*` constants, spell items,
spell FX, and `Spell_Logic_*` functions, but several important parts of the
engine still use hardcoded spell id checks:

- projectile and spread collision classes in `oCVisualFX`;
- transform spell body swap and cleanup ranges;
- Gothic 1 telekinesis item handling;
- Gothic 1 control spell handling;
- active transform cleanup on Enter and level change;
- scroll removal for spells that spend invested mana before the final cast.

SpellManager extends those behaviors using metadata read from Daedalus. It
also implements Control and Telekinesis for G2/G2A, where those spells are
missing, and protects the original controlling body from spawn-manager removal
in all supported engines.

Example metadata:

```d
INSTANCE MySpell_Data(C_SPELL_DATA)
{
    spellId = SPL_MySpell;
    spellType = SPELL_TYPE_PROJECTILE;
    spellEnergyType = ATR_MANA;
    spellIsInvestSpell = FALSE;
};
```

## Supported Engines

The source has hookspaces for all Union Gothic API targets:

- `G1`: Gothic 1
- `G1A`: Gothic Sequel / Gothic 1 Addon target
- `G2`: Gothic 2 Classic
- `G2A`: Gothic 2 Night of the Raven

Use a per-game build when you only target one executable, or the `MP-*` presets
when you want one plugin build containing all supported hookspaces.

## Installation

1. Install Union in the target game so it can load Union plugins.
2. Build or obtain `SpellManager.dll` for that game (or use an `MP-*` build).
3. Put the DLL in the game's `System/Autorun` directory, or package it in a VDF
   that installs to `System/Autorun`.
4. Add the script-side `C_SPELL_DATA` class and constants shown below, then
   follow [Adding A Custom Spell](#adding-a-custom-spell) for each new spell.
5. For `SPELL_TYPE_TELEKINESIS` or `SPELL_TYPE_CONTROL`, add any missing
   animation declarations and ASC files from `ASSETS` as described under
   [Assets](#assets). G1 already has the original spell animations.
6. Recompile the content scripts and any changed VFX/PFX/SFX scripts and
   animations. Start the game with your mod's compiled scripts and assets.

For the safeguard against removing vanilla G1's controlling body, installing
the DLL is enough. No spell metadata, FX edits, or animation imports are needed
for that fix. See [G1 Control Safeguards](#g1-control-safeguards).

## Building

Build on Windows with CMake 3.21 or newer, the x86 MSVC toolchain, and Ninja
available. Run these commands from an x86 Native Tools command prompt; the
presets expect the compiler environment to be set up externally.

```bat
git submodule update --init --recursive
cmake --preset MP-Release
cmake --build --preset MP-Release
```

The resulting DLL is `out/build/MP-Release/SpellManager.dll` (replace
`MP-Release` with the chosen preset).

Useful presets:

```text
G1-Release
G1A-Release
G2-Release
G2A-Release
MP-Release
```

The project also has matching `*-Debug` presets.

## Script API

Add this class to your scripts before any `C_SPELL_DATA` instances are parsed.
In a vanilla script tree, `Content/_intern/Classes.d` is a good place.

```d
CLASS C_SPELL_DATA
{
    var int spellId;
    var int spellType;
    var int spellEnergyType;
    var int spellIsInvestSpell;
};

const int SPELL_TYPE_DEFAULT     = 0;
const int SPELL_TYPE_PROJECTILE  = 1;
const int SPELL_TYPE_TRANSFORM   = 2;
const int SPELL_TYPE_SPREAD      = 3;
const int SPELL_TYPE_TELEKINESIS = 4;
const int SPELL_TYPE_CONTROL     = 5;
```

The field order and size must stay exactly as shown. The plugin validates the
class size when it scans script instances.

### Fields

| Field | Meaning |
| --- | --- |
| `spellId` | The numeric `SPL_*` id of the spell. It must match the id used by your rune or scroll item. |
| `spellType` | Engine behavior category. See the spell type table below. |
| `spellEnergyType` | NPC attribute used by the engine for spell energy: `ATR_MANA` (`2`) for mana or `ATR_HITPOINTS` (`0`) for HP. |
| `spellIsInvestSpell` | Set to `TRUE` for sustained spells such as telekinesis or Gothic 1-style pyrokinesis, whose effect runs while the action button is held and ends on release. This enables scroll consumption on the invest/stop path. |

`spellEnergyType` does not rewrite your scripts. Any resource checks or manual
deductions in `Spell_Logic_*` and `Spell_Cast_*` must use the same attribute.

The two classes have different `spellType` fields: `C_Spell_Proto.spellType`
uses `SPELL_BAD`, `SPELL_GOOD`, or `SPELL_NEUTRAL`, while
`C_SPELL_DATA.spellType` uses the `SPELL_TYPE_*` constants above.

### Spell Types

| Type | Engine behavior |
| --- | --- |
| `SPELL_TYPE_DEFAULT` | Uses pass-through box collision, allowing FX to pass through walls. Suitable for waves, rain effects, or spells applied directly to a target, such as sleep or oblivion. The plugin still applies the energy and invested-scroll settings. |
| `SPELL_TYPE_PROJECTILE` | Marks the visual FX as a projectile and uses projectile collision. Use this for bolt, ball, lance, and similar single-projectile spells. |
| `SPELL_TYPE_TRANSFORM` | Enables transform behavior for this id, without needing the id to be inside the vanilla transform range. The spell logic must call `Npc_SetActiveSpellInfo` with the NPC instance to transform into. |
| `SPELL_TYPE_SPREAD` | Projectile behavior for normal FX visuals, but pass-through box collision for visuals whose `visName_S` contains `_SPREAD`. Use this for spells with a projectile origin plus a spreading child effect. |
| `SPELL_TYPE_TELEKINESIS` | Enables item-only target validation, item focus priority, telekinesis movement, physics cleanup, and auto-pickup when the item reaches the caster. Requires the telekinesis animation assets. |
| `SPELL_TYPE_CONTROL` | Enables control-spell body switching, controlled NPC tracking, range checks, cleanup on death/damage, and NPC focus priority. Requires control scripts, VFX, and animation assets. |

## Adding A Custom Spell

This is the normal script-side checklist for a new spell id. The paths below
refer to a Gothic 2 Night of the Raven script tree:

1. Add a unique `SPL_*` constant in `Content/_intern/Constants.d` and increase
   the existing `MAX_SPELL` so it is greater than the highest spell id.
2. Add entries at that id's index in all three arrays:

   - `spellFxInstanceNames` in `Constants.d`: the spell name suffix, such as
     `"MyBolt"`;
   - `spellFxAniLetters` in `Constants.d`: an animation code, such as `"FBT"`,
     matching animation declarations available on the caster model;
   - `TXT_SPELLS` in `Content/Story/Text.d`: the displayed spell name.

   Keep existing entries and any reserved slots in place.
3. Create `INSTANCE Spell_<Name>(C_Spell_Proto)`,
   `func int Spell_Logic_<Name>(var int manaInvested)`, and
   `func void Spell_Cast_<Name>()` in `Content/AI/Magic/Spells/Spell_<Name>.d`.
   The cast function can be empty if the spell needs no script work on cast.
4. Beside the spell instance, create `INSTANCE Spell_<Name>_Data(C_SPELL_DATA)`
   with the matching id and the appropriate type, energy, and invest settings.
5. Add a branch in `Content/AI/Magic/Spell_ProcessMana.d` that returns your
   `Spell_Logic_<Name>(manaInvested)` result. For spells that need special
   behavior on release, also update `Spell_ProcessMana_Release.d`.
6. Review `Content/AI/Magic/C_CanNpcCollideWithSpell.d` for damaging spells or
   victim effects. Add your id where you need specific damage, immunity, or
   victim-state rules. If the fallback already returns `COLL_DOEVERYTHING`
   for unlisted ids, an explicit branch is needed only to change that behavior.
   Add any scripted victim reactions to `B_AssessMagic` and the relevant AI states.
7. Create the matching `SPELLFX_<Name>` VFX and its required PFX, SFX, and assets.
   Follow an existing spell's FX keys and collision settings; the plugin only
   supplies the engine behavior selected by `C_SPELL_DATA`.
8. Create a rune or scroll item with `spell = SPL_<Name>`.
9. Ensure the files are included in the relevant `.src` lists. In
   `Content/Gothic.src`, constants and classes must precede instances,
   `C_Spell_Proto.d` must precede spell instances, and spell logic must precede
   `Spell_ProcessMana.d`. The usual `AI\MAGIC\Spells\Spell_*.d` entry includes
   new spell files automatically. Include new effect files through
   `System/VisualFX.src`, `System/ParticleFX.src`, and `System/SFX.src` as needed.
10. Recompile the affected scripts and assets. Add teaching, NPC AI, and
    crafting entries as needed by your mod.

The important convention is that `spellFxInstanceNames[spellId]` maps an id to
the script names used by the engine:

```d
// spellFxInstanceNames[SPL_MyBolt] = "MyBolt"
INSTANCE Spell_MyBolt(C_Spell_Proto) { ... };
INSTANCE SPELLFX_MyBolt(CFx_Base_Proto) { ... };
```

For a new spell, use the same `<Name>` suffix for `Spell_Logic_<Name>` and
`Spell_Cast_<Name>`. `Spell_ProcessMana` calls the logic function explicitly,
so that function can have a different name when sharing an existing spell
instance, as in the transform example below.

The metadata instance name is a convention: the plugin discovers every
`C_SPELL_DATA` instance and matches it by `spellId`. Define only one metadata
instance for each spell id.

Treat `C_SPELL_DATA` as static configuration. The plugin reads these instances
when the game starts and restores its parser instance pointers on level change.

If you add `C_SPELL_DATA` for an existing vanilla spell, choose the type that
matches the behavior you still want. For example, setting an old projectile to
`SPELL_TYPE_DEFAULT` will override the vanilla hardcoded projectile collision
with pass-through box collision.

## Minimal Projectile Example

These are G2A integration excerpts. They assume id `103` is unused and reuse
the firebolt animation and rune mesh. Merge the array entries into your existing
arrays at index `103`; the omitted entries `0` through `102` must remain in
place. Update the existing `MAX_SPELL` declaration instead of adding a second one.

```d
// Content/_intern/Constants.d
const int SPL_MyBolt = 103;
const int MAX_SPELL  = 104;

const string spellFxInstanceNames[MAX_SPELL] =
{
    // Keep existing entries 0..102 here, followed by a comma.
    "MyBolt" // 103 SPL_MyBolt
};

const string spellFxAniLetters[MAX_SPELL] =
{
    // Keep existing entries 0..102 here, followed by a comma.
    "FBT" // 103 SPL_MyBolt
};
```

```d
// Content/Story/Text.d
const string TXT_SPELLS[MAX_SPELL] =
{
    // Keep existing entries 0..102 here, followed by a comma.
    "My Bolt" // 103 SPL_MyBolt
};
```

```d
// Content/AI/Magic/Spells/Spell_MyBolt.d
INSTANCE Spell_MyBolt(C_Spell_Proto)
{
    time_per_mana = 0;
    damage_per_level = 25;
    damageType = DAM_MAGIC;
    spelltype = SPELL_BAD;
    targetCollectAlgo = TARGET_COLLECT_FOCUS;
    targetCollectType = TARGET_TYPE_NPCS;
};

INSTANCE Spell_MyBolt_Data(C_SPELL_DATA)
{
    spellId = SPL_MyBolt;
    spellType = SPELL_TYPE_PROJECTILE;
    spellEnergyType = ATR_MANA;
    spellIsInvestSpell = FALSE;
};

func int Spell_Logic_MyBolt(var int manaInvested)
{
    if (self.attribute[ATR_MANA] >= 5)
    {
        return SPL_SENDCAST;
    };

    return SPL_SENDSTOP;
};

func void Spell_Cast_MyBolt()
{
    self.attribute[ATR_MANA] = self.attribute[ATR_MANA] - 5;
};
```

This simple example charges 5 mana per cast. If you also make a scroll, add the
usual `Npc_GetActiveSpellIsScroll(self)` checks when it should have a different
cost.

```d
// Inside Spell_ProcessMana, after activeSpell has been set:
if (activeSpell == SPL_MyBolt)
{
    return Spell_Logic_MyBolt(manaInvested);
};
```

For a working FX setup, copy the `SPELLFX_Firebolt` definitions from
`System/VisualFX/VisualFxInst.d` under the `SPELLFX_MyBolt` prefix, including
their `_KEY_OPEN`, `_KEY_INIT`, `_KEY_CAST`, `_KEY_COLLIDE`, `_COLLIDEFX`, and
`_COLLIDEDYNFX` instances. Update references to the renamed FX instances. You
can reuse the original firebolt PFX and sounds, or supply your own. Preserve
the cast key's `emCheckCollision = 1` and the appropriate static/dynamic
collision actions.

```d
// Content/Items/IT_MyBolt.d
INSTANCE ItRu_MyBolt(C_Item)
{
    name = "My Bolt";
    mainflag = ITEM_KAT_RUNE;
    flags = 0;
    value = 500;
    visual = "ItRu_FireBolt.3DS";
    material = MAT_STONE;
    spell = SPL_MyBolt;
    mag_circle = 1;
    description = name;
};
```

## Control and Telekinesis Assets in G1 and G2A

Both spells exist in G1. G2/G2A have no working vanilla Control or Telekinesis
spell. The comparison below uses the original G1 and G2A scripts; check the
contents of your G2 Classic script set before copying missing definitions.

| Script component | Original G1 | Original G2A |
| --- | --- | --- |
| Spell instances, logic, ids, items, and dispatch | Present for both spells. | Missing for both spells; add them using the setup below. |
| `spellFX_Control` and `spellFX_Telekinesis` VFX families | Present, with some optional instances commented out. | Missing; copy the required G1 definitions. |
| `CONTROL_CASTBLEND`, `CONTROL_LEAVERANGEFX` | Present. | Still present; reuse the existing definitions. |
| `MFX_CONTROL_*` and `MFX_TELEKINESIS_*` particle definitions | Present. | Still present in `System/PFX/PfxInstMagic.d`. |
| Control and Telekinesis sounds | Present. | Still present in `System/SFX/SfxInst.d`. |
| Control preparation and recovery AI states | Present under `Content/Magic/ZS`. | Missing; port or implement the states used by your Control logic. |

The particle and sound definitions used by these two spells are unchanged
between the original G1, original G2A, and tested G2A scripts. The tested setup
adds the missing VFX definitions and spell scripts while reusing those PFX/SFX.
The specific FX changes are listed under each example below.

## Telekinesis Example

For G2/G2A, `SPELL_TYPE_TELEKINESIS` supplies the behavior through this plugin.
G2A retains the particles and sounds, but needs the missing VFX definitions
described below.

Complete the general setup above: map
`SPL_TELEKINESIS_NEW` to `"Telekinesis"` in `spellFxInstanceNames`, use `"TEL"`
in `spellFxAniLetters`, add the display name and mana-dispatch branch, and
provide the spell's VFX and item.

```d
INSTANCE Spell_Telekinesis(C_Spell_Proto)
{
    time_per_mana = 1500;
    spelltype = SPELL_NEUTRAL;
    canTurnDuringInvest = FALSE;
    canChangeTargetDuringInvest = FALSE;
    targetCollectAlgo = TARGET_COLLECT_FOCUS;
    targetCollectType = TARGET_TYPE_ITEMS;
    targetCollectAzi = 20;
};

INSTANCE Spell_Telekinesis_Data(C_SPELL_DATA)
{
    spellId = SPL_TELEKINESIS_NEW;
    spellType = SPELL_TYPE_TELEKINESIS;
    spellEnergyType = ATR_MANA;
    spellIsInvestSpell = TRUE;
};

func int Spell_Logic_Telekinesis(var int manaInvested)
{
    if (Npc_GetActiveSpellLevel(self) <= 1)
    {
        return SPL_NEXTLEVEL;
    };

    return SPL_RECEIVEINVEST;
};

func void Spell_Cast_Telekinesis()
{
};
```

Runtime behavior:

- only item vobs are valid targets;
- magic focus prefers items and ignores NPCs/mobs while the spell is selected;
- the item rises to caster height while `S_TELSHOOT` is active;
- pressing forward pulls the item toward the caster;
- if the item comes close enough, the spell releases and the item is taken;
- `spellIsInvestSpell = TRUE` makes scroll stacks lose one scroll after mana is
  invested.

Animation files for the G2A setup:

- `ASSETS/_work/data/anims/TELEKINESIS_ADD_TO_HUMANS.mds`
- `ASSETS/_work/data/anims/asc_telekinesis/HUM_MAGTEL_M01.ASC`

The plugin checks for the animation state `S_TELSHOOT`, so keep that name unless
you also change the C++ code.

### Telekinesis FX for G2A

Copy these instances from G1's `System/VisualFX/VisualFxInst.d` into the G2A
VFX scripts, keeping their fields and references:

- `spellFX_Telekinesis`;
- `spellFX_Telekinesis_KEY_INIT`;
- `spellFX_Telekinesis_KEY_INVEST_1`;
- `spellFX_Telekinesis_KEY_CAST`;
- `spellFX_Telekinesis_ORIGIN`.

To reproduce the tested setup, also enable the stop key, which is commented
out in G1. It uses the existing `MFX_TELEKINESIS_TARGETEND` particles:

```d
INSTANCE spellFX_Telekinesis_KEY_STOP(C_ParticleFXEmitKey)
{
    visName_S = "MFX_Telekinesis_TargetEnd";
};
```

Keep `emFXInvestOrigin_S = "spellFX_Telekinesis_ORIGIN"` in the main effect.
Leave the stock commented `emFXInvestTarget_S` assignment disabled: there is
no `spellFX_Telekinesis_TARGET` VFX instance in this setup. The main effect's
`_KEY_INVEST_1` already uses the `MFX_TELEKINESIS_TARGET` particle definition.
The logic above returns `SPL_NEXTLEVEL` to reach that invest key.

Reuse these existing G2A definitions without adding duplicates:

- PFX: `MFX_TELEKINESIS_INIT`, `MFX_TELEKINESIS_TARGET`,
  `MFX_TELEKINESIS_TARGETEND`, `MFX_TELEKINESIS_BRIDGE`;
- SFX: `MFX_TELEKINESIS_STARTINVEST`, `MFX_TELEKINESIS_INVEST`.

Recompile `System/VisualFX.src` after adding the VFX. These changes require no
edits to the stock PFX/SFX definitions. Keep the referenced textures, sounds,
and other assets available in your mod installation.

## Control Example

Control is already present in G1. Adding it to G2/G2A requires the plugin's
engine behavior and the spell scripts, AI states, FX, and animations.
The following excerpt only defines the spell instance and metadata. Complete
the general setup with `"Control"` in `spellFxInstanceNames`, `"CON"` in
`spellFxAniLetters`, a display name, item, VFX, `Spell_Logic_Control`,
`Spell_Cast_Control`, and a branch in `Spell_ProcessMana`.

```d
INSTANCE Spell_Control(C_Spell_Proto)
{
    time_per_mana = 500;
    spelltype = SPELL_BAD;
    canTurnDuringInvest = FALSE;
    canChangeTargetDuringInvest = FALSE;
    targetCollectAlgo = TARGET_COLLECT_FOCUS;
    targetCollectRange = 1000;
    targetCollectType = TARGET_TYPE_HUMANS;
};

INSTANCE Spell_Control_Data(C_SPELL_DATA)
{
    spellId = SPL_CONTROL_NEW;
    spellType = SPELL_TYPE_CONTROL;
    spellEnergyType = ATR_MANA;
    spellIsInvestSpell = FALSE;
};
```

Runtime behavior:

- magic focus prefers NPCs and ignores items/mobs while the spell is selected;
- on cast, the target NPC becomes the player-controlled body;
- the original caster receives `BS_MOD_CONTROLLING`;
- the controlled NPC receives `BS_MOD_CONTROLLED`;
- control ends if the target dies or moves at least 4000 units away;
- `CONTROL_LEAVERANGEFX` plays when the target is at least 3500 units away;
- the plugin protects the original controlling body from being removed by the
  spawn manager;
- if the controlling body is damaged, control is ended.

Your scripts should provide at least:

- `ZS_CONTROLLED` and its loop/end functions, because the plugin starts this
  state when control ends;
- the preparation/defense states used by your logic; the tested setup uses
  `ZS_PsiDefense`, `B_StopPsiDefense`, and `ZS_PC_Controlling`;
- `Spell_Logic_Control` and `Spell_Cast_Control`, with the logic connected to
  `Spell_ProcessMana`;
- the Control VFX described below and the required animation entries.

G1 defines these states under `Content/Magic/ZS` and its logic in
`Content/Magic/Spell_Control.d`. Adapt them to your G2A helpers and state
names when porting them: the G1 versions reference `B_FullStop`,
`B_RegainDroppedWeapon`, `B_RegainDroppedArmor`, and `ZS_AssessEnemy`, which
are absent from the G2A scripts. The tested states use G2A helpers such as
`Npc_ClearAIQueue`, `AI_StandUpQuick`, and `AI_ContinueRoutine`.

The tested G2A logic starts `ZS_PsiDefense` on the target, returns
`SPL_NEXTLEVEL` at least once to activate the invest FX, then returns
`SPL_RECEIVEINVEST` while charging. On success it starts
`ZS_PC_Controlling` on the caster, calls `Npc_SetActiveSpellInfo(self, 1)`,
and returns `SPL_SENDCAST`.

This setup starts the target's defense state directly in the spell logic.
G2A's generic `B_AssessMagic` handling for `SPELL_BAD` already sends the fight
sound perception, so a new Control-specific branch there is not required for
this setup.

Animation files for the G2A setup:

- `ASSETS/_work/data/anims/CONTROL_ADD_TO_HUMANS.mds`
- `ASSETS/_work/data/anims/asc_control/*.ASC`

The provided MDS snippet defines `S_CONSHOOT`, `S_CONACTIVE`,
`S_CON_VICTIM`, `T_PSI_VICTIM`, and `T_PSI_VICTIM_2_STAND`.

### Control FX for G2A

Copy these instances from G1's `System/VisualFX/VisualFxInst.d` into the G2A
VFX scripts:

- `spellFX_Control`;
- `spellFX_Control_KEY_INVEST_1`;
- `spellFX_Control_KEY_CAST`;
- `spellFX_Control_TARGET`;
- `spellFX_Control_BRIDGE`;
- `spellFX_Control_BRIDGE_KEY_INIT`.

These six definitions match G1 in the tested G2A setup. Preserve the
main effect's `emFXInvestTarget_S` and `emFXInvestOrigin_S` references, and
`sendAssessMagic = 1` in `spellFX_Control_TARGET`.

There is also a dangling reference in both the original G1 block and the test
copy: `spellFX_Control_BRIDGE.emFXCreate_S` names `spellFX_Control_ORIGIN`,
but that VFX instance is commented out. When porting, either enable the
commented `spellFX_Control_ORIGIN` instance (its `MFX_CONTROL_ORIGIN` particle
definition already exists), or omit that optional origin effect by changing
the field inside `spellFX_Control_BRIDGE`:

```d
emFXCreate_S = "";
```

Resolving that reference is an additional cleanup to the copied block; the
test scripts leave it unchanged.

Reuse the existing G2A dependencies:

- VFX: `CONTROL_CASTBLEND` and `CONTROL_LEAVERANGEFX` in
  `System/VisualFX/VisualFxInst.d`;
- PFX: `MFX_CONTROL_INIT`, `MFX_CONTROL_TARGET`, `MFX_CONTROL_BRIDGE`,
  `MFX_CONTROL_ORIGIN` in `System/PFX/PfxInstMagic.d`;
- SFX: `MFX_CONTROL_STARTINVEST`, `MFX_CONTROL_INVEST`, `MFX_CONTROL_CAST` in
  `System/SFX/SfxInst.d`.

`_KEY_CAST` references `CONTROL_CASTBLEND`; the plugin looks up
`CONTROL_LEAVERANGEFX` by that exact name for the range warning. Recompile
`System/VisualFX.src` after the additions. The stock PFX/SFX definitions can
remain unchanged.

### G1 Control Safeguards

Installing the plugin protects NPCs marked `BS_MOD_CONTROLLING` from removal
by the spawn manager. This includes the original caster during vanilla G1
Control and requires no game-script changes. G1 already supplies the spell,
its logic, states, FX, and animations.

The replacement Control functions also check for missing caster/target
pointers before casting, ending control, or checking its range. Those
functions and the additional damage-triggered cleanup require a matching
`C_SPELL_DATA` instance with `spellType = SPELL_TYPE_CONTROL`. The automatic
spawn-manager safeguard does not require that metadata.

## Transform Example

With `SPELL_TYPE_TRANSFORM`, a transform spell no longer has to be inside the
vanilla `SPL_TRFSHEEP` through `SPL_TRFDRAGONSNAPPER` range.

This excerpt follows the tested G2A setup: the new id `SPL_TrfMeatbug` uses
`"Transform"` in `spellFxInstanceNames` and `"TRF"` in `spellFxAniLetters`,
reusing `Spell_Transform` and `SPELLFX_Transform`. Add its displayed name to
`TXT_SPELLS`, create the item, and dispatch `SPL_TrfMeatbug` to
`Spell_Logic_TrfMeatbug` in `Spell_ProcessMana`.

```d
INSTANCE Spell_TrfMeatbug_Data(C_SPELL_DATA)
{
    spellId = SPL_TrfMeatbug;
    spellType = SPELL_TYPE_TRANSFORM;
    spellEnergyType = ATR_MANA;
    spellIsInvestSpell = FALSE;
};

func int Spell_Logic_TrfMeatbug(var int manaInvested)
{
    if (self.attribute[ATR_MANA] >= 10)
    {
        self.attribute[ATR_MANA] = self.attribute[ATR_MANA] - 10;
        Npc_SetActiveSpellInfo(self, Meatbug);
        return SPL_SENDCAST;
    };

    return SPL_SENDSTOP;
};
```

Runtime behavior:

- the plugin routes the spell through the engine transform cast path;
- the transformed NPC receives the active spell and `BS_MOD_TRANSFORMED`;
- Enter ends active transform/control spells;
- level-change triggers revert active transform spells before changing worlds;
- `DeleteCaster` is enabled for custom transform ids.

## Spread FX

Use `SPELL_TYPE_SPREAD` when a spell has a projectile phase and a spread child
effect. The plugin uses the visual name to decide which collision class to use:

- if `visName_S` contains `_SPREAD`, the FX uses pass-through box collision;
- otherwise it is treated as a projectile.

This mirrors the vanilla special cases for spells such as firestorm-like FX,
but works for custom ids.

## Invested Scrolls

Sustained spells such as telekinesis and Gothic 1-style pyrokinesis apply their
effect during the invest phase while the action button is held. They stop on
release, so vanilla scroll removal can miss the consumed scroll.

Set `spellIsInvestSpell = TRUE` for these spells to enable scroll consumption
on the invest/stop path:

```d
INSTANCE Spell_MyInvestedSpell_Data(C_SPELL_DATA)
{
    spellId = SPL_MyInvestedSpell;
    spellType = SPELL_TYPE_DEFAULT;
    spellEnergyType = ATR_MANA;
    spellIsInvestSpell = TRUE;
};
```

This flag only controls SpellManager's scroll-removal hook. The spell still
needs normal invest logic in `Spell_Logic_*`, such as `SPL_RECEIVEINVEST`,
`SPL_NEXTLEVEL`, `SPL_SENDCAST`, or `SPL_SENDSTOP`.
Check `Spell_ProcessMana_Release` too: the G2A scripts used for these examples default to
`SPL_SENDSTOP` on release. Add a branch returning `SPL_SENDCAST` for a charged
spell that should fire on release; use a stop branch with any necessary cleanup
for a sustained effect. Charging before a normal final cast does not by itself
require `spellIsInvestSpell = TRUE`.

## Assets

`ASSETS.zip` and the unpacked `ASSETS/_work` folder contain optional animation
assets for the built-in custom telekinesis and control behavior.

Use these when adding spells of the following types to a game or model that
lacks the corresponding animations:

- `SPELL_TYPE_TELEKINESIS`
- `SPELL_TYPE_CONTROL`

G1 already has the original Control and Telekinesis animations; merge only
missing declarations and files. The G2A setup above uses the supplied snippets.

Copy the animation declarations inside each snippet's `aniEnum` into the
existing human model script used by your mod. Copying the standalone
`*_ADD_TO_HUMANS.mds` file alone does not add those animations to the model.
Use `"TEL"` or `"CON"` in `spellFxAniLetters` for the supplied animations.

Merge `ASSETS/_work` into the game's `_work` directory, keeping the ASC files
under their animation subdirectories, then recompile/package the animations
as usual for your mod pipeline. The ZIP includes an outer `ASSETS` directory;
extracting it at the game root alone does not put the files in the game's
`_work` tree.

## Internal Behavior

The plugin creates one spell data table on game init:

- it looks for the parser class `C_SPELL_DATA`;
- it creates every instance of that class;
- it stores `spellId`, `spellType`, `spellEnergyType`, and
  `spellIsInvestSpell`;
- it restores parser instances on level change.

Main hook areas:

- `oCSpell::InitValues`: applies `spellEnergyType`;
- `oCVisualFX::InitEffect` and `SetCollisionEnabled`: applies projectile/spread
  collision behavior;
- `oCSpell::CastSpecificSpell`: routes custom control and transform ids;
- `oCSpell::EndTimedEffect`, `DoTimedEffect`, and `DeleteCaster`: cleans up
  control and transform spells;
- `oCSpell::IsValidTarget`, `StopTargetEffects`, and `DoLogicInvestEffect`:
  implements telekinesis item behavior;
- `oCAIHuman::CheckActiveSpells`: lets Enter end active transform/control
  effects;
- `oCAIHuman` magic-mode handling: removes scrolls for invested spells;
- `oCTriggerChangeLevel`: reverts active transforms before level change;
- `oCSpawnManager`: prevents the original controlling body from being removed.

## Troubleshooting

**The spell uses the wrong FX or crashes while selecting it.**

Check that `MAX_SPELL` is greater than your highest id and that
`spellFxInstanceNames`, `spellFxAniLetters`, and `TXT_SPELLS` have correctly
aligned entries for every index. Confirm the named spell and FX instances and
the selected casting animations exist.

**The projectile passes through walls.**

Make sure the spell has a `C_SPELL_DATA` instance and that `spellType` is
`SPELL_TYPE_PROJECTILE` or `SPELL_TYPE_SPREAD`. Also check that the VFX receives
the correct spell id through the normal spell setup path, enables collision,
and has the appropriate static collision actions.

**The spell does not damage NPCs or apply its victim effect.**

Check `damage_per_level`, `damageType`, targeting, and the VFX's dynamic
collision settings. Review `C_CanNpcCollideWithSpell` and, for scripted victim
effects, `B_AssessMagic` and the relevant AI states. Choosing a plugin spell
type does not configure these scripts.

**A spread spell collides incorrectly.**

For `SPELL_TYPE_SPREAD`, visuals with `_SPREAD` in `visName_S` are pass-through;
other visuals are projectile. Rename the child visual or use a different spell
type if that convention does not fit the effect.

**A telekinesis spell does not move the item.**

Check that the caster model has the `S_TELSHOOT` animation and that the spell
uses `"TEL"` in `spellFxAniLetters` and
`targetCollectType = TARGET_TYPE_ITEMS`.

**A control spell errors on end.**

Provide the `ZS_CONTROLLED` state and the `spellFX_Control` VFX family, retain
`CONTROL_LEAVERANGEFX`, and include the required control animations. G2A
already has the range-warning VFX, but lacks the spell VFX and AI states.

**An invested scroll is not removed.**

Set `spellIsInvestSpell = TRUE` on the matching `C_SPELL_DATA` instance. This
applies to scroll stacks handled by the magic book.

**The plugin seems to ignore your spell.**

Confirm that the compiled scripts contain `C_SPELL_DATA`, the instance is parsed
before game init, the `spellId` matches the item's `spell` field, and the DLL is
loaded by Union from `System/Autorun`.

## Source Map

Important source files:

- `src/oCSpell_Data.hpp`: parser data layout;
- `src/oCSpell_DataManager.hpp`: scan and lookup of `C_SPELL_DATA` instances;
- `src/Hooks_oCSpell.hpp`: main spell behavior hooks;
- `src/Hooks_oCVisualFX.hpp`: projectile and spread collision hooks;
- `src/Spell_Telekinesis.hpp`: custom telekinesis behavior;
- `src/Spell_Control.hpp`: custom control behavior;
- `src/Hooks_oCAIHuman.hpp`: active spell and invested scroll hooks;
- `ASSETS/_work/data/anims`: optional animation snippets and ASC files.
