#pragma once

#include "RodentLampArrayController.h"
#include "RGBController.h"
#include <string>

#define RODENT_LAMPARRAY_CONTROLLER_VERSION "v1.0.0"

class RGBController_RodentLampArray : public RGBController
{
public:
    RGBController_RodentLampArray() = delete;
    RGBController_RodentLampArray(RodentLampArrayController *controller);

    void SetupZones();
    void ResizeZone(int zone, int new_size);
    void DeviceUpdateLEDs();
    void UpdateZoneLEDs(int zone);
    void UpdateSingleLED(int led);
    void DeviceUpdateMode();

private:
    RodentLampArrayController *controller;
};