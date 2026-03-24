# sonix-toolkit
Synchronize time of your Archon/AULA keyboard LCD

### ⚠️ WARNING: USE AT YOUR OWN RISK!

This tool is **NOT an official software**. It was developed through reverse engineering to mimic the original USB communication protocols. While every effort has been made to ensure accuracy, the following risks apply:

1.  **NO WARRANTY**: This tool is provided "as-is." Use of this software may cause malfunctions, data loss, or permanent hardware damage.
2.  **LCD-LESS KEYBOARD DANGER**: If you run this tool while a keyboard without an LCD (but sharing the same VID/PID) is connected, **there is a high probability of causing severe malfunction or bricking the device.**
3.  **COMPATIBILITY**: This tool is specifically designed for keyboards with LCD screens. Do not attempt to use it on other models.
4.  **CUSTOM VID/PID RISK**: When providing custom VID/PID via arguments, the internal supported device list is completely ignored. This will force the tool to send packets to the specified device, which may cause unpredictable behavior or hardware damage if the target is not a compatible device.

**BY PROCEEDING, YOU ACKNOWLEDGE THESE RISKS. YOU HAVE BEEN WARNED.**

### Usage
Launching without any arguments will automatically iterate through connected USB HID devices, look for compatible devices, and sync them.

If you need specific options, use the arguments below:
```
Usage: sonix-toolkit.exe [options]

If no options are provided, the tool will automatically scan all HID devices,
match them against the internal compatibility list, and synchronize the time.

Options:
  -v, --vid <hex>      Override target Vendor ID (e.g. 0C45)
  -p, --pid <hex>      Override target Product ID (e.g. 800A)
  -d, --date <date>    Set custom date (yyyy-mm-dd)
  -t, --time <time>    Set custom time in 24h format (hh:mm:ss, e.g. 15:30:00)
  -V, --verbose        Enable detailed logging
  -h, --help           Show this help message

Notes:
  * By default, all matched devices in the supported list will be synchronized.
  * If you use --vid or --pid, the internal supported list will be ignored.
  * --date and --time must be used together if provided.
```
Note: You might need to run the application with administrator privileges to access HID devices.

### Build
Building should be straight forward since this project uses CMake and vcpkg.
