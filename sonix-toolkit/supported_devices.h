#include <vector>
#include <string>

// Structure to hold device identification
struct supported_device {
    unsigned short vid = 0;
    unsigned short pid = 0;

    supported_device() = default;
    supported_device(unsigned short v, unsigned short p) : vid(v), pid(p) {}
};

// Structure to hold detected supported devices found during enumeration
struct detected_device {
    std::string path = "";
    unsigned short vid = 0;
    unsigned short pid = 0;

    detected_device() = default;
    detected_device(std::string p, unsigned short v, unsigned short i)
        : path(p), vid(v), pid(i) {
    }
};

// Global list of supported devices
const std::vector<supported_device> SUPPORTED_DEVICES = {
    { 0x0C45, 0x800A },
    { 0x05AC, 0x024F }
};