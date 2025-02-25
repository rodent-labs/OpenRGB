#include <stdexcept>

#include "RodentLampArrayController.h"
#include "StringUtils.h"
#include "LogManager.h"

RodentLampArrayController::RodentLampArrayController(
    lamp_array_devices devices,
    hid_device_info *info) : devices(devices)
{
    // Get product name
    std::wstring product_str{info->product_string};
    this->name = StringUtils::wstring_to_string(product_str);

    // Get manufacturer
    std::wstring manufacturer_str{info->manufacturer_string};
    this->vendor = StringUtils::wstring_to_string(manufacturer_str);

    // Get serial
    std::wstring serial_str{info->serial_number};
    this->serial = StringUtils::wstring_to_string(serial_str);

    // Get the path
    this->path = std::string{info->path};

    // Initialize data
    this->init();
}

void RodentLampArrayController::init()
{
    int bytes = 0;

    // Gather the report
    bytes = hid_get_feature_report(
        this->devices.attributes_report,
        (unsigned char *)&(this->lamp_array),
        sizeof(LampArrayAttributesReport));

    if (bytes <= 0)
    {
        std::wstring error_str{hid_error(this->devices.attributes_report)};
        throw std::runtime_error{StringUtils::wstring_to_string(error_str)};
    }

    // 2. LampAttributesRequestReport
    LampAttributesRequestReport request_report = {
        .LampId = 0U,
    };

    bytes = hid_send_feature_report(
        this->devices.request_report,
        (const unsigned char *)&request_report,
        sizeof(LampAttributesRequestReport));

    if (bytes <= 0)
    {
        std::wstring error_str{hid_error(this->devices.attributes_report)};
        throw std::runtime_error{StringUtils::wstring_to_string(error_str)};
    }

    // Get other lamps
    for (uint16_t LampId = 0U; LampId < this->lamp_array.LampCount; LampId++)
    {
        // Create lamp report
        LampAttributesResponseReport lamp{};

        // Gather the report
        bytes = hid_get_feature_report(
            this->devices.response_report,
            (unsigned char *)&(this->lamp_array),
            sizeof(LampAttributesResponseReport));

        if (bytes <= 0)
        {
            std::wstring error_str{hid_error(this->devices.attributes_report)};
            throw std::runtime_error{StringUtils::wstring_to_string(error_str)};
        }

        // Push result
        this->lamps.push_back(lamp);
    }

    // 3. LampArrayControlReport (AutonomousMode == disabled)
    LampArrayControlReport control_report = {.AutonomousMode = 0x00};

    // Send report
    bytes = hid_send_feature_report(
        this->devices.control_report,
        (const unsigned char *)&control_report,
        sizeof(LampArrayControlReport));

    if (bytes <= 0)
    {
        std::wstring error_str{hid_error(this->devices.attributes_report)};
        throw std::runtime_error{StringUtils::wstring_to_string(error_str)};
    }
}

std::string RodentLampArrayController::GetName()
{
    return name;
}

std::string RodentLampArrayController::GetVendor()
{
    return vendor;
}

std::string RodentLampArrayController::GetSerial()
{
    return serial;
}

std::string RodentLampArrayController::GetPath()
{
    return path;
}

uint16_t RodentLampArrayController::GetLampCount()
{
    return this->lamp_array.LampCount;
}

void RodentLampArrayController::LampMultiUpdate(std::vector<color_type> &update)
{
    // Initial data alloc
    size_t size =
        // Header
        sizeof(LampMultiUpdateReport_Begin) +
        // LampId fragment for each lamp
        (sizeof(LampMultiUpdateReport_LampId) * update.size()) +
        // UpdateChannels fragment for each lamp
        (sizeof(LampMultiUpdateReport_UpdateChannels) * update.size());

    size_t offset = 0U;
    uint8_t *data = new uint8_t[size];

    // 1. Initial Header
    const LampMultiUpdateReport_Begin begin = {
        .LampCount = update.size(),
        .LampUpdateFlags = 0x01,
    };

    // Copy data
    memcpy(data, (const void *)&begin, sizeof(begin));
    offset += sizeof(begin);

    // 2. LampId data
    // For each of the LEDs
    for (auto &updt : update)
    {
        // Set report data
        const LampMultiUpdateReport_LampId id = {.LampId = updt.idx};
        // Copy data
        memcpy(data + offset, (const void *)&id, sizeof(id));
        offset += sizeof(id);
    }

    // 3. Update data
    // For each of the LEDs
    for (auto &updt : update)
    {
        // Set report data
        const LampMultiUpdateReport_UpdateChannels channels = {
            .RedUpdateChannel = updt.r,
            .GreenUpdateChannel = updt.g,
            .BlueUpdateChannel = updt.b,
            .IntensityUpdateChannel = 0xFF,
        };
        // Copy data
        memcpy(data + offset, (const void *)&channels, sizeof(channels));
        offset += sizeof(channels);
    }

    // Send data
    if (hid_send_feature_report(
            this->devices.multi_update_report,
            (const unsigned char *)data,
            size) < 0)
    {
        std::wstring error_str{hid_error(this->devices.multi_update_report)};
        throw std::runtime_error{StringUtils::wstring_to_string(error_str)};
    }

    // Deallocate data
    delete[] data;
    data = nullptr;
}

void RodentLampArrayController::LampSingleUpdate(color_type &color)
{
    // Initial data alloc
    size_t size =
        // Header
        sizeof(LampMultiUpdateReport_Begin) +
        // LampId
        sizeof(LampMultiUpdateReport_LampId) +
        // UpdateChannels
        sizeof(LampMultiUpdateReport_UpdateChannels);

    size_t offset = 0U;
    uint8_t *data = new uint8_t[size];

    // 1. Initial Header
    const LampMultiUpdateReport_Begin begin = {
        .LampCount = 1U,
        .LampUpdateFlags = 0x01,
    };

    // Copy data
    memcpy(data, (const void *)&begin, sizeof(begin));
    offset += sizeof(begin);

    // 2. LampID
    const LampMultiUpdateReport_LampId lampid = {.LampId = color.idx};
    memcpy(data + offset, (const void *)&lampid, sizeof(lampid));
    offset += sizeof(lampid);

    // 3. UpdateChannels
    const LampMultiUpdateReport_UpdateChannels channels = {
        .RedUpdateChannel = color.r,
        .GreenUpdateChannel = color.g,
        .BlueUpdateChannel = color.b,
        .IntensityUpdateChannel = 0xFF,
    };

    // Copy data
    memcpy(data + offset, (const void *)&channels, sizeof(channels));
    offset += sizeof(channels);

    // Deallocate data
    delete[] data;
    data = nullptr;
}