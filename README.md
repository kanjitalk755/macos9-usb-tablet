1. Download USBTablet.sit from https://github.com/kanjitalk755/macos9-usb-tablet/releases.
1. Expand it and move to System Folder -> Startup Items of guest MacOS9.
1. Append "-device usb-tablet" instead of "-device usb-mouse" to argument of QEMU.

The project file was made by CodeWarrior Pro 6.  
To build, change file creator of `USBTablet.prj` to 'CWIE' and double click.
