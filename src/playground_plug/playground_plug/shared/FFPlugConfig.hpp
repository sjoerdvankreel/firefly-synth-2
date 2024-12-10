#pragma once

#include <playground_base/base/shared/FBStringify.hpp>

#define FF_PLUG_VERSION FB_STRINGIFY(FF_PLUG_VERSION_MAJOR.FF_PLUG_VERSION_MINOR.FF_PLUG_VERSION_PATCH)

inline int constexpr FFGLFOCount = 1;
inline int constexpr FFOsciCount = 2; // todo oscis - it must be a single module
inline int constexpr FFShaperCount = 1;
inline int constexpr FFOsciGainCount = 2;

inline constexpr char FFVendorName[] = "Sjoerd van Kreel";
inline constexpr char FFVendorMail[] = "sjoerdvankreel@gmail.com";
inline constexpr char FFVendorURL[] = "https://github.com/sjoerdvankreel";

inline constexpr char FFPlugName[] = "Playground Plug";
inline constexpr char FFPlugProcessorId[] = "754068B351A04DB4813B58D562BDFC1F";
inline constexpr char FFPlugControllerId[] = "959E6302402B461A8C9AA5A6737BCAAD";