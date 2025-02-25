#pragma once

#include "HIDLampArrayReports.h"

#include <string>
#include <vector>
#include <hidapi/hidapi.h>

/**
 * @brief LampArray HID device references to reports
 */
struct lamp_array_devices
{
    hid_device *lamp_array;
    hid_device *attributes_report;
    hid_device *request_report;
    hid_device *response_report;
    hid_device *multi_update_report;
    hid_device *range_update_report;
    hid_device *control_report;
};

/**
 * @brief RGB Color Structure
 */
struct color_type {
    uint16_t idx;
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

/**
 * @brief Manipulate Rodent. LampArray devices
 */
class RodentLampArrayController
{
public:
    RodentLampArrayController() = delete;
    RodentLampArrayController(lamp_array_devices devices, hid_device_info *info);

private:
    /**
     * @brief References to report descriptors devices
     */
    lamp_array_devices devices;

    std::string name;
    std::string vendor;
    std::string serial;
    std::string path;

public:
    std::string GetName();
    std::string GetVendor();
    std::string GetSerial();
    std::string GetPath();

private:
    /**
     * @brief LampArrayAttributesReport gathered from device
     */
    LampArrayAttributesReport lamp_array;

    /**
     * @brief LampAttributesResponseReport for each of the declared Lamps
     */
    std::vector<LampAttributesResponseReport> lamps{};

    /**
     * @brief Initialize reports
     */
    void init();

public:
    /**
     * @brief Get the number of Lamp items registered
     * @return uint16_t Number of lamps
     */
    uint16_t GetLampCount();

    /**
     * @brief Update all the lamps for their given data
     * @param update Vector with lamp update data
     */
    void LampMultiUpdate(std::vector<color_type> &update);

    /**
     * @brief Update a single LED
     * @param color Color to update to
     */
    void LampSingleUpdate(color_type &color);
};