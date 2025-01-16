#pragma once

#include <playground_base/base/shared/FBStringify.hpp>
#include <playground_base/base/topo/FBStaticTopoMeta.hpp>

#define FF_PLUG_VERSION FB_STRINGIFY(FF_PLUG_VERSION_MAJOR.FF_PLUG_VERSION_MINOR.FF_PLUG_VERSION_PATCH)

FBStaticTopoMeta
FFPlugMeta(); 

inline constexpr char FFVendorName[] = "Sjoerd van Kreel";
inline constexpr char FFVendorMail[] = "sjoerdvankreel@gmail.com";
inline constexpr char FFVendorURL[] = "https://github.com/sjoerdvankreel";

inline constexpr char FFPlugName[] = "Playground Plug";
inline constexpr char FFPlugUniqueId[] = "BC02809BCDA540EFA521502D9BB47E1F";
inline constexpr char FFPlugProcessorId[] = "754068B351A04DB4813B58D562BDFC1F";
inline constexpr char FFPlugControllerId[] = "959E6302402B461A8C9AA5A6737BCAAD";
