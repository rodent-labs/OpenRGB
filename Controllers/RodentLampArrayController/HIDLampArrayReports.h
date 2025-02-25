/**
 * @file HIDLampArrayReports.h
 * @author Angel Talero (angelgotalero@outlook.com)
 * @brief HID LampArray report descriptors.
 *
 * Most of these structures were gathered from this repo:
 * https://github.com/taleroangel/zephyr-lamparray/blob/main/src/hid/private/reports.h
 *
 * @date 2025-02-18
 */

#pragma once

#include <stdint.h>

/*---------------------------------------------------------*\
| Struct packing macro for GCC and MSVC                     |
\*---------------------------------------------------------*/
#ifdef __GNUC__
#define PACK(__Declaration__) __Declaration__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK(__Declaration__) __pragma(pack(push, 1)) __Declaration__ __pragma(pack(pop))
#endif

/**
 * @brief Report usage ids for each of the reports
 */
enum class LampArrayReportUsage : uint8_t
{
    LAMPARRAY_USAGE = 0x01,
    LAMPARRAY_ATTRIBUTES_REPORT = 0x02,
    LAMPARRAY_ATTRIBUTES_REQUEST_REPORT = 0x20,
    LAMPARRAY_ATTRIBUTES_RESPONSE_REPORT = 0x22,
    LAMPARRAY_MULTIUPDATE_REPORT = 0x50,
    LAMPARRAY_RANGE_UPDATE_REPORT = 0x60,
    LAMPARRAY_CONTROL_REPORT = 0x70,
};

/**
 * @brief All of the 6 report types ordered
 */
enum class LampArrayReportType: uint8_t
{
    LAMPARRAY_ATTRIBUTES_REPORT = 1,
    LAMPARRAY_ATTRIBUTES_REQUEST_REPORT = 2,
    LAMPARRAY_ATTRIBUTES_RESPONSE_REPORT = 3,
    LAMPARRAY_MULTIUPDATE_REPORT = 4,
    LAMPARRAY_RANGE_UPDATE_REPORT = 5,
    LAMPARRAY_CONTROL_REPORT = 6,
};

/**
 * @brief Type of LampArray device in LampArrayAttributesReport
 *
 */
enum LampArrayKind : uint8_t
{
    LAMPARRAY_KIND_KEYBOARD = 0x01,
    LAMPARRAY_KIND_MOUSE = 0x02,
    LAMPARRAY_KIND_GAME_CONTROLLER = 0x03,
    LAMPARRAY_KIND_PERIPHERAL = 0x04,
    LAMPARRAY_KIND_SCENE = 0x05,
    LAMPARRAY_KIND_NOTIFICATION = 0x06,
    LAMPARRAY_KIND_CHASSIS = 0x07,
    LAMPARRAY_KIND_WEARABLE = 0x08,
    LAMPARRAY_KIND_FURNITURE = 0x09,
    LAMPARRAY_KIND_ART = 0x0A,
};

/**
 * @brief Specify the purpose of a single Lamp in LampAttributesResponseReport
 * @note Flags are permitted to be combined.
 */
enum LampArrayPurposes
{
    LAMPARRAY_PURPOSE_CONTROL = 0x01,
    LAMPARRAY_PURPOSE_ACCENT = 0x02,
    LAMPARRAY_PURPOSE_BRANDING = 0x04,
    LAMPARRAY_PURPOSE_STATUS = 0x08,
    LAMPARRAY_PURPOSE_ILLUMINATION = 0x10,
    LAMPARRAY_PURPOSE_PRESENTATION = 0x20,
};

/**
 * @brief Report #1
 */
PACK(struct LampArrayAttributesReport {
    LampArrayReportType ReportId = LampArrayReportType::LAMPARRAY_ATTRIBUTES_REPORT;
    uint16_t LampCount;
    uint32_t BoundingBoxWidthInMicrometers;
    uint32_t BoundingBoxHeightInMicrometers;
    uint32_t BoundingBoxDepthInMicrometers;
    uint32_t LampArrayKind;
    uint32_t MinUpdateIntervalInMicroseconds;
});

/**
 * @brief Report #2
 */
PACK(struct LampAttributesRequestReport {
    LampArrayReportType ReportId = LampArrayReportType::LAMPARRAY_ATTRIBUTES_REQUEST_REPORT;
    uint16_t LampId;
});

/**
 * @brief Report #3
 */
PACK(struct LampAttributesResponseReport {
    LampArrayReportType ReportId = LampArrayReportType::LAMPARRAY_ATTRIBUTES_RESPONSE_REPORT;
    uint16_t LampId;
    uint32_t PositionXInMicrometers;
    uint32_t PositionYInMicrometers;
    uint32_t PositionZInMicrometers;
    uint32_t LampPurposes;
    uint32_t UpdateLatencyInMicroseconds;
    uint8_t RedLevelCount;
    uint8_t GreenLevelCount;
    uint8_t BlueLevelCount;
    uint8_t IntensityLevelCount;
    uint8_t IsProgrammable;
    uint8_t InputBinding;
});

/**
 * @brief Report #4 (First Half)
 * @note Report #4 is tricky because it comes in parts, make sure to join it in a proper way
 */
PACK(struct LampMultiUpdateReport_Begin {
    LampArrayReportType ReportId = LampArrayReportType::LAMPARRAY_MULTIUPDATE_REPORT;
    uint64_t LampCount;
    uint8_t LampUpdateFlags;
});

/**
 * @brief Comes after LampMultiUpdateReport_Begin, repeats itself
 * for every lamp in LampCount
 */
PACK(struct LampMultiUpdateReport_LampId {
    uint16_t LampId;
});

/**
 * @brief Comes after all the LampMultiUpdateReport_LampId elements,
 * repeats itself for every lamp in LampCount
 */
PACK(struct LampMultiUpdateReport_UpdateChannels {
    uint8_t RedUpdateChannel;
    uint8_t GreenUpdateChannel;
    uint8_t BlueUpdateChannel;
    uint8_t IntensityUpdateChannel;
});

/**
 * @brief Report #5
 */
PACK(struct LampRangeUpdateRecord {
    LampArrayReportType ReportId = LampArrayReportType::LAMPARRAY_RANGE_UPDATE_REPORT;
    uint8_t LampUpdateFlags;
    uint16_t LampIdStart;
    uint16_t LampIdEnd;
    uint8_t RedUpdateChannel;
    uint8_t GreenUpdateChannel;
    uint8_t BlueUpdateChannel;
    uint8_t IntensityUpdateChannel;
});

/**
 * @brief HID report #6
 */
PACK(struct LampArrayControlReport {
    LampArrayReportType ReportId = LampArrayReportType::LAMPARRAY_CONTROL_REPORT;
    uint8_t AutonomousMode : 1;
});