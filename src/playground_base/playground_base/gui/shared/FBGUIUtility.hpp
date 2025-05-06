#pragma once

#include <string>

void FBGUIInit();
void FBGUITerminate();

// TODO account for different fonts/control types
int 
FBGetStringWidthCached(std::string const& text);