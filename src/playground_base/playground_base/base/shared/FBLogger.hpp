#pragma once

#include <string>

struct FBStaticTopoMeta;

void 
FBLoggerTerminate();
void
FBLoggerInit(FBStaticTopoMeta const& meta);
void
FBLoggerWrite(
  char const* file, int line, 
  char const* func, std::string const& message);