1. Download USBTabletINIT.sit from https://github.com/kanjitalk755/macos9-usb-tablet/releases.
1. Expand it and move to System Folder of guest MacOS9.
1. Append "-device usb-tablet" instead of "-device usb-mouse" to argument of QEMU.

The project file was made by CodeWarrior Pro 6.  
To build:
1. Expand `icon.rsrc.sit`
1. Change file creator of `USBTabletRSRC.prj` and `USBTabletINIT.prj` to 'CWIE'.
1. Open `USBTabletRSRC.prj` and build.
1. Open `USBTabletINIT.prj` and build.

Do not add `via=pmu` to the QEMU argument `-M` as it will not work properly.
