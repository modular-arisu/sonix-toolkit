#include "sonix-toolkit.h"
#include "supported_devices.h"

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstring>
#include <ctime>
#include <regex>
#include <hidapi.h>

// App metadata
std::string APP_VERSION = "v0.1.0";
std::string APP_AUTHOR = "modular-arisu";

/**
 * @brief Sends a feature report and waits for sync response.
 * @return true if both send and get succeed, false otherwise.
 */
bool send_and_sync(hid_device* handle, const unsigned char* data) {
    unsigned char buf[65];
    memset(buf, 0, sizeof(buf));

    buf[0] = 0x00; // Report ID
    memcpy(&buf[1], data, 64);

    // Send Set_Report
    if (hid_send_feature_report(handle, buf, 65) < 0) {
        fprintf(stderr, "[%s] Send failed: %ls\n", "ERROR", hid_error(handle));
        return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Get Get_Report to sync
    if (hid_get_feature_report(handle, buf, 65) < 0) {
        fprintf(stderr, "[%s] Read sync failed: %ls\n", "ERROR", hid_error(handle));
        return false;
    }

    return true;
}

void get_now(struct tm* result) {
    time_t t = time(NULL);
#ifdef _WIN32
    localtime_s(result, &t);
#else
    localtime_r(&t, result);
#endif
}

bool sync_time(const char* dev_path, struct tm* time_data) {
    hid_device* handle = hid_open_path(dev_path);
    if (!handle) {
        std::cerr << "Failed to open device handle." << std::endl;
        return false;
    }

    bool success = true;

    // Step 1: Init 1
    unsigned char init1[64] = { 0x04, 0x18 };
    if (!send_and_sync(handle, init1)) success = false;

    // Step 2: Init 2
    if (success) {
        unsigned char init2[64] = { 0x04, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
        if (!send_and_sync(handle, init2)) success = false;
    }

    // Step 3: Time Sync
    if (success) {
        unsigned char time_pkt[64] = { 0, };

        time_pkt[1] = 0x01;
        time_pkt[2] = 0x5a;
        time_pkt[3] = (unsigned char)(time_data->tm_year % 100);
        time_pkt[4] = (unsigned char)(time_data->tm_mon + 1);
        time_pkt[5] = (unsigned char)time_data->tm_mday;
        time_pkt[6] = (unsigned char)time_data->tm_hour;
        time_pkt[7] = (unsigned char)time_data->tm_min;
        time_pkt[8] = (unsigned char)time_data->tm_sec;
        time_pkt[10] = 0x21;
        time_pkt[11] = 0x01;
        time_pkt[13] = 0x04;
        time_pkt[62] = 0xaa;
        time_pkt[63] = 0x55;

        if (!send_and_sync(handle, time_pkt)) success = false;
    }

    // Step 4: Apply
    if (success) {
        unsigned char apply_pkt[64] = { 0x04, 0x02 };
        if (!send_and_sync(handle, apply_pkt)) success = false;
    }

    // 4. Final Result
    if (success) {
        printf("Sync Completed Successfully: %04d-%02d-%02d %02d:%02d:%02d\n",
            time_data->tm_year + 1900,
            time_data->tm_mon + 1,
            time_data->tm_mday,
            time_data->tm_hour,
            time_data->tm_min,
            time_data->tm_sec);
    }
    else {
        std::cerr << "Sync failed during protocol steps." << std::endl;
    }

    hid_close(handle);
    return success;
}

int main(int argc, char* argv[]) {
    printf("SONiX Keyboard Toolkit (%s) by %s\n\n",
        APP_VERSION.c_str(), APP_AUTHOR.c_str());

    unsigned short target_vid = 0x0;
    unsigned short target_pid = 0x0;
    std::string target_time = "";
    std::string target_date = "";
    bool is_custom_path = false;
    bool is_custom_datetime = false;

    // Regex patterns for validation
    // Time: hh:mm:ss (24h format)
    std::regex time_pattern(R"(^([01][0-9]|2[0-3]):([0-5][0-9]):([0-5][0-9])$)");
    // Date: yy-mm-dd
    std::regex date_pattern(R"(^\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12][0-9]|3[01])$)");

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        // 1. VID check (supports --vid and -v)
        if ((arg == "--vid" || arg == "-v") && i + 1 < argc) {
            target_vid = (unsigned short)std::stoul(argv[++i], nullptr, 16);
            is_custom_path = true;
        }
        // 2. PID check (supports --pid and -p)
        else if ((arg == "--pid" || arg == "-p") && i + 1 < argc) {
            target_pid = (unsigned short)std::stoul(argv[++i], nullptr, 16);
            is_custom_path = true;
        }
        // 3. Time check (supports --time and -t)
        else if ((arg == "--time" || arg == "-t") && i + 1 < argc) {
            std::string val = argv[++i];
            if (std::regex_match(val, time_pattern)) {
                target_time = val;
                is_custom_datetime = true;
            }
            else {
                std::cerr << "Invalid time format. Expected hh:mm:ss" << std::endl;
                return -1;
            }
        }
        // 4. Date check (supports --date and -d)
        else if ((arg == "--date" || arg == "-d") && i + 1 < argc) {
            std::string val = argv[++i];
            if (std::regex_match(val, date_pattern)) {
                target_date = val;
                is_custom_datetime = true;
            }
            else {
                std::cerr << "Invalid date format. Expected yy-mm-dd" << std::endl;
                return -1;
            }
        }
    }

    if (target_date.empty() != target_time.empty()) {
        std::cerr << "Error: Both --date and --time must be provided together." << std::endl;
        return -1;
    }

    // Parse custom datetime
    struct tm custom_tm = { 0 };

    if (is_custom_datetime) {
        int year, mon, mday, hour, min, sec;

        int date_parsed = sscanf_s(target_date.c_str(), "%d-%d-%d", &year, &mon, &mday);
        int time_parsed = sscanf_s(target_time.c_str(), "%d:%d:%d", &hour, &min, &sec);

        if (date_parsed == 3 && time_parsed == 3) {
            custom_tm.tm_year = year - 1900; // Packet logic uses % 100
            custom_tm.tm_mon = mon - 1;      // struct tm uses 0-11
            custom_tm.tm_mday = mday;
            custom_tm.tm_hour = hour;
            custom_tm.tm_min = min;
            custom_tm.tm_sec = sec;
        }
        else {
            std::cerr << "Failed to parse date or time string correctly." << std::endl;
            return -1;
        }
    }

    if (hid_init()) {
        std::cerr << "Failed to init hidapi library." << std::endl;
        return -1;
    }

    if (is_custom_path) {
        std::cout << "Custom VID/PID provided:" << std::endl;
        printf("  Target VID: 0x%04X\n", target_vid);
        printf("  Target PID: 0x%04X\n\n", target_pid);
    }

    if (is_custom_datetime) {
        std::cout << "Custom date and time provided:" << std::endl;
        printf("  %s %s\n\n", target_date.c_str(), target_time.c_str());
    }

    struct hid_device_info* devs;
    struct hid_device_info* cur_dev;
    devs = hid_enumerate(0x0, 0x0);

    std::vector<detected_device> detected_devices;

    for (cur_dev = devs; cur_dev != nullptr; cur_dev = cur_dev->next) {
        for (const auto& supported : SUPPORTED_DEVICES) {

            // Is on the supported device list
            if (cur_dev->vendor_id == supported.vid && cur_dev->product_id == supported.pid) {

                // Knock to see if interface is open to set/get feature
                hid_device* handle = hid_open_path(cur_dev->path);
                if (handle) {
                    unsigned char probe_buf[65] = { 0 };
                    if (hid_get_feature_report(handle, probe_buf, 65) > 0) {
                        // Is open
                        detected_device dd;
                        dd.path = cur_dev->path;
                        dd.vid = cur_dev->vendor_id;
                        dd.pid = cur_dev->product_id;
                        dd.model_name = supported.model_name;
                        detected_devices.push_back(dd);
                    }
                    hid_close(handle);
                }
            }
        }
    }

    hid_free_enumeration(devs);

    for (const auto& dev : detected_devices) {
        std::cout << dev.path << std::endl;

        struct tm time_data = { 0 };

        if (is_custom_datetime) {
            time_data = custom_tm;
        }
        else {
            // Default to current system time if no input provided
            get_now(&time_data);
        }

        sync_time(dev.path.c_str(), &time_data);
    }

    hid_exit();
}
