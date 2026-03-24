# sonix-toolkit
Synchronize time of your Archon/AULA keyboard LCD

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