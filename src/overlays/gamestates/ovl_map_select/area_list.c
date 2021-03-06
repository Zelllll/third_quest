#include "global.h"

MapSelectArea gMapSelectSceneList[] = {
    // 3Q Scenes
    // [REDACTED]
    // OoT Scenes
    { "Hyrule Field", 0x00CD },
    { "Kakariko Village", 0x00DB },
    { "Graveyard", 0x00E4 },
    { "Zora's River", 0x00EA },
    { "Kokiri Forest", 0x00EE },
    { "Sacred Forest Meadow", 0x00FC },
    { "Lake Hylia", 0x0102 },
    { "Zora's Domain", 0x0108 },
    { "Zora's Fountain", 0x010E },
    { "Gerudo Valley", 0x0117 },
    { "Lost Woods", 0x011E },
    { "Desert Colossus", 0x0123 },
    { "Gerudo's Fortress", 0x0129 },
    { "Haunted Wasteland", 0x0130 },
    { "Hyrule Castle", 0x0138 },
    { "Death Mountain Trail", 0x013D },
    { "Death Mountain Crater", 0x0147 },
    { "Goron City", 0x014D },
    { "Lon Lon Ranch", 0x0157 },
    { "Temple Of Time", 0x0053 },
    { "Chamber of Sages", 0x006B },
    { "Shooting Gallery", 0x003B },
    { "Castle Garden", 0x007A },
    { "Grave 1", 0x031C },
    { "Grave 2", 0x004B },
    { "Royal Family's Tomb", 0x002D },
    { "Great Fairy's Fountain (Din)", 0x0315 },
    { "Great Fairy's Fountain (Farore)", 0x036D },
    { "Great Fairy's Fountain (Nayru)", 0x0371 },
    { "Ganon's Tower - Collapsing", 0x043F },
    { "Castle Courtyard", 0x0400 },
    { "Fishing Pond", 0x045F },
    { "Bombchu Bowling Alley", 0x0507 },
    { "Lon Lon Ranch House", 0x004F },
    { "Lon Lon Ranch Silo", 0x05D0 },
    { "Guard House", 0x007E },
    { "Potion Shop", 0x0072 },
    { "Treasure Chest Game", 0x0063 },
    { "House Of Skulltula", 0x0550 },
    { "Entrance to Market	", 0x0033 },
    { "Market", 0x00B1 },
    { "Back Alley	", 0x00AD },
    { "Temple of Time Exterior", 0x0171 },
    { "Link's House", 0x00BB },
    { "Kakariko House 1", 0x02FD },
    { "Back Alley House 1", 0x043B },
    { "Know-it-All Brothers House", 0x00C9 },
    { "House of Twins", 0x009C },
    { "Mido's House", 0x0433 },
    { "Saria's House", 0x0437 },
    { "Stable", 0x02F9 },
    { "Grave Keeper's Hut", 0x030D },
    { "Dog Lady's House", 0x0398 },
    { "Impa's House", 0x039C },
    { "Lakeside Laboratory", 0x0043 },
    { "Running Man's Tent", 0x03A0 },
    { "Bazaar", 0x00B7 },
    { "Kokiri Shop", 0x00C1 },
    { "Goron Shop", 0x037C },
    { "Zora Shop", 0x0380 },
    { "Closed Shop", 0x0384 },
    { "Potion Shop", 0x0388 },
    { "Bombchu Shop (Back Alley)", 0x0390 },
    { "Happy Mask Shop", 0x0530 },
    { "Gerudo Training Ground", 0x0008 },
    { "Inside the Deku Tree", 0x0000 },
    { "Gohma's Lair", 0x040F },
    { "Dodongo's Cavern", 0x0004 },
    { "King Dodongo's Lair", 0x040B },
    { "Inside Jabu-Jabu's Belly", 0x0028 },
    { "Barinade's Lair", 0x0301 },
    { "Forest Temple", 0x0169 },
    { "Phantom Ganon's Lair", 0x000C },
    { "Bottom of the Well", 0x0098 },
    { "Shadow Temple", 0x0037 },
    { "Bongo Bongo's Lair", 0x0413 },
    { "Fire Temple", 0x0165 },
    { "Volvagia's Lair", 0x0305 },
    { "Water Temple", 0x0010 },
    { "Morpha's Lair", 0x0417 },
    { "Spirit Temple", 0x0082 },
    { "Iron Knuckle's Lair", 0x008D },
    { "Twinrova's Lair", 0x05EC },
    { "Stairs to Ganondorf's Lair", 0x041B },
    { "Ganondorf's Lair", 0x041F },
    { "Ice Cavern", 0x0088 },
    { "Damp?? Grave Relay", 0x044F },
    { "Inside Ganon's Castle", 0x0467 },
    { "Final Battle With Ganon", 0x0517 },
    { "Escaping Ganon's Castle 1", 0x0179 },
    { "Escaping Ganon's Castle 2", 0x01B5 },
    { "Escaping Ganon's Castle 3", 0x03DC },
    { "Escaping Ganon's Castle 4", 0x03E4 },
    { "Escaping Ganon's Castle 5", 0x056C },
    { "Thieves' Hideout 1", 0x0486 },
    { "Thieves' Hideout 2", 0x048E },
    { "Thieves' Hideout 3", 0x0496 },
    { "Thieves' Hideout 4", 0x049E },
    { "Thieves' Hideout 5", 0x04AE },
    { "Thieves' Hideout 6", 0x0570 },
    { "Grotto 1", 0x003F },
    { "Grotto 2", 0x0598 },
    { "Grotto 3", 0x059C },
    { "Grotto 4", 0x05A0 },
    { "Grotto 5", 0x05A4 },
    { "Grotto 6", 0x05A8 },
    { "Grotto 7", 0x05AC },
    { "Grotto 8", 0x05B0 },
    { "Grotto 9", 0x05B4 },
    { "Grotto 10", 0x05B8 },
    { "Grotto 11", 0x05BC },
    { "Grotto 12", 0x05C0 },
    { "Grotto 13", 0x05C4 },
    { "Grotto 14", 0x05FC },
    { "Goddess Cutscene Environment", 0x00A0 },
    { "Treasure Chest Warp", 0x0520 },
    { "SRD Map", 0x0018 },
    { "Test Map", 0x0094 },
    { "Test Room", 0x0024 },
    { "Stalfos Miniboss Room", 0x001C },
    { "Stalfos Boss Room", 0x0020 },
    { "Dark Link Room", 0x0047 },
    { "Shooting Gallery Duplicate", 0x02EA },
    { "Depth Test", 0x00B6 },
    { "Castle Garden Test", 0x0076 },
};
