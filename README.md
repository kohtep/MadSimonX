# MadSimonX

A debugging environment builder for the **Cry of Fear** mod.

## More Details

<p align="center">
  <img src="https://github.com/kohtep/kohtep/blob/main/resources/shared/cof-msx-preview.jpg" alt="Header Image">
</p>

**MadSimonX** is a from-scratch rewrite of the original **MadSimon** project, with a complete rework of all its functionality and underlying techniques.  
It replicates most of the original features, but now the source code is open.

The project is also adapted for the current **Steam** version of the game.

> [!NOTE]  
> **MadSimon** is my custom debugging tool for the game **Cry of Fear**, which I originally wrote many years ago just for fun - and which unexpectedly became relatively popular among speedrunners.  
> I developed it for personal use and later shared it with one person, and at some point, it gained popularity.  
> It is most likely outdated by now, but that doesn't really matter.

## Why?

During an online game, I accidentally met a few players with whom I briefly discussed the **MadSimon** project - something I had mostly forgotten about.
Although they mentioned that several alternatives have existed for quite some time, our conversation inspired me to release the original **MadSimon** source code for educational purposes.

However, since the original project was written purely for personal use, I didn't pay much attention to code quality back then.
So, I decided to quickly rewrite it from scratch - and that's how **MadSimonX** was born.

After all, it's a nice addition to the portfolio, isn't it?

> [!WARNING]  
> Please take this into account. Some code may still be questionable.  
> If something looks odd, it's most likely old code - back then I wasn't very mindful of quality, and right now I don't have time to fix absolutely everything.

## What About the Features?

The original **MadSimon** included various experimental functions I created to test the engine, as well as features such as:

* Disabling speed slowdown while bunnyhopping
* Console commands for manipulating entities and the player character
* Unlocking **noclip**, **god**, and **notarget** modes
* Real-time debugging information about entities
* Various small additions made just for fun

The rewritten version of **MadSimon** also introduces additional features, such as:

* Raw Input for the mouse (m_rawinput 1; experimental feature)

## Console Commands maybe?

Sure thing. For convenient control, it's best to use the engine's built-in API rather than reinventing your own system.  
A full list of available commands is provided below.

| Command / CVar | Description |
|-----------------|-------------|
| `find` | Search for console commands and variables by substring. |
| `ent_create` | Spawns an entity at the position the player is currently looking at. |
| `ent_remove` | Removes the entity the player is currently looking at. |
| `ent_killed` | Kills the entity the player is currently looking at. |
| `ent_setname` | Changes the entity's name. Honestly, I don't remember why this was needed. |
| `ent_getname` | Retrieves the entity's name. |
| `ent_dump` | Collects a list of all spawnable entities and prints it to the console. |
| `ent_find` | Collects a list of all entities with filtering. |
| `ent_info` | If set to `1`, displays information about all entities on screen. |
| `ent_info_type` | Displays only entities of the specified type. |
| `ent_info_filter` | Displays only entities with the specified name. |
| `give` | Gives the player an item. Example: `weapon_rifle`. |
| `hurtme` | Deals damage to the player. |
| `smn_about` | Displays product information. |
| `smn_kohtep` | Personal debugging function for granting a favorite weapon. |
| `smn_kohtep2` | Personal debugging function for dropping weapons, items, and ammunition in front of the player. |
| `smn_nofear` | If I'm not mistaken, this command disables Simon's breathing during the forest scene. |
| `smn_showinfo` | Displays various debugging information about the game world. |
| `smn_infinite_stamina` | Infinite stamina. |
| `smn_infinite_health` | Infinite health. |
| `smn_infinite_ammo` | Infinite ammo. |
| `smn_noviewpunch` | Disables view shaking, including camera shake from jumping or side dashing. |
| `smn_bhop` | When set to `1`, enables bunny hopping. |
| `smn_always_first_deploy` | Always plays the first-time weapon draw animation. I liked how it looked and enjoyed watching it. |
| `hud_nprintf_time` | Sets the duration (in seconds) for how long `nprintf` info is displayed on screen. |
| `m_rawinput` | **MadSimon** implements the `m_rawinput` variable for Cry of Fear. The behavior is the same as in other GoldSource games. |

## How to Use

Place the **MadSimon** library, after renaming it to `CrashRpt.dll`, into the `cryoffear\cl_dlls` folder. In this case, **MadSimon** will be loaded automatically each time the game starts.

Alternatively, use any injection method convenient for you, for example via **Process Hacker**.

## License

This project is distributed under the **2-Clause BSD License**.

Basically, you can use, modify, and share it freely, as well as use it as a learning resource - just make sure to give proper credit when using or sharing it.

## Third-party components

This project includes the Hacker Disassembler Engine (HDE) by Vyacheslav Patkov, licensed under the BSD 2-Clause License.  
See `vendor/hde/LICENSE` for details.
