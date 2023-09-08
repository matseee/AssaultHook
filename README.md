# AssaultHook
is an internal cheat for the game [AssaultCube](https://github.com/assaultcube/AC) written in c++. I wrote this cheat to refresh my introduction to programming, which was cheat development. The game AssaultCube was chosen for this because it needs little resources, is open source and well documented for cheat development. To find the simple memory addresses of the game the program CheatEngine was used. This was followed by the use of ReClass.net to build structures from the memory area. To find game functions for example the NoRecoil function the debugger x64dbg and IDA Free was used. The guides on [guidedhacking.com](https://guidedhacking.com) are the foundation on which the entire cheat development is based. If you are interested in learning the basics of cheat development and already know a programming language, this is the best source.

After the AssaultHook-DLL is injected into the game, a new thread is created. In this thread the needed hack instances are created and the opengl function [`wglSwapBuffers`](https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-wglswaplayerbuffers) is hooked using a [trampoline function](http://jbremer.org/x86-api-hooking-demystified/#ah-trampoline). The cheat logic is then executed in the hooked function. The advantage of the `wglSwapBuffers` function is that it is called at the end of each rendered frame.

### features
| feature | how its done |
| --- | --- |
| Aimbot | loop over the entitiy list; check if the entity is alive; check if the entity is visible, by calling the `intersectgeometry` function from assault cube; calculate the needed viewing angles; set the viewing angles |
| ESP Box | loop over the entitiy list; calculate the screen position using the [`WorldToScreen`](https://guidedhacking.com/threads/world2screen-direct3d-and-opengl-worldtoscreen-functions.8044/) function; render a rectangle|
| ESP Name | same as esp; but render the name over the entity |
| ESP Health | same as esp; but render a filled rect that changes color and decreases in size when the health drops |
| ESP Line | same as esp; but render a line to the entity |
| Unlimited health | hook `dodamage` function; set damage to 0 if the this-pointer is equal to the `LocalPlayerState` |
| Unlimted ammo | overwrite decrease ammo instructions with `nop` instructions |
| No recoil | overwrite instructions: instead of calculate the recoil just return directly |

#### next features
- [ ] signature scanning
- [ ] detours with assembly for unl. ammo and no recoil
- [ ] fov aimbot
- [ ] dll injector

![Screenshot](./img/screen.gif "screenshot")

## Used tools
- [IDA Free](https://hex-rays.com/ida-free/)
- [Visual Studio 2022](https://visualstudio.microsoft.com/de/)
- [CheatEngine](https://cheatengine.org/)
- [ReClass.net](https://github.com/ReClassNET/ReClass.NET)
- [x64dbg](https://x64dbg.com/)

## Credits
- [guidedhacking.com](https://guidedhacking.com)