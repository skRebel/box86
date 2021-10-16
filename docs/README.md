# box86

![Official logo](img/Box86Logo.png "Official Logo")
Linux Userspace x86 Emulator with a twist

[![Build Status](https://app.travis-ci.com/ptitSeb/box86.svg?branch=master)](https://app.travis-ci.com/ptitSeb/box86)

----

Compiling/Installation
----
> Compilation instructions can be found [here](COMPILE.md)  
> Instructions for installing Wine for Box86 can be found [here](X86WINE.md)  

Box86 lets you run x86 Linux programs (such as games) on non-x86 Linux systems, like ARM (host system needs to be 32bit little-endian).

You *NEED* a 32-bit subsystem to run and build Box86. Box86 is useless on 64-bit only systems. Also, you *NEED* a 32-bit toolchain to build Box86. A toolchain that only supports 64-bit will not compile Box86, and you'll get errors (typically on aarch64, you get "-marm" not recognized, and you'll need a multiarch or chroot environnement).

Because Box86 uses the native versions of some "system" libraries, like libc, libm, SDL, and OpenGL, it's easy to integrate and use with most applications, and performance can be surprisingly high in many cases. Take a look at thoses bench analysis for an example [here](https://box86.org/index.php/2021/06/game-performances/).

Most x86 Games need OpenGL, so on ARM platforms a solution like [gl4es](https://github.com/ptitSeb/gl4es) is usually necessary. (Most ARM platforms only support OpenGL ES and/or their OpenGL implementation is dodgy (see OpenGL on Android).)

Box86 now integrates a DynaRec (dynamic recompiler) for the ARM platform, providing a speed boost between 5 to 10 times faster than only using the interpreter. Some high level information on how the Dynarec work can be found [here](https://box86.org/2021/07/inner-workings-a-high%e2%80%91level-view-of-box86-and-a-low%e2%80%91level-view-of-the-dynarec/).

Many games already work without much tweaking, for example: WorldOfGoo, Airline Tycoon Deluxe, and FTL. Many of the GameMaker Linux games also run fine. (there's a long list, among them are UNDERTALE, A Risk of Rain, and Cook Serve Delicious)

If you are serious about developing Box86, you should install ccache and build Box86 with it. (Use ccmake for example.)
To enable TRACE (i.e. dumping to stdout all individual x86 instructions executed, with dump of registers), you'll also need [Zydis library](https://github.com/zyantific/zydis) available on your system.

Some x86 internal opcodes use parts of "Realmode X86 Emulator Library", see [x86primop.c](../src/emu/x86primop.c) for copyright details

Here's  6 videos, the first 2 videos are videos of "Airline Tycoon Deluxe" and "Heretic 2" running on a GigaHertz OpenPandora (the second one is using  the dynarec), and the next 2 videos are videos of of "Bit.Trip.Runner" and "Neverwinter Night" running on an ODroid XU4 (without dynarec), and the last 2  videos are on on a Pi4: Shovel Knight (video from @ITotalJustice) and Freedom Planet (video from @djazz), also without dynarec.

[![Play on Youtube](https://img.youtube.com/vi/bLt0hMoFDLk/3.jpg)](https://www.youtube.com/watch?v=bLt0hMoFDLk) [![Play on Youtube](https://img.youtube.com/vi/MM7kWYts7IA/3.jpg)](https://www.youtube.com/watch?v=MM7kWYts7IA) [![Play on Youtube](https://img.youtube.com/vi/8hr71S029Hg/1.jpg)](https://www.youtube.com/watch?v=8hr71S029Hg) [![Play on Youtube](https://img.youtube.com/vi/B4YN37z3-ws/1.jpg)](https://www.youtube.com/watch?v=B4YN37z3-ws) [![Play on Youtube](https://img.youtube.com/vi/xk8Q30mxqPg/1.jpg)](https://www.youtube.com/watch?v=xk8Q30mxqPg) [![Play on Youtube](https://img.youtube.com/vi/_QMRMVvYrqU/1.jpg)](https://www.youtube.com/watch?v=_QMRMVvYrqU)

You can find many more Box86 video on Youtube in the [MicroLinux](https://www.youtube.com/channel/UCwFQAEj1lp3out4n7BeBatQ), [Pi Labs](https://www.youtube.com/channel/UCgfQjdc5RceRlTGfuthBs7g) or [The Byteman](https://www.youtube.com/channel/UCEr8lpIJ3B5Ctc5BvcOHSnA) Channels

Compatibility list is there: https://github.com/ptitSeb/box86-compatibility-list/issues

<img src="img/Box86Icon.png" width="96" height="96">

Logo and Icon made by @grayduck, thanks!

Note that this project is not to be mistaken with [86box](https://github.com/86Box/86Box), a nice "Full system" emulator specialized in early (to fairly recent) PC hardware.

----

Usage
----

There are a few environment variables to control the behaviour of Box86.

See [here](USAGE.md) for all environment variables and what they do.

Note: Box86's Dynarec uses a mechanism with Memory Protection and a SegFault signal handler to handle JIT code. In simpler terms, if you want to use GDB to debug a running program that use JIT'd code (like mono/Unity3D), you will still have many "normal" segfaults triggering. It is suggested to use something like `handle SIGSEGV nostop` in GDB to not stop at each segfault, and maybe put a breakpoint inside `my_memprotectionhandler` in `signals.c` if you want to trap SegFaults.

----

Version history
----

The change log is available [here](CHANGELOG.md)

----

Notes about 64-bit platforms
----

Because Box86 works by directly translating function calls from x86 to host system, the host system (the one Box86 is running on) needs to have 32-bit libraries. Box86 doesn't include any 32-bit <-> 64-bit translation. So basically, to run Box86 on, for example, an ARM64 platform, you will need to build Box86 for ARM 32-bit, and also need to have a chroot with 32-bit libraries.

If you look at a 64bits version of box86, look at [Box64](https://github.com/ptitSeb/box64): this one is able to run x86_64 binaries on 64-bit platforms. But note that you still need Box86 (and a 32-bit chroot) to run x86 binaries (as it also happens on actual x86_64 Linux that need x86 libs and binary on multiarch to run).

----

Notes about Unity game emulation
----

Running Unity games is a hit or miss for now. Unity uses Mono (which uses signals that are not well emulated enough), and a runtime embedded in the main binary. A solution would be to use a native version of the libmono library used by Unity (it can be found here: https://github.com/Unity-Technologies/mono and needs to be built from source). But the wrapping of this lib is tricky, and not done for now, so the only solution is to emulate everything. The tricky part is to emulate the "JIT" code emitted by Mono, however with the new "protected memory" mechanism implemented it should be running with correct performance now.
You should also note that some Unity3D games require OpenGL 3+ which can be tricky to provide on ARM SBC (single-board computers) for now.

TL;DR: Not all Unity games work and can require a high OpenGL profile, but the speed, for the ones running, should be correct now.

----

Notes about Steam
----

Linux Steam's can run now with box86. But it's still a bit unstable, and not everything works:
- First problem is Steam crashing after the sign-in window, if you encounter this issue, you may need to add libappindicator. To install it on Debian, run `sudo apt install libappindicator1`.
- If you select to "Remember password", Steam is crashing on subsequent starts, unless you have libnm intalled. To install it on Debian, run `sudo apt install libnm0`.
- Once open, Steam will only work on "Small Mode" and in "Big Picture", not in the regular "Large Mode". This is because some Steam components used in the browser view are only 64-bit now. So go in the "View" menu and switch to "Small view", else the list will stay empty. Alternatively, Steam can be started in small mode directly by using `+open steam://open/minigameslist` command line arguments.
- To avoid the "libc.so.6 is absent" message, you can use `STEAMOS=1` and `STEAM_RUNTIME=1` as environment variables. 
- Some Steam games (most Source engine games, like "Portal" or "Half-Life 2") use libtcmalloc. Box86 will detect it and will try to LD_PRELOAD it, for better compatibility. While it should work without the aformentionned feature, it is safer to add it to your system if you intend to play those game. To install it on Debian, run `sudo apt install libtcmalloc-minimal4`.

Steam for Windows installs fine but doesn't work yet.

----

Notes about Wine
----

Wine is now supported. Wine integrated program all runs, and many windows programs and games also runs fine. Don't forget most Windows games use Direct3D, this may require a complete OpenGL driver and a as high profile as possible (and gl4es with ES2 backend have issue with Wine for now).
Note: if you plan to use box86 with Wine on Raspberry Pi 3 or earlier, those models use a default OS that have a kernel with a 2/2 Split (meaning 2G of space for user program, and 2G of space for the Kernel). This is not compatible with Wine programs that needs to access memory > 2Gb address. So you'll need to reconfigure your kernel for a 3G/1G split.

----

Notes about Vulkan
----

Box86 already wraps vulkan. If your system has a 32bit vulkan driver, box86 will use it when needed. Note that vulkan wrapping has not been tested much, due to the limited vulkan support on the hardware I currently own. Profile 1.0 and 1.1, whith some extension, should be OK. 1.2 is not really wrapped. I know some demos work on Pi4 (Sascha Willems demos build for x86 work the same as if build on armhf directly). Note that the Vulkan driver of the Pi4 DOES NOT support dxvk for now (wine DirectX->Vulkan wrapper). It's not a box86 issue, it's missing extensions (hardxware support) and a few other things that make dxvk not working on pi4. On Panfrost side, PanVK is a bit young and I haven't tested dxvk with it yet.

----
Final word
----

I want to thank everyone who has contributed to box86 developpement.
There are many ways to contribute: code, financial, hardware and advertisement!
So, in no particular order, I want to thank:
 * For their major code contribution: rajdakin, icecream95, M-HT
 * For their major financial contribution: FlyingFathead, stormchaser3000
 * For their hardware contribution: [Radxa](https://rockpi.org/), [Pine64](https://www.pine64.org/), [DragonBox](https://pyra-handheld.com/), [Novaspirit](https://www.youtube.com/channel/UCrjKdwxaQMSV_NDywgKXVmw), [HardKernel](https://www.hardkernel.com/), [TwisterOS team](https://twisteros.com/)
 * For their continous advertisement of box86 project: salva ([microLinux](https://www.youtube.com/channel/UCwFQAEj1lp3out4n7BeBatQ)), [PILab](https://www.youtube.com/channel/UCgfQjdc5RceRlTGfuthBs7g)/[TwisterOS team](https://twisteros.com/), [The Byteman](https://www.youtube.com/channel/UCEr8lpIJ3B5Ctc5BvcOHSnA), [NicoD](https://www.youtube.com/channel/UCpv7NFr0-9AB5xoklh3Snhg)

And I also thank the many other people who participated even once in the project.

(If you use Box86 in your project, please don't forget to mention it!)
