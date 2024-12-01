#pragma once

#include <playground_base/base/shared/FBStringify.hpp>

#define FF_OSCI_COUNT 2 // todo oscis - it must be a single module
#define FF_SHAPER_COUNT 2
#define FF_OSCI_GAIN_COUNT 2

#define FF_VENDOR_NAME "Sjoerd van Kreel"
#define FF_VENDOR_MAIL "sjoerdvankreel@gmail.com"
#define FF_VENDOR_URL "https://github.com/sjoerdvankreel"

#define FF_PLUG_NAME "Playground Plug"
#define FF_PLUG_PROCESSOR_ID "754068B351A04DB4813B58D562BDFC1F"
#define FF_PLUG_CONTROLLER_ID "959E6302402B461A8C9AA5A6737BCAAD"
#define FF_PLUG_VERSION FB_STRINGIFY(FF_PLUG_VERSION_MAJOR.FF_PLUG_VERSION_MINOR.FF_PLUG_VERSION_PATCH)