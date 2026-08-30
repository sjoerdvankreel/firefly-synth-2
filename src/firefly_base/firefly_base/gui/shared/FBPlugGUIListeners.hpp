#pragma once

class IFBGUIResetListener
{
public:
  virtual ~IFBGUIResetListener() {}
  virtual void OnResetRequest() = 0;
};

class IFBThemeListener
{
public:
  virtual ~IFBThemeListener() {}
  virtual void ThemeChanged() = 0;
};

class IFBParamListener
{
public:
  virtual ~IFBParamListener() {}
  virtual void AudioParamChanged(int index, double normalized, bool changedFromUI) = 0;
};