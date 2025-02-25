#include "RGBController_RodentLampArray.h"

#include <string>
#include <sstream>
#include <vector>

RGBController_RodentLampArray::RGBController_RodentLampArray(
    RodentLampArrayController *controller) : controller(controller)
{
    // Set properties
    name = controller->GetName();
    vendor = controller->GetVendor();
    description = "Custom WS2812B Driver powered by Raspberry Pi Pico";
    version = RODENT_LAMPARRAY_CONTROLLER_VERSION;
    serial = controller->GetSerial();
    location = controller->GetPath();
    type = DEVICE_TYPE_CASE;

    // Direct mode configuration
    mode mode_direct;
    mode_direct.name = "Direct";
    mode_direct.value = 0;
    mode_direct.flags = MODE_FLAG_HAS_PER_LED_COLOR;
    mode_direct.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(mode_direct);

    // Setup zones
    SetupZones();
}

void RGBController_RodentLampArray::SetupZones()
{
    // Create the new zone
    zone zone_default;

    // Set zone properties
    zone_default.name = "Default Zone";
    zone_default.type = ZONE_TYPE_LINEAR;
    zone_default.leds_count = controller->GetLampCount();
    zone_default.leds_min = 0U;
    zone_default.leds_max = (zone_default.leds_count - 1);

    // Add the zone
    zones.push_back(zone_default);

    // Add every led
    for (size_t idx = 0U; idx < controller->GetLampCount(); idx++)
    {
        // Create the LED properties
        led led_prop;

        // Generate name
        std::stringstream name_str;
        name_str << "Lamp " << idx;

        // Set properties
        led_prop.name = name_str.str();
        led_prop.value = idx;

        // Add the LED
        leds.push_back(led_prop);
    }

    // Add colors
    SetupColors();
}

void RGBController_RodentLampArray::ResizeZone(
    [[maybe_unused]] int zone,
    [[maybe_unused]] int new_size) {}

void RGBController_RodentLampArray::DeviceUpdateLEDs()
{
    // Create the update array
    std::vector<color_type> update{leds.size()};

    // Get each of the LEDs and update their alues
    for (size_t idx = 0; idx < leds.size(); idx++)
    {
        update[idx].idx = (uint16_t)idx;
        update[idx].r = RGBGetRValue(colors[idx]);
        update[idx].g = RGBGetGValue(colors[idx]);
        update[idx].b = RGBGetBValue(colors[idx]);
    }

    // Update the controller data
    controller->LampMultiUpdate(update);
}

void RGBController_RodentLampArray::UpdateZoneLEDs(int zone)
{
    // Create the update array
    std::vector<color_type> update;

    // Get each of the LEDs and update their alues
    for (size_t idx = zones[zone].leds_min; idx <= zones[zone].leds_max; idx++)
    {
        // Update data format
        color_type updt_data = {
            .idx = (uint16_t)idx,
            .r = (uint8_t)(colors[idx]),
            .g = (uint8_t)RGBGetGValue(colors[idx]),
            .b = (uint8_t)RGBGetBValue(colors[idx]),
        };

        // Add update data
        update.push_back(updt_data);
    }

    // Update the controller data
    controller->LampMultiUpdate(update);
}

void RGBController_RodentLampArray::UpdateSingleLED(int led)
{
    // Get color data
    color_type color = {
        .idx = (uint16_t)led,
        .r = (uint8_t)RGBGetRValue(colors[led]),
        .g = (uint8_t)RGBGetGValue(colors[led]),
        .b = (uint8_t)RGBGetBValue(colors[led]),
    };

    // Update single LED
    controller->LampSingleUpdate(color);
}

void RGBController_RodentLampArray::DeviceUpdateMode() {}
