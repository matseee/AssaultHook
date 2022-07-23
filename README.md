# AssaultHook
is an internal cheat for the game [AssaultCube](https://github.com/assaultcube/AC) written in c++. I wrote this cheat to refresh my introduction to programming, which was cheat development. The game AssaultCube was chosen for this because it needs little resources, is open source and well documented for cheat development. To find the simple memory addresses of the game the program CheatEngine was used. This was followed by the use of ReClass.net to build structures from the memory area. To find game functions for example the NoRecoil function the debugger x64dbg and the SRE(software reverse engineering) framework ghidra (mostly disassembly and decompilation) was used. The guides on [guidedhacking.com](https://guidedhacking.com) are the foundation on which the entire cheat development is based. If you are interested in learning the basics of cheat development and already know a programming language, this is the best source.

Small disclaimer: I have never used the cheat online and will not use it online.

After the AssaultHook-DLL is injected into the game, a new thread is created. In this thread the needed hack instances are created and the opengl function [`wglSwapBuffers`](https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-wglswaplayerbuffers) is hooked using a [trampoline function](http://jbremer.org/x86-api-hooking-demystified/#ah-trampoline). The cheat logic is then executed in the hooked function. The advantage of the `wglSwapBuffers` function is that it is called at the end of each rendered frame.

### The cheat has the following features:
| feature | how its done |
| --- | --- |
| Unlimited health | set integer every tick |
| Unlimted ammo | set integer every tick |
| No recoil | overwrite instructions: instead of calculate the recoil just return directly |
| ESP | loop over the entitiy list; calculate the screen position using the [`WorldToScreen`](https://guidedhacking.com/threads/world2screen-direct3d-and-opengl-worldtoscreen-functions.8044/) function; render a rectangle|
| Snapline | same as esp; but render a line to the entity |
| Aimbot | loop over the entitiy list; check if the entity is alive; check if the entity is visible, by calling the trace line function from assault cube; calculate the needed viewing angles; set the viewing angles |


![Screenshot](./img/screen.png "screenshot")

## Used tools
- [CheatEngine](https://cheatengine.org/)
- [ghidra](https://github.com/NationalSecurityAgency/ghidra)
- [ReClass.net](https://github.com/ReClassNET/ReClass.NET)
- [Visual Studio 2022](https://visualstudio.microsoft.com/de/)
- [x64dbg](https://x64dbg.com/)

## Credits
- [guidedhacking.com](https://guidedhacking.com)