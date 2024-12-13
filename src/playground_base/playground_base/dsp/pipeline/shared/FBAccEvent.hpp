#pragma once

enum class FBAccEventType 
{
  Automation,
  Modulation
};

struct FBAccEvent
{
  int pos = -1;
  int index = -1;
  int voice = -2;
  float value = -2;
  FBAccEventType type = static_cast<FBAccEventType>(-1);
};