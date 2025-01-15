#pragma once

#include <string>

struct FBStaticTopoMeta;

void 
FBLoggerTerminate();
void
FBLoggerInit(FBStaticTopoMeta const& meta);
void
FBLoggerWrite(
  std::string const& file, std::string const& line, 
  std::string const& func, std::string const& message);