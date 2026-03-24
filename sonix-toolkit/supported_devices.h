#include <vector>
#include <string>

// Structure to hold device identification
struct supported_device {
    unsigned short vid;
    unsigned short pid;
    std::string model_name;
};

// Structure to hold detected supported devices found during enumeration
struct detected_device {
    std::string path;
    unsigned short vid;
    unsigned short pid;
    std::string model_name;
};

// Global list of supported devices
const std::vector<supported_device> SUPPORTED_DEVICES = {
    { 0x0C45, 0x800A, "USB" },
    { 0x05AC, 0x024F, "2.4G" }
};