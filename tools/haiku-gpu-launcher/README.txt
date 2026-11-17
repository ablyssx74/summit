Summit with GPU compositing and WebGL on a stock Haiku (NVK from Nebula)

Summit (WebKit browser from air/OS, github.com/jmgasper/summit, Haiku build "latest-haiku") uses its own private Mesa with Zink, which goes
through the Vulkan loader to Nebula's NVK. On this machine the GPU path works, but Summit's web process crashed at start-up: worker threads
that Mesa starts jumped to address 0 in pthread_once() (Mesa's util_call_once_data_slow() read back a thread_local value that the same thread
had just stored, and got zeros).

libsummit_tlsfix.so works around it: every thread the process starts first touches thread-local data twice, then runs its routine. It is
preloaded with LD_PRELOAD, which Summit passes on to its network and web processes. With it WebGL 1 and 2 render on the GPU.
The real cause is not proven (suspect: Haiku's runtime_loader, src/system/runtime_loader/elf_tls.cpp, per-thread generation counter).

Package (recommended)
  ./build-package.sh   on Haiku (shredder) builds out/summit_gpu-0.1.0-1-x86_64.hpkg; installing it puts "Summit GPU" in /boot/system/apps (so it is in
  Deskbar's Applications menu with Summit's icon) and libsummit_tlsfix.so in /boot/system/lib. The launcher (launcher.cpp) starts Summit as a team of its
  own with LD_PRELOAD set; it needs the summit package. The files below are the older, hand-installed way (still work).

Files
  tlsfix.cpp     the library (about 40 lines)
  summit-gl      launcher: sets LD_PRELOAD and starts /boot/system/apps/Summit with the arguments it gets
  launcher.cpp / PackageInfo / build-package.sh   the package (see above)
  install.sh     builds the library into ~/config/non-packaged/lib and puts the launcher in ~/config/non-packaged/bin

Use
  ./install.sh
  summit-gl [URL] [--profile DIR]

Things that start Summit directly do NOT get the workaround and will crash the web process again:
  - Summit's own "Install and Restart" after an update (it reopens /boot/system/apps/Summit)
  - double-clicking Summit in Tracker or the Applications menu, or opening a link/file with Summit as the handler
Use summit-gl (or the "Summit (GPU)" Deskbar entry) instead, and reinstall this after a Haiku or Summit update if the crash comes back.
