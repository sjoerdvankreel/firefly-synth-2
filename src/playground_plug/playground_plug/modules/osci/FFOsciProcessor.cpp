#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/osci/FFOsciProcessor.hpp>

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugInputBlock.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

#include <xsimd/xsimd.hpp>

// blep waves
// https://www.taletn.com/reaper/mono_synth/
// https://github.com/martinfinke/PolyBLEP/blob/master/PolyBLEP.cpp
// http://www.acoustics.hut.fi/publications/papers/smc2010-phaseshaping/
// dsf https://www.verklagekasper.de/synths/dsfsynthesis/dsfsynthesis.html

static inline float constexpr MinPW = 0.05f;
static inline float const Exp2 = std::exp(2.0f);
static inline float const SqrtPi = std::sqrt(FBPi);

using namespace juce::dsp;

static inline int
OsciModStartSlot(int osciSlot)
{
  switch (osciSlot)
  {
  case 0: return -1;
  case 1: return 0;
  case 2: return 1;
  case 3: return 3;
  default: assert(false); return -1;
  }
}

static inline float
FMRatioRatio(int v)
{
  assert(0 <= v && v < FFOsciFMRatioCount * FFOsciFMRatioCount);
  return ((v / FFOsciFMRatioCount) + 1.0f) / ((v % FFOsciFMRatioCount) + 1.0f);
}

static inline float
BLEP(float t, float dt)
{
  if (t < dt)
  {
    float b = t / dt - 1.0f;
    return -(b * b);
  } else if (t > 1.0f - dt)
  {
    float b = (t - 1.0f) / dt + 1.0f;
    return b * b;
  }
  else
    return 0.0f;
}

static inline float
BLAMP(float t, float dt)
{
  if (t < dt)
  {
    float b = t / dt - 1.0f;
    return -1.0f / 3.0f * b * b * b;
  } else if (t > 1.0f - dt)
  {
    float b = (t - 1.0f) / dt + 1.0f;
    return 1.0f / 3.0f * b * b * b;
  } else
    return 0.0f;
}

static inline float 
BLUH(float t, float dt)
{
  if (t < dt)
  {
   t = t / dt - 1.0f;
   t *= t;
   return (t * t - t) * -4.0f;
  }
  else if (t > 1.0f - dt)
  {
    t = (t - 1.0f) / dt + 1.0f;
    t *= t;
    return (t * t - t) * 4.0f;
  }
  else
    return 0.0f;
}

static inline FBSIMDVector<float>
WaveBasicSin(
  FBSIMDVector<float> t)
{
  return xsimd::sin(t * FBTwoPi);
}

static inline FBSIMDVector<float>
WaveBasicCos(
  FBSIMDVector<float> t)
{
  return xsimd::cos(t * FBTwoPi);
}

static inline FBSIMDVector<float>
WaveBasicSaw(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float t1 = FBPhaseWrap(t + 0.5f);
    float y = 2.0f * t1 - 1.0f;
    y -= BLEP(t1, dt);
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WaveBasicRamp(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float y = 1.0f - 2.0f * t;
    y += BLEP(t, dt);
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WaveBasicRect(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float t2 = FBPhaseWrap(t + 0.5f);
    float y = t < 0.5f ? 1.0f : -1.0f;
    y += BLEP(t, dt) - BLEP(t2, dt);
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WaveBasicTri(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float t0 = t;
    float t1 = FBPhaseWrap(t + 0.25f);
    float t2 = FBPhaseWrap(t + 0.75f);
    float y = t * 4.0f;
    if (y >= 3.0f)
      y -= 4.0f;
    else if (y > 1.0f)
      y = 2.0f - y;
    y += 4.0f * dt * (BLAMP(t1, dt) - BLAMP(t2, dt));
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WaveBasicTrap(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float y = 4.0f * t;
    if (y >= 3.0f)
      y -= 4.0f;
    else if (y > 1.0f)
      y = 2.0f - y;
    y = std::clamp(2.0f * y, -1.0f, 1.0f);
    float t1 = FBPhaseWrap(t + 0.125f);
    float t2 = FBPhaseWrap(t1 + 0.5f);
    y += 4.0f * dt * (BLAMP(t1, dt) - BLAMP(t2, dt));
    t1 = FBPhaseWrap(t + 0.375f);
    t2 = FBPhaseWrap(t1 + 0.5f);
    y += 4.0f * dt * (BLAMP(t1, dt) - BLAMP(t2, dt));
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WaveBasicSinSqr(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float y = -BLEP(FBPhaseWrap(t + 0.5f), dt) + BLAMP(t, dt) * FBPi * dt;
    if (t < 0.5f)
      y += std::sin(t * FBPi) * 2.0f;
    y -= 2.0f / FBPi;
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WaveBasicSawSqr(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    t = FBPhaseWrap(t + 0.5f);
    float y = -BLEP(t, dt);
    if (t >= 0.5f)
      y += std::cos(t * FBTwoPi) + 1.0f;
    y -= 0.5f;
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WaveBasicSinSaw(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float y = BLEP(t, dt) + std::sin((0.5f - t) * FBPi);
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WaveBasicSawM1(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float t2 = FBPhaseWrap(t + 0.5f);
    float y = (std::sin(t * FBTwoPi) * (dt * dt * 3.28765f /* what? */ / FBPi - 1.0f / FBPi) + t2) * 2.0f - 1.0f;
    y -= BLEP(t2, dt);
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WaveBasicSqrM1(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float t2 = FBPhaseWrap(t + 0.5f);
    float y = (std::sin(t * FBTwoPi) * (dt * dt * 6.5753f /* what? */ / FBPi - 2.0f / FBPi));
    if (t < 0.5f)
      y += 1.0f;
    y = (y * 2.0f - 1.0f) + BLEP(t, dt) - BLEP(t2, dt);
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WaveBasicBSSin(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float y = (BLUH(FBPhaseWrap(t + 0.5f), dt) - BLUH(t, dt)) * dt * dt * -SqrtPi;
    float stp = std::sin(t * FBTwoPi);
    if (t < 0.5f)
      y += stp * stp;
    else
      y -= stp * stp;
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WaveBasicHWSin(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float y = (BLAMP(t, dt) + BLAMP(FBPhaseWrap(t + 0.5f), dt)) * FBTwoPi * dt;
    if (t < 0.5f)
      y += std::sin(t * FBTwoPi) * 2.0f;
    y -= 2.0f / FBPi;
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WaveBasicFWSin(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    t = FBPhaseWrap(t + 0.25f);
    float y = BLAMP(t, dt) * FBTwoPi * dt + std::sin(t * FBPi) * 2.0f - 4.0f / FBPi;
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WaveBasicAltSin(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float y = (BLAMP(FBPhaseWrap(t + 0.5f), dt) - BLAMP(t, dt)) * -FBTwoPi * dt;
    if (t < 0.5f)
      y += std::sin(t * 4.0f * FBPi);
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WaveBasicParabl(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float y = (BLUH(FBPhaseWrap(t + 0.5f), dt) - BLUH(t, dt)) * 2.0f / 3.0f * dt * dt;
    y += (t * 16.0f - 8.0f) * (std::abs(t - 0.5f) - 0.5f);
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WaveBasicHypTri(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float t1 = t;
    float t2 = FBPhaseWrap(t1 + 0.5f);
    float y = (BLAMP(t1, dt) * 4.0f / FBPi - BLAMP(t2, dt) * 9.273f /* what? */) * dt;
    y += std::exp((t1 < 0.5f ? t1 : 1.0f - t1) * 4.0f) * (2.0f / (Exp2 - 1.0f)) - 1.0f;
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WavePWRect(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec,
  FBSIMDVector<float> pwVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  FBSIMDArray<float, FBSIMDFloatCount> pwArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  pwArr.Store(0, pwVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float y;
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float pw = pwArr.Get(i);
    float t1 = t;
    float t2 = FBPhaseWrap(t1 - pw + 1.0f);
    if (t1 < pw)
      y = (1.0f - pw) * 2.0f;
    else
      y = -pw * 2.0f;
    if (t1 < dt)
    {
      float x = t1 / dt - 1;
      y -= x * x;
    }
    else if (t1 > 1.0f - dt)
    {
      float x = (t1 - 1.0f) / dt + 1.0f;
      y += x * x;
    }
    if (t2 < dt)
    {
      float x = t2 / dt - 1.0f;
      y += x * x;
    }
    else if (t2 > 1.0f - dt)
    {
      float x = (t2 - 1.0f) / dt + 1.0f;
      y -= x * x;
    }
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WavePWSqr(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec,
  FBSIMDVector<float> pwVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  FBSIMDArray<float, FBSIMDFloatCount> pwArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  pwArr.Store(0, pwVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float pw = pwArr.Get(i);
    float t1 = FBPhaseWrap(t + 0.875f + 0.25f * (pw - 0.5f));
    float t2 = FBPhaseWrap(t + 0.375f + 0.25f * (pw - 0.5f));
    float y = t1 < 0.5f ? 1.0f : -1.0f;
    y += BLEP(t1, dt) - BLEP(t2, dt);
    t1 = FBPhaseWrap(t1 + 0.5f * (1.0f - pw));
    t2 = FBPhaseWrap(t2 + 0.5f * (1.0f - pw));
    y += t1 < 0.5f ? 1.0f : -1.0f;
    y += BLEP(t1, dt) - BLEP(t2, dt);
    yArr.Set(i, y * 0.5f);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WavePWHWSaw(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec,
  FBSIMDVector<float> pwVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  FBSIMDArray<float, FBSIMDFloatCount> pwArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  pwArr.Store(0, pwVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float pw = pwArr.Get(i);
    float t2 = FBPhaseWrap(t + 0.5f);
    float t1 = FBPhaseWrap(t2 + pw);
    float y = 0.0f;
    if (t1 < pw)
      y = t1 * 2.0f / pw;
    y -= pw;
    if (t1 < dt)
    {
      float p = t1 / dt - 1.0f;
      y -= p * p * p / (pw * 3.0f) * dt;
    }
    else if (t1 > 1.0f - dt)
    {
      float p = (t1 - 1.0f) / dt + 1.0f;
      y += p * p * p / (pw * 3.0f) * dt;
    }
    if (t2 < dt)
    {
      float p = t2 / dt - 1.0f;
      y += ((p * p) / (pw * 3.0f) * dt + p) * p;
    }
    else if (t2 > 1.0f - dt)
    {
      float p = (t2 - 1.0f) / dt + 1.0f;
      y -= ((p * p) / (pw * 3.0f) * dt + p) * p;
    }
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WavePWTriSaw(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec,
  FBSIMDVector<float> pwVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  FBSIMDArray<float, FBSIMDFloatCount> pwArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  pwArr.Store(0, pwVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float pw = pwArr.Get(i);
    float t1 = FBPhaseWrap(t + 0.5f * pw);
    float t2 = FBPhaseWrap(t + 1.0f - 0.5f * pw);
    float y = t * 2.0f;
    if (y >= 2.0f - pw)
      y = (y - 2.0f) / pw;
    else if (y >= pw)
      y = 1.0f - (y - pw) / (1.0f - pw);
    else
      y /= pw;
    y += dt / (pw - pw * pw) * (BLAMP(t1, dt) - BLAMP(t2, dt));
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WavePWTriPls(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec,
  FBSIMDVector<float> pwVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  FBSIMDArray<float, FBSIMDFloatCount> pwArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  pwArr.Store(0, pwVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float y;
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float pw = pwArr.Get(i);
    float t1 = FBPhaseWrap(t + 0.75f + 0.5f * pw);
    if (t1 >= pw)
      y = -pw;
    else 
    {
      y = 4.0f * t1;
      y = (y >= 2.0f * pw ? 4.0f - y / pw - pw : y / pw - pw);
    }
    float t2 = FBPhaseWrap(t1 + 1.0f - 0.5f * pw);
    float t3 = FBPhaseWrap(t1 + 1.0f - pw);
    y += 2.0f * dt / pw * (BLAMP(t1, dt) - 2.0f * BLAMP(t2, dt) + BLAMP(t3, dt));
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WavePWTrapTri(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec,
  FBSIMDVector<float> pwVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  FBSIMDArray<float, FBSIMDFloatCount> pwArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  pwArr.Store(0, pwVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float pw = pwArr.Get(i);
    float scale = 1.0f / (1.0f - pw);
    float y = 4.0f * t;
    if (y >= 3.0f)
      y -= 4.0f;
    else if (y > 1.0f)
      y = 2.0f - y;
    y = std::clamp(scale * y, -1.0f, 1.0f);
    float t1 = FBPhaseWrap(t + 0.25f - 0.25f * pw);
    float t2 = FBPhaseWrap(t1 + 0.5f);
    y += scale * 2.0f * dt * (BLAMP(t1, dt) - BLAMP(t2, dt));
    t1 = FBPhaseWrap(t + 0.25f + 0.25f * pw);
    t2 = FBPhaseWrap(t1 + 0.5f);
    y += scale * 2.0f * dt * (BLAMP(t1, dt) - BLAMP(t2, dt));
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WaveHSSaw(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec,
  FBSIMDVector<float> freqRatioVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  FBSIMDArray<float, FBSIMDFloatCount> frArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  frArr.Store(0, freqRatioVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float fr = frArr.Get(i);
    
    float pw;
    int n = static_cast<int>(std::max(std::ceil(fr) - 1.0f, 0.0f));
    float scale = fr - n;
    if (fr > 0.0f)
      pw = scale / fr;
    else
      pw = 1.0f;

    t = FBPhaseWrap(t + 0.5f);
    float y = t * fr;
    y = FBPhaseWrap(y) * 2.0f - pw * pw * fr + pw - 1.0f;
    if (t < dt)
    {
      float a = t / dt - 1.0f;
      y += a * a * scale;
    }
    else if (t > 1.0f - dt)
    {
      float a = (t - 1.0f) / dt + 1.0f;
      y -= a * a * scale;
    }

    if (n > 0)
      pw = (pw - 1.0f) / n + 1;

    for (int i = 0; i < n; i++)
    {
      t = FBPhaseWrap(t + pw);
      if (t < dt)
      {
        float a = t / dt - 1.0f;
        y += a * a;
      }
      else if (t > 1.0f - dt)
      {
        float a = (t - 1.0f) / dt + 1.0f;
        y -= a * a;
      }
    }

    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WaveHSSqr(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec,
  FBSIMDVector<float> freqRatioVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  FBSIMDArray<float, FBSIMDFloatCount> frArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  frArr.Store(0, freqRatioVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float fr = frArr.Get(i);
    
    float y;
    t = FBPhaseWrap(t + 0.5f);
    float x = t * fr;
    if (x - FBFastFloor(x) < 0.5f)
      y = -2.0f;
    else
      y = 0.0f;
    x = static_cast<float>(FBFastFloor(fr));
    if (x + 0.5f > fr)
      y += 2.0f - x / std::max(fr, 0.5f);
    else
      y += (x + 1.0f) / fr;
    
    float scale;
    float pw = 1.0f - 0.5f / fr;
    int n = static_cast<int>(std::ceil(fr - 0.5f));
    if (x < n)
      scale = -1.0f;
    else
    {
      t += pw;
      scale = 1.0f;
    }

    for (int i = 0; i < 2 * n; i++)
    {
      t = FBPhaseWrap(t);
      y += BLEP(t, dt) * scale;
      t += pw;
      scale = -scale;
    }
    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
WaveHSTri(
  FBSIMDVector<float> tVec,
  FBSIMDVector<float> dtVec,
  FBSIMDVector<float> freqRatioVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> tArr;
  FBSIMDArray<float, FBSIMDFloatCount> yArr;
  FBSIMDArray<float, FBSIMDFloatCount> dtArr;
  FBSIMDArray<float, FBSIMDFloatCount> frArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  frArr.Store(0, freqRatioVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
    float fr = frArr.Get(i);

    t = FBPhaseWrap(t + 0.5f);
    float x = t * fr + 0.75f;
    float y = std::abs((FBFastFloor(x + 0.75f) - x) * 4.0f - 1.0f) - 1.0f;
    float dc = -1.0f / fr;
    float pw = dc * 0.5f + 1.0f;
    x = FBPhaseWrap(fr);
    float scale = std::abs(x * 2.0f - 1.0f) - 1.0f;

    if (x > 0.5f)
      x -= 0.5f;
    else
      dc = -dc;
    y -= (x * x * 2 - x) * dc;
    y += BLEP(t, dt) * scale;

    int n = static_cast<int>(std::ceil(fr * 2.0f));
    scale = fr * 4.0f * dt;
    if (n % 2 == 1)
    {
      n -= 1;
      t += pw;
      scale = -scale;
    }

    for (int i = 0; i < n; i++)
    {
      t = FBPhaseWrap(t);
      y += BLAMP(t, dt) * scale;
      t += pw;
      scale = -scale;
    }    

    yArr.Set(i, y);
  }
  return yArr.Load(0);
}

static inline FBSIMDVector<float>
GenerateWaveBasicCheck(
  FFOsciWaveBasicMode mode,
  FBSIMDVector<float> phaseVec,
  FBSIMDVector<float> incrVec)
{
  switch (mode)
  {
  case FFOsciWaveBasicMode::Sin: return WaveBasicSin(phaseVec);
  case FFOsciWaveBasicMode::Cos: return WaveBasicCos(phaseVec);
  case FFOsciWaveBasicMode::Saw: return WaveBasicSaw(phaseVec, incrVec);
  case FFOsciWaveBasicMode::Ramp: return WaveBasicRamp(phaseVec, incrVec);
  case FFOsciWaveBasicMode::Rect: return WaveBasicRect(phaseVec, incrVec);
  case FFOsciWaveBasicMode::Tri: return WaveBasicTri(phaseVec, incrVec);
  case FFOsciWaveBasicMode::Trap: return WaveBasicTrap(phaseVec, incrVec);
  case FFOsciWaveBasicMode::SinSqr: return WaveBasicSinSqr(phaseVec, incrVec);
  case FFOsciWaveBasicMode::SawSqr: return WaveBasicSawSqr(phaseVec, incrVec);
  case FFOsciWaveBasicMode::SinSaw: return WaveBasicSinSaw(phaseVec, incrVec);
  case FFOsciWaveBasicMode::SawM1: return WaveBasicSawM1(phaseVec, incrVec);
  case FFOsciWaveBasicMode::SqrM1: return WaveBasicSqrM1(phaseVec, incrVec);
  case FFOsciWaveBasicMode::BSSin: return WaveBasicBSSin(phaseVec, incrVec);
  case FFOsciWaveBasicMode::HWSin: return WaveBasicHWSin(phaseVec, incrVec);
  case FFOsciWaveBasicMode::FWSin: return WaveBasicFWSin(phaseVec, incrVec);
  case FFOsciWaveBasicMode::AltSin: return WaveBasicAltSin(phaseVec, incrVec);
  case FFOsciWaveBasicMode::Parabl: return WaveBasicParabl(phaseVec, incrVec);
  case FFOsciWaveBasicMode::HypTri: return WaveBasicHypTri(phaseVec, incrVec);
  default: assert(false); return 0.0f;
  }
}

static inline FBSIMDVector<float>
GenerateWavePWCheck(
  FFOsciWavePWMode mode,
  FBSIMDVector<float> phaseVec,
  FBSIMDVector<float> incrVec,
  FBSIMDVector<float> pwVec)
{
  switch (mode)
  {
  case FFOsciWavePWMode::Rect: return WavePWRect(phaseVec, incrVec, pwVec);
  case FFOsciWavePWMode::Sqr: return WavePWSqr(phaseVec, incrVec, pwVec);
  case FFOsciWavePWMode::HWSaw: return WavePWHWSaw(phaseVec, incrVec, pwVec);
  case FFOsciWavePWMode::TriSaw: return WavePWTriSaw(phaseVec, incrVec, pwVec);
  case FFOsciWavePWMode::TriPls: return WavePWTriPls(phaseVec, incrVec, pwVec);
  case FFOsciWavePWMode::TrapTri: return WavePWTrapTri(phaseVec, incrVec, pwVec);
  default: assert(false); return 0.0f;
  }
}

static inline FBSIMDVector<float>
GenerateWaveHSCheck(
  FFOsciWaveHSMode mode,
  FBSIMDVector<float> phaseVec,
  FBSIMDVector<float> incrVec,
  FBSIMDVector<float> freqRatioVec)
{
  switch (mode)
  {
  case FFOsciWaveHSMode::Saw: return WaveHSSaw(phaseVec, incrVec, freqRatioVec);
  case FFOsciWaveHSMode::Sqr: return WaveHSSqr(phaseVec, incrVec, freqRatioVec);
  case FFOsciWaveHSMode::Tri: return WaveHSTri(phaseVec, incrVec, freqRatioVec);
  default: assert(false); return 0.0f;
  }
}

static inline FBSIMDVector<float>
GenerateWaveBasic(
  FFOsciWaveBasicMode mode,
  FBSIMDVector<float> phaseVec,
  FBSIMDVector<float> incrVec)
{
  auto result = GenerateWaveBasicCheck(mode, phaseVec, incrVec);
  FBSIMDVectorNaNCheck(result);
  return result;
}

static inline FBSIMDVector<float>
GenerateWavePW(
  FFOsciWavePWMode mode,
  FBSIMDVector<float> phaseVec,
  FBSIMDVector<float> incrVec,
  FBSIMDVector<float> pwVec)
{
  auto result = GenerateWavePWCheck(mode, phaseVec, incrVec, pwVec);
  FBSIMDVectorNaNCheck(result);
  return result;
}

static inline FBSIMDVector<float>
GenerateWaveHS(
  FFOsciWaveHSMode mode,
  FBSIMDVector<float> phaseVec,
  FBSIMDVector<float> incrVec,
  FBSIMDVector<float> freqRatioVec)
{
  auto result = GenerateWaveHSCheck(mode, phaseVec, incrVec, freqRatioVec);
  FBSIMDVectorNaNCheck(result);
  return result;
}

static inline FBSIMDVector<float>
GenerateDSF(
  FBSIMDVector<float> phaseVec, FBSIMDVector<float> freqVec, 
  FBSIMDVector<float> decayVec, FBSIMDVector<float> distFreqVec, 
  FBSIMDVector<float> overtoneVec)
{
  float const decayRange = 0.99f;
  float const scaleFactor = 0.975f;

  auto n = overtoneVec;
  auto w = decayVec * decayRange;
  auto wPowNp1 = xsimd::pow(FBSIMDVector<float>(w), overtoneVec + 1.0f);
  auto u = 2.0f * FBPi * phaseVec;
  auto v = 2.0f * FBPi * distFreqVec * phaseVec / freqVec;
  auto a = w * xsimd::sin(u + n * v) - xsimd::sin(u + (n + 1.0f) * v);
  auto x = (w * xsimd::sin(v - u) + xsimd::sin(u)) + wPowNp1 * a;
  auto y = 1.0f + w * w - 2.0f * w * xsimd::cos(v);
  auto scale = (1.0f - wPowNp1) / (1.0f - w);
  return x * scaleFactor / (y * scale);
}

static inline FBSIMDVector<float>
GenerateDSFOvertones(
  FBSIMDVector<float> phaseVec, FBSIMDVector<float> freqVec, 
  FBSIMDVector<float> decayVec, FBSIMDVector<float> distFreqVec, 
  FBSIMDVector<float> maxOvertoneVec, float overtones_)
{
  auto overtoneVec = xsimd::min(FBSIMDVector<float>(overtones_), xsimd::floor(maxOvertoneVec));
  return GenerateDSF(phaseVec, freqVec, decayVec, distFreqVec, overtoneVec);
}

static inline FBSIMDVector<float>
GenerateDSFBandwidth(
  FBSIMDVector<float> phaseVec, FBSIMDVector<float> freqVec, 
  FBSIMDVector<float> decayVec, FBSIMDVector<float> distFreqVec, 
  FBSIMDVector<float> maxOvertoneVec, float bandwidth)
{
  auto overtoneVec = 1.0f + xsimd::floor(bandwidth * (maxOvertoneVec - 1.0f));
  overtoneVec = xsimd::min(overtoneVec, xsimd::floor(maxOvertoneVec));
  return GenerateDSF(phaseVec, freqVec, decayVec, distFreqVec, overtoneVec);
}

FFOsciProcessor::
FFOsciProcessor() :
_oversampler(
    FFOsciUniMaxCount, FFOsciOversamplingFactor,
    Oversampling<float>::filterHalfBandPolyphaseIIR, false, false)
{
  _oversampler.initProcessing(FBFixedBlockSamples);
  for (int u = 0; u < FFOsciUniMaxCount; u++)
    _downsampledChannelPtrs[u] = _uniOutputDownsampled[u].Ptr(0);
  _downsampledBlock = AudioBlock<float>(_downsampledChannelPtrs.data(), FFOsciUniMaxCount, 0, FBFixedBlockSamples);
  _oversampledBlock = _oversampler.processSamplesUp(_downsampledBlock);
}

void
FFOsciProcessor::BeginVoice(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];

  int modStartSlot = OsciModStartSlot(state.moduleSlot);
  auto const& modParams = procState->param.voice.osciMod[0];
  auto const& modTopo = state.topo->static_.modules[(int)FFModuleType::OsciMod];

  auto const& typeNorm = params.block.type[0].Voice()[voice];
  auto const& uniCountNorm = params.block.uniCount[0].Voice()[voice];
  auto const& uniOffsetNorm = params.block.uniOffset[0].Voice()[voice];
  auto const& uniRandomNorm = params.block.uniRandom[0].Voice()[voice];
  auto const& waveHSModeNorm = params.block.waveHSMode[0].Voice()[voice];
  auto const& waveDSFBWNorm = params.block.waveDSFBW[0].Voice()[voice];
  auto const& waveDSFModeNorm = params.block.waveDSFMode[0].Voice()[voice];
  auto const& waveDSFOverNorm = params.block.waveDSFOver[0].Voice()[voice];
  auto const& waveDSFDistanceNorm = params.block.waveDSFDistance[0].Voice()[voice];
  auto const& fmExpNorm = params.block.fmExp[0].Voice()[voice];
  auto const& fmRatioModeNorm = params.block.fmRatioMode[0].Voice()[voice];
  auto const& fmRatioRatio12Norm = params.block.fmRatioRatio[0].Voice()[voice];
  auto const& fmRatioRatio23Norm = params.block.fmRatioRatio[1].Voice()[voice];

  auto const& modExpoFMNorm = modParams.block.expoFM[0].Voice()[voice];
  auto const& modOversamplingNorm = modParams.block.oversampling[0].Voice()[voice];

  _phaseGen = {};
  _prng = FBParkMillerPRNG(state.moduleSlot / static_cast<float>(FFOsciCount));

  _key = static_cast<float>(state.voice->event.note.key);
  _type = topo.NormalizedToListFast<FFOsciType>(FFOsciParam::Type, typeNorm);
  _uniCount = topo.NormalizedToDiscreteFast(FFOsciParam::UniCount, uniCountNorm);
  _uniOffsetPlain = topo.NormalizedToIdentityFast(FFOsciParam::UniOffset, uniOffsetNorm);
  _uniRandomPlain = topo.NormalizedToIdentityFast(FFOsciParam::UniRandom, uniRandomNorm);
  _waveHSMode = topo.NormalizedToListFast<FFOsciWaveHSMode>(FFOsciParam::WaveHSMode, waveHSModeNorm);
  _waveDSFBWPlain = topo.NormalizedToLog2Fast(FFOsciParam::WaveDSFBW, waveDSFBWNorm);
  _waveDSFMode = topo.NormalizedToListFast<FFOsciWaveDSFMode>(FFOsciParam::WaveDSFMode, waveDSFModeNorm);
  _waveDSFOver = static_cast<float>(topo.NormalizedToDiscreteFast(FFOsciParam::WaveDSFOver, waveDSFOverNorm));
  _waveDSFDistance = static_cast<float>(topo.NormalizedToDiscreteFast(FFOsciParam::WaveDSFDistance, waveDSFDistanceNorm));
  _fmExp = topo.NormalizedToBoolFast(FFOsciParam::FMExp, fmExpNorm);
  _fmRatioMode = topo.NormalizedToListFast<FFOsciFMRatioMode>(FFOsciParam::FMRatioMode, fmRatioModeNorm);
  _fmRatioRatio12 = FMRatioRatio(topo.NormalizedToDiscreteFast(FFOsciParam::FMRatioRatio, fmRatioRatio12Norm));
  _fmRatioRatio23 = FMRatioRatio(topo.NormalizedToDiscreteFast(FFOsciParam::FMRatioRatio, fmRatioRatio23Norm));

  for (int i = 0; i < FFOsciWavePWCount; i++)
  {
    auto const& wavePWModeNorm = params.block.wavePWMode[i].Voice()[voice];
    _wavePWMode[i] = topo.NormalizedToListFast<FFOsciWavePWMode>(FFOsciParam::WavePWMode, wavePWModeNorm);
  }
  for (int i = 0; i < FFOsciWaveBasicCount; i++)
  {
    auto const& waveBasicModeNorm = params.block.waveBasicMode[i].Voice()[voice];
    _waveBasicMode[i] = topo.NormalizedToListFast<FFOsciWaveBasicMode>(FFOsciParam::WaveBasicMode, waveBasicModeNorm);
  }

  bool oversampling = modTopo.NormalizedToBoolFast(FFOsciModParam::Oversampling, modOversamplingNorm);
  _modMatrixExpoFM = modTopo.NormalizedToBoolFast(FFOsciModParam::ExpoFM, modExpoFMNorm);
  _oversamplingTimes = oversampling ? FFOsciOversamplingTimes : 1;

  for (int modSlot = modStartSlot; modSlot < modStartSlot + state.moduleSlot; modSlot++)
  {
    int srcOsciSlot = modSlot - modStartSlot;
    auto const& srcOsciParams = procState->param.voice.osci[srcOsciSlot];
    auto const& modFMOnNorm = modParams.block.fmOn[modSlot].Voice()[voice];
    auto const& modAMModeNorm = modParams.block.amMode[modSlot].Voice()[voice];
    auto const& modUniCountNorm = srcOsciParams.block.uniCount[0].Voice()[voice];
    _modSourceFMOn[srcOsciSlot] = modTopo.NormalizedToBoolFast(FFOsciModParam::FMOn, modFMOnNorm);
    _modSourceUniCount[srcOsciSlot] = topo.NormalizedToDiscreteFast(FFOsciParam::UniCount, modUniCountNorm);
    _modSourceAMMode[srcOsciSlot] = modTopo.NormalizedToListFast<FFOsciModAMMode>(FFOsciModParam::AMMode, modAMModeNorm);
  }
  
  FBSIMDArray<float, FFOsciUniMaxCount> uniPhaseInit = {};
  for (int u = 0; u < _uniCount; u++)
  {
    float random = _uniRandomPlain;
    float uniPhase = u * _uniOffsetPlain / _uniCount;
    uniPhaseInit.Set(u, ((1.0f - _uniRandomPlain) + _uniRandomPlain * _prng.Next()) * uniPhase);
    _uniPhaseGens[u] = FFOsciPhaseGenerator(uniPhaseInit.Get(u));
    if (_uniCount == 1)
    {
      _uniPosMHalfToHalf.Set(u, 0.0f);
      _uniPosAbsHalfToHalf.Set(u, 0.0f);
    }
    else
    {
      _uniPosMHalfToHalf.Set(u, u / (_uniCount - 1.0f) - 0.5f);
      _uniPosAbsHalfToHalf.Set(u, std::fabs(_uniPosMHalfToHalf.Get(u)));
    }
  }

  if (_type == FFOsciType::FM)
  {
    _prevUniFMOutput.Fill(0.0f);
    for (int o = 0; o < FFOsciFMOperatorCount; o++)
      for (int u = 0; u < _uniCount; u += FBSIMDFloatCount)
      {
        auto phaseInits = FBSIMDVector<float>::load_aligned(uniPhaseInit.Ptr(0) + u);
        _uniFMPhaseGens[o][u / FBSIMDFloatCount] = FFOsciFMPhaseGenerator(phaseInits);
      }
  }
}

int
FFOsciProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto& voiceState = procState->dsp.voice[voice];
  auto& output = voiceState.osci[state.moduleSlot].output;
  auto& uniOutputOversampled = voiceState.osci[state.moduleSlot].uniOutputOversampled;

  output.Fill(0.0f);
  uniOutputOversampled.Fill(0.0f);
  if (_type == FFOsciType::Off)
    return 0;

  float sampleRate = state.input->sampleRate;
  float oversampledRate = sampleRate * _oversamplingTimes;
  int totalSamples = FBFixedBlockSamples * _oversamplingTimes;
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  int prevPositionSamplesUpToFirstCycle = _phaseGen.PositionSamplesUpToFirstCycle();

  auto const& fineNorm = procParams.acc.fine[0].Voice()[voice];
  auto const& coarseNorm = procParams.acc.coarse[0].Voice()[voice];
  auto const& gainNorm = procParams.acc.gain[0].Voice()[voice];
  auto const& uniBlendNorm = procParams.acc.uniBlend[0].Voice()[voice];
  auto const& uniDetuneNorm = procParams.acc.uniDetune[0].Voice()[voice];
  auto const& uniSpreadNorm = procParams.acc.uniSpread[0].Voice()[voice];
  auto const& waveHSGainNorm = procParams.acc.waveHSGain[0].Voice()[voice];
  auto const& waveHSSyncNorm = procParams.acc.waveHSSync[0].Voice()[voice];
  auto const& waveDSFDecayNorm = procParams.acc.waveDSFDecay[0].Voice()[voice];

  FBSIMDArray<float, FFOsciFixedBlockOversamples> gainPlain;
  FBSIMDArray<float, FFOsciFixedBlockOversamples> baseFreqPlain;
  FBSIMDArray<float, FFOsciFixedBlockOversamples> basePitchPlain;
  FBSIMDArray<float, FFOsciFixedBlockOversamples> uniBlendPlain;
  FBSIMDArray<float, FFOsciFixedBlockOversamples> uniDetunePlain;
  FBSIMDArray<float, FFOsciFixedBlockOversamples> uniSpreadPlain;
  FBSIMDArray<float, FFOsciFixedBlockOversamples> waveHSGainPlain;
  FBSIMDArray<float, FFOsciFixedBlockOversamples> waveHSSyncPlain;
  FBSIMDArray<float, FFOsciFixedBlockOversamples> waveDSFDecayPlain;
  FBSIMDArray2<float, FFOsciFixedBlockOversamples, FFOsciWavePWCount> wavePWPWPlain;
  FBSIMDArray2<float, FFOsciFixedBlockOversamples, FFOsciWavePWCount> wavePWGainPlain;
  FBSIMDArray2<float, FFOsciFixedBlockOversamples, FFOsciWaveBasicCount> waveBasicGainPlain;
  FBSIMDArray2<float, FFOsciFixedBlockOversamples, FFOsciFMMatrixSize> fmIndexPlain;
  FBSIMDArray2<float, FFOsciFixedBlockOversamples, FFOsciFMOperatorCount - 1> fmRatioPlain;
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    auto fine = topo.NormalizedToLinearFast(FFOsciParam::Fine, fineNorm, s);
    auto coarse = topo.NormalizedToLinearFast(FFOsciParam::Coarse, coarseNorm, s);
    auto pitch = _key + coarse + fine;
    auto baseFreq = FBPitchToFreq(pitch);
    basePitchPlain.Store(s, pitch);
    baseFreqPlain.Store(s, baseFreq);
    _phaseGen.Next(baseFreq / sampleRate);

    gainPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::Gain, gainNorm, s));
    uniBlendPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::UniBlend, uniBlendNorm, s));
    uniDetunePlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::UniDetune, uniDetuneNorm, s));
    uniSpreadPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::UniSpread, uniSpreadNorm, s));

    if (_type == FFOsciType::Wave)
    {
      for (int i = 0; i < FFOsciWaveBasicCount; i++)
        if (_waveBasicMode[i] != FFOsciWaveBasicMode::Off)
        {
          auto const& waveBasicGainNorm = procParams.acc.waveBasicGain[i].Voice()[voice];
          waveBasicGainPlain[i].Store(s, topo.NormalizedToLinearFast(FFOsciParam::WaveBasicGain, waveBasicGainNorm, s));
        }
      for (int i = 0; i < FFOsciWavePWCount; i++)
        if (_wavePWMode[i] != FFOsciWavePWMode::Off)
        {
          auto const& wavePWPWNorm = procParams.acc.wavePWPW[i].Voice()[voice];
          auto const& wavePWGainNorm = procParams.acc.wavePWGain[i].Voice()[voice];
          wavePWPWPlain[i].Store(s, topo.NormalizedToIdentityFast(FFOsciParam::WavePWPW, wavePWPWNorm, s));
          wavePWGainPlain[i].Store(s, topo.NormalizedToLinearFast(FFOsciParam::WavePWGain, wavePWGainNorm, s));
        }
      if (_waveHSMode != FFOsciWaveHSMode::Off)
      {
        waveHSGainPlain.Store(s, topo.NormalizedToLinearFast(FFOsciParam::WaveHSGain, waveHSGainNorm, s));
        waveHSSyncPlain.Store(s, topo.NormalizedToLinearFast(FFOsciParam::WaveHSSync, waveHSSyncNorm, s));
      }
      if (_waveDSFMode != FFOsciWaveDSFMode::Off)
      {
        waveDSFDecayPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::WaveDSFDecay, waveDSFDecayNorm, s));
      }
    }
    else if (_type == FFOsciType::FM)
    {
      for (int m = 0; m < FFOsciFMMatrixSize; m++)
      {
        auto const& fmIndexNorm = procParams.acc.fmIndex[m].Voice()[voice];
        fmIndexPlain[m].Store(s, topo.NormalizedToLog2Fast(FFOsciParam::FMIndex, fmIndexNorm, s));
      }
      if (_fmRatioMode == FFOsciFMRatioMode::Ratio)
      {
        fmRatioPlain[0].Store(s, _fmRatioRatio12);
        fmRatioPlain[1].Store(s, _fmRatioRatio23);
      }
      else
      {
        for (int o = 0; o < FFOsciFMOperatorCount - 1; o++)
        {
          auto const& fmRatioFreeNorm = procParams.acc.fmRatioFree[o].Voice()[voice];
          fmRatioPlain[o].Store(s, topo.NormalizedToLog2Fast(FFOsciParam::FMRatioFree, fmRatioFreeNorm, s));
        }
      }
    }
    else
    {
      assert(false);
    }
  }
  if (_oversamplingTimes != 1)
  {
    gainPlain.UpsampleStretch<FFOsciOversamplingTimes>();
    basePitchPlain.UpsampleStretch<FFOsciOversamplingTimes>();
    baseFreqPlain.UpsampleStretch<FFOsciOversamplingTimes>();
    uniBlendPlain.UpsampleStretch<FFOsciOversamplingTimes>();
    uniDetunePlain.UpsampleStretch<FFOsciOversamplingTimes>();
    uniSpreadPlain.UpsampleStretch<FFOsciOversamplingTimes>();
    if (_type == FFOsciType::Wave)
    {
      for (int i = 0; i < FFOsciWaveBasicCount; i++)
        if (_waveBasicMode[i] != FFOsciWaveBasicMode::Off)
          waveBasicGainPlain[i].UpsampleStretch<FFOsciOversamplingTimes>();
      for (int i = 0; i < FFOsciWavePWCount; i++)
        if (_wavePWMode[i] != FFOsciWavePWMode::Off)
        {
          wavePWPWPlain[i].UpsampleStretch<FFOsciOversamplingTimes>();
          wavePWGainPlain[i].UpsampleStretch<FFOsciOversamplingTimes>();
        }
      if (_waveHSMode != FFOsciWaveHSMode::Off)
      {
        waveHSGainPlain.UpsampleStretch<FFOsciOversamplingTimes>();
        waveHSSyncPlain.UpsampleStretch<FFOsciOversamplingTimes>();
      }
      if (_waveDSFMode != FFOsciWaveDSFMode::Off)
      {
        waveDSFDecayPlain.UpsampleStretch<FFOsciOversamplingTimes>();
      }
    }
    else if (_type == FFOsciType::FM)
    {
      for (int m = 0; m < FFOsciFMMatrixSize; m++)
        fmIndexPlain[m].UpsampleStretch<FFOsciOversamplingTimes>();
      for (int o = 0; o < FFOsciFMOperatorCount - 1; o++)
        fmRatioPlain[o].UpsampleStretch<FFOsciOversamplingTimes>();
    }
    else
    {
      assert(false);
    }
  }

  float applyModMatrixExpoFM = _modMatrixExpoFM ? 1.0f : 0.0f;
  float applyModMatrixLinearFM = _modMatrixExpoFM ? 0.0f : 1.0f;
  if (_type != FFOsciType::FM)
  {
    for (int u = 0; u < _uniCount; u++)
    {
      float uniPosMHalfToHalf = _uniPosMHalfToHalf.Get(u);
      for (int s = 0; s < totalSamples; s += FBSIMDFloatCount)
      {
        FBSIMDVector<float> matrixFMMod(0.0f);
        for (int src = 0; src < state.moduleSlot; src++)
          if (_modSourceFMOn[src] && _modSourceUniCount[src] > u)
          {
            int modSlot = OsciModStartSlot(state.moduleSlot) + src;
            auto const& thatUniOutput = voiceState.osci[src].uniOutputOversampled[u].Load(s);
            matrixFMMod += thatUniOutput * voiceState.osciMod.outputFMIndex[modSlot].Load(s);
          }

        auto uniPitch = basePitchPlain.Load(s);
        uniPitch += uniPosMHalfToHalf * uniDetunePlain.Load(s);
        uniPitch += matrixFMMod * uniPitch * applyModMatrixExpoFM;

        auto uniFreq = FBPitchToFreq(uniPitch);
        auto uniIncr = uniFreq / oversampledRate;
        auto uniPhase = _uniPhaseGens[u].Next(uniIncr, matrixFMMod * applyModMatrixLinearFM);

        FBSIMDVector<float> thisUniOutput = 0.0f;
        if (_type == FFOsciType::Wave)
        {
          for (int i = 0; i < FFOsciWaveBasicCount; i++)
            if (_waveBasicMode[i] != FFOsciWaveBasicMode::Off)
            {
              auto waveBasicGain = waveBasicGainPlain[i].Load(s);
              thisUniOutput += GenerateWaveBasic(_waveBasicMode[i], uniPhase, uniIncr) * waveBasicGain;
            }
          for (int i = 0; i < FFOsciWavePWCount; i++)
            if (_wavePWMode[i] != FFOsciWavePWMode::Off)
            {
              auto wavePWPW = wavePWPWPlain[i].Load(s);
              auto wavePWGain = wavePWGainPlain[i].Load(s);
              wavePWPW = (MinPW + (1.0f - MinPW) * wavePWPW) * 0.5f;
              thisUniOutput += GenerateWavePW(_wavePWMode[i], uniPhase, uniIncr, wavePWPW) * wavePWGain;
            }
          if (_waveHSMode != FFOsciWaveHSMode::Off)
          {
            auto waveHSGain = waveHSGainPlain.Load(s);
            auto waveHSSync = waveHSSyncPlain.Load(s);
            auto uniSyncFreq = FBPitchToFreq(uniPitch + waveHSSync);
            auto uniSyncFreqRatio = uniSyncFreq / uniFreq;
            thisUniOutput += GenerateWaveHS(_waveHSMode, uniPhase, uniIncr, uniSyncFreqRatio) * waveHSGain;
          }
          if (_waveDSFMode != FFOsciWaveDSFMode::Off)
          {
            auto waveDSFDecay = waveDSFDecayPlain.Load(s);
            auto distFreq = _waveDSFDistance * uniFreq;
            auto maxOvertones = xsimd::max(FBSIMDVector<float>(0.0f), (sampleRate * 0.5f - uniFreq) / distFreq);
            if (_waveDSFMode == FFOsciWaveDSFMode::Over)
              thisUniOutput += GenerateDSFOvertones(uniPhase, uniFreq, waveDSFDecay, distFreq, maxOvertones, _waveDSFOver);
            else if (_waveDSFMode == FFOsciWaveDSFMode::BW)
              thisUniOutput += GenerateDSFBandwidth(uniPhase, uniFreq, waveDSFDecay, distFreq, maxOvertones, _waveDSFBWPlain);
            else
              assert(false);
          }
        }
        else
        {
          assert(false);
        }
        uniOutputOversampled[u].Store(s, thisUniOutput);
        for (int s2 = 0; s2 < FBSIMDFloatCount; s2++)
          assert(!std::isnan(uniOutputOversampled[u].Get(s + s2)));
      }
    }
  }

  if (_type == FFOsciType::FM)
  {
    for (int s = 0; s < totalSamples; s++)
      for (int u = 0; u < _uniCount; u += FBSIMDFloatCount)
      {
        int block = u / FBSIMDFloatCount;
        auto uniPosMHalfToHalf = _uniPosMHalfToHalf.Load(u);

        FBSIMDVector<float> matrixFMMod(0.0f);
        for (int src = 0; src < state.moduleSlot; src++)
          if (_modSourceFMOn[src] && _modSourceUniCount[src] > u)
          {
            int modSlot = OsciModStartSlot(state.moduleSlot) + src;
            float thatUniOutput = voiceState.osci[src].uniOutputOversampled[u].Get(s);
            matrixFMMod += thatUniOutput * voiceState.osciMod.outputFMIndex[modSlot].Get(s);
          }

        FBSIMDVector<float> op3UniPitch = basePitchPlain.Get(s) + uniPosMHalfToHalf * uniDetunePlain.Get(s);
        op3UniPitch += matrixFMMod * op3UniPitch * applyModMatrixExpoFM;
        auto op3UniFreq = FBPitchToFreq(op3UniPitch);

        FBSIMDVector<float> op1UniPhase;
        FBSIMDVector<float> fmTo1 = 0.0f;
        fmTo1 += fmIndexPlain[0].Get(s) * _prevUniFMOutput[0].Load(u);
        fmTo1 += fmIndexPlain[3].Get(s) * _prevUniFMOutput[1].Load(u);
        fmTo1 += fmIndexPlain[6].Get(s) * _prevUniFMOutput[2].Load(u);
        if (_fmExp)
        {
          auto op1UniPitch = op3UniPitch / fmRatioPlain[1].Get(s) / fmRatioPlain[0].Get(s);
          op1UniPitch += op1UniPitch * fmTo1;
          auto op1UniFreq = FBPitchToFreq(op1UniPitch);
          op1UniPhase = _uniFMPhaseGens[0][block].Next(op1UniFreq / oversampledRate, 0.0f);
        }
        else
        {
          auto op1UniFreq = op3UniFreq / fmRatioPlain[1].Get(s) / fmRatioPlain[0].Get(s);
          op1UniPhase = _uniFMPhaseGens[0][block].Next(op1UniFreq / oversampledRate, fmTo1);
        }
        auto output1 = xsimd::sin(op1UniPhase * FBTwoPi);

        FBSIMDVector<float> op2UniPhase;
        FBSIMDVector<float> fmTo2 = 0.0f;
        fmTo2 += fmIndexPlain[1].Get(s) * output1;
        fmTo2 += fmIndexPlain[4].Get(s) * _prevUniFMOutput[1].Load(u);
        fmTo2 += fmIndexPlain[7].Get(s) * _prevUniFMOutput[2].Load(u);
        if (_fmExp)
        {
          auto op2UniPitch = op3UniPitch / fmRatioPlain[1].Get(s);
          op2UniPitch += op2UniPitch * fmTo2;
          auto op2UniFreq = FBPitchToFreq(op2UniPitch);
          op2UniPhase = _uniFMPhaseGens[1][block].Next(op2UniFreq / oversampledRate, 0.0f);
        }
        else
        {
          auto op2UniFreq = op3UniFreq / fmRatioPlain[1].Get(s);
          op2UniPhase = _uniFMPhaseGens[1][block].Next(op2UniFreq / oversampledRate, fmTo2);
        }
        auto output2 = xsimd::sin(op2UniPhase * FBTwoPi);

        FBSIMDVector<float> op3UniPhase;
        FBSIMDVector<float> fmTo3 = 0.0f;
        fmTo3 += fmIndexPlain[2].Get(s) * output1;
        fmTo3 += fmIndexPlain[5].Get(s) * output2;
        fmTo3 += fmIndexPlain[8].Get(s) * _prevUniFMOutput[2].Load(u);
        if (_fmExp)
        {
          op3UniPitch += op3UniPitch * fmTo3;
          auto op3UniFreq = FBPitchToFreq(op3UniPitch);
          op3UniPhase = _uniFMPhaseGens[2][block].Next(op3UniFreq / oversampledRate, 0.0f);
        }
        else
        {
          op3UniPhase = _uniFMPhaseGens[2][block].Next(op3UniFreq / oversampledRate, fmTo3 + matrixFMMod);
        }
        auto output3 = xsimd::sin(op3UniPhase * FBTwoPi);

        _prevUniFMOutput[0].Store(u, output1);
        _prevUniFMOutput[1].Store(u, output2);
        _prevUniFMOutput[2].Store(u, output3);

        FBSIMDArray<float, FBSIMDFloatCount> outputArray;
        outputArray.Store(0, output3);
        for (int v = 0; v < FBSIMDFloatCount; v++)
          uniOutputOversampled[u + v].Set(s, outputArray.Get(v));
      }
  }

  for (int u = 0; u < _uniCount; u++)
    for (int src = 0; src < state.moduleSlot; src++)
      if (_modSourceAMMode[src] != FFOsciModAMMode::Off && _modSourceUniCount[src] > u)
      {
        float scale = _modSourceAMMode[src] == FFOsciModAMMode::RM ? 1.0f : 0.5f;
        float offset = _modSourceAMMode[src] == FFOsciModAMMode::RM ? 0.0f : 0.5f;
        for (int s = 0; s < totalSamples; s += FBSIMDFloatCount)
        {
          int modSlot = OsciModStartSlot(state.moduleSlot) + src;
          auto outputAMMix = voiceState.osciMod.outputAMMix[modSlot].Load(s);
          auto thisUniOutput = uniOutputOversampled[u].Load(s);
          auto const& thatUniOutput = voiceState.osci[src].uniOutputOversampled[u].Load(s);
          uniOutputOversampled[u].Store(s, (1.0f - outputAMMix) * thisUniOutput + outputAMMix * thisUniOutput * (thatUniOutput * scale + offset));
        }
      }

  if (_oversamplingTimes == 1)
    for (int u = 0; u < _uniCount; u++)
      for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
        _uniOutputDownsampled[u].Store(s, uniOutputOversampled[u].Load(s));
  else
  {
    for (int u = 0; u < _uniCount; u++)
      for (int s = 0; s < FFOsciFixedBlockOversamples; s++)
        _oversampledBlock.setSample(u, s, uniOutputOversampled[u].Get(s));
    auto channelBlockDown = _downsampledBlock.getSubsetChannelBlock(0, _uniCount);
    _oversampler.processSamplesDown(channelBlockDown);
  }

  for (int u = 0; u < _uniCount; u++)
  {
    float uniPosMHalfToHalf = _uniPosMHalfToHalf.Get(u);
    float uniPosAbsHalfToHalf = _uniPosAbsHalfToHalf.Get(u);
    for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
    {
      auto uniPanning = 0.5f + uniPosMHalfToHalf * uniSpreadPlain.Load(s);
      auto uniBlend = 1.0f - (uniPosAbsHalfToHalf * 2.0f * (1.0f - uniBlendPlain.Load(s)));
      auto uniMono = _uniOutputDownsampled[u].Load(s) * gainPlain.Load(s) * uniBlend;
      output[0].Add(s, (1.0f - uniPanning) * uniMono);
      output[1].Add(s, uniPanning * uniMono);
      for (int s2 = 0; s2 < FBSIMDFloatCount; s2++)
      {
        assert(!std::isnan(output[0].Get(s + s2)));
        assert(!std::isnan(output[1].Get(s + s2)));
      }
    }
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return _phaseGen.PositionSamplesUpToFirstCycle() - prevPositionSamplesUpToFirstCycle;

  auto& exchangeDSP = exchangeToGUI->voice[voice].osci[state.moduleSlot];
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = FBFreqToSamples(baseFreqPlain.Get(_oversamplingTimes * FBFixedBlockSamples - 1), state.input->sampleRate);
  exchangeDSP.positionSamples = _phaseGen.PositionSamplesCurrentCycle() % exchangeDSP.lengthSamples;

  auto& exchangeParams = exchangeToGUI->param.voice.osci[state.moduleSlot];
  exchangeParams.acc.gain[0][voice] = gainNorm.Last();
  exchangeParams.acc.fine[0][voice] = fineNorm.Last();
  exchangeParams.acc.coarse[0][voice] = coarseNorm.Last();
  exchangeParams.acc.uniBlend[0][voice] = uniBlendNorm.Last();
  exchangeParams.acc.uniDetune[0][voice] = uniDetuneNorm.Last();
  exchangeParams.acc.uniSpread[0][voice] = uniSpreadNorm.Last();
  exchangeParams.acc.waveHSGain[0][voice] = waveHSGainNorm.Last();
  exchangeParams.acc.waveHSSync[0][voice] = waveHSSyncNorm.Last();
  exchangeParams.acc.waveDSFDecay[0][voice] = waveDSFDecayNorm.Last();
  for (int i = 0; i < FFOsciWaveBasicCount; i++)
  {
    auto const& waveBasicGainNorm = procParams.acc.waveBasicGain[i].Voice()[voice];
    exchangeParams.acc.waveBasicGain[i][voice] = waveBasicGainNorm.Last();
  }
  for (int i = 0; i < FFOsciWavePWCount; i++)
  {
    auto const& wavePWPWNorm = procParams.acc.wavePWPW[i].Voice()[voice];
    auto const& wavePWGainNorm = procParams.acc.wavePWGain[i].Voice()[voice];
    exchangeParams.acc.wavePWPW[i][voice] = wavePWPWNorm.Last();
    exchangeParams.acc.wavePWGain[i][voice] = wavePWGainNorm.Last();
  }
  for (int o = 0; o < FFOsciFMOperatorCount - 1; o++)
  {
    auto const& fmRatioFreeNorm = procParams.acc.fmRatioFree[o].Voice()[voice];
    exchangeParams.acc.fmRatioFree[o][voice] = fmRatioFreeNorm.Last();
  }
  for (int m = 0; m < FFOsciFMMatrixSize; m++)
  {
    auto const& fmIndexNorm = procParams.acc.fmIndex[m].Voice()[voice];
    exchangeParams.acc.fmIndex[m][voice] = fmIndexNorm.Last();
  }
  return FBFixedBlockSamples;
}