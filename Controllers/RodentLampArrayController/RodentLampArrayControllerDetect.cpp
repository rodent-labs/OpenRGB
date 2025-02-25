#include "RodentLampArrayController.h"
#include "RGBController_RodentLampArray.h"

#include "Detector.h"
#include "ResourceManager.h"
#include "LogManager.h"

#include <hidapi/hidapi.h>

/**
 * @brief Rodent. USB Vendor ID
 */
#define RODENT_VID 0x524f

/**
 * @brief Detect Rodent HID devices
 */
void DetectRodentLampArrayControllers(hid_device_info *info, const std::string &name)
{
    // Create HID device references
    static lamp_array_devices devices;

    // Open the device via HID
    hid_device *device = hid_open_path(info->path);

    // If device successfully opened
    if (!device)
        return;

    switch ((LampArrayReportUsage)info->usage)
    {
    case LampArrayReportUsage::LAMPARRAY_ATTRIBUTES_REPORT:
        devices.attributes_report = device;
        break;

    case LampArrayReportUsage::LAMPARRAY_ATTRIBUTES_REQUEST_REPORT:
        devices.request_report = device;
        break;

    case LampArrayReportUsage::LAMPARRAY_ATTRIBUTES_RESPONSE_REPORT:
        devices.response_report = device;
        break;

    case LampArrayReportUsage::LAMPARRAY_CONTROL_REPORT:
        devices.control_report = device;
        break;

    case LampArrayReportUsage::LAMPARRAY_MULTIUPDATE_REPORT:
        devices.multi_update_report = device;
        break;

    case LampArrayReportUsage::LAMPARRAY_RANGE_UPDATE_REPORT:
        devices.range_update_report = device;
        break;

    case LampArrayReportUsage::LAMPARRAY_USAGE:
        devices.lamp_array = device;
        break;
    }

    // If all references are filled
    if (devices.control_report &&
        devices.attributes_report &&
        devices.lamp_array &&
        devices.multi_update_report &&
        devices.range_update_report &&
        devices.request_report &&
        devices.response_report)
    {
        try
        {
            // Create the controller
            RodentLampArrayController *controller =
                new RodentLampArrayController(devices, info);

            // Create the RGB controller
            RGBController_RodentLampArray *rgb_controller =
                new RGBController_RodentLampArray(controller);

            // Register the class
            ResourceManager::get()->RegisterRGBController(
                dynamic_cast<RGBController *>(rgb_controller));
        }
        catch (std::runtime_error &e)
        {
            LOG_ERROR("Failed to register '%s' device. What: %s",
                      name.data(), e.what());
        }
    }
}

// Register the device
REGISTER_HID_DETECTOR("Rodent. LampArray Driver",
                      DetectRodentLampArrayControllers,
                      RODENT_VID, 0x4C41);