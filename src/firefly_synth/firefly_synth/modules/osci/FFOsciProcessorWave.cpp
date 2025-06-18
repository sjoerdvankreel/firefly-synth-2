#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/osci/FFOsciTopo.hpp>
#include <firefly_synth/modules/osci/FFOsciProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

#include <xsimd/xsimd.hpp>

static inline float constexpr MinPW = 0.05f;
static inline float const Exp2 = std::exp(2.0f);
static inline float const SqrtPi = std::sqrt(FBPi);

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

static inline FBBatch<float>
WaveBasicSin(
  FBBatch<float> t)
{
  return xsimd::sin(t * FBTwoPi);
}

static inline FBBatch<float>
WaveBasicCos(
  FBBatch<float> t)
{
  return xsimd::cos(t * FBTwoPi);
}

static inline FBBatch<float>
WaveBasicSaw(
  FBBatch<float> tVec,
  FBBatch<float> dtVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
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

static inline FBBatch<float>
WaveBasicRamp(
  FBBatch<float> tVec,
  FBBatch<float> dtVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
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

static inline FBBatch<float>
WaveBasicRect(
  FBBatch<float> tVec,
  FBBatch<float> dtVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
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

static inline FBBatch<float>
WaveBasicTri(
  FBBatch<float> tVec,
  FBBatch<float> dtVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
  tArr.Store(0, tVec);
  dtArr.Store(0, dtVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float t = tArr.Get(i);
    float dt = dtArr.Get(i);
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

static inline FBBatch<float>
WaveBasicTrap(
  FBBatch<float> tVec,
  FBBatch<float> dtVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
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

static inline FBBatch<float>
WaveBasicSinSqr(
  FBBatch<float> tVec,
  FBBatch<float> dtVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
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

static inline FBBatch<float>
WaveBasicSawSqr(
  FBBatch<float> tVec,
  FBBatch<float> dtVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
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

static inline FBBatch<float>
WaveBasicSinSaw(
  FBBatch<float> tVec,
  FBBatch<float> dtVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
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

static inline FBBatch<float>
WaveBasicSawM1(
  FBBatch<float> tVec,
  FBBatch<float> dtVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
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

static inline FBBatch<float>
WaveBasicSqrM1(
  FBBatch<float> tVec,
  FBBatch<float> dtVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
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

static inline FBBatch<float>
WaveBasicBSSin(
  FBBatch<float> tVec,
  FBBatch<float> dtVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
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

static inline FBBatch<float>
WaveBasicHWSin(
  FBBatch<float> tVec,
  FBBatch<float> dtVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
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

static inline FBBatch<float>
WaveBasicFWSin(
  FBBatch<float> tVec,
  FBBatch<float> dtVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
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

static inline FBBatch<float>
WaveBasicAltSin(
  FBBatch<float> tVec,
  FBBatch<float> dtVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
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

static inline FBBatch<float>
WaveBasicParabl(
  FBBatch<float> tVec,
  FBBatch<float> dtVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
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

static inline FBBatch<float>
WaveBasicHypTri(
  FBBatch<float> tVec,
  FBBatch<float> dtVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
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

static inline FBBatch<float>
WavePWRect(
  FBBatch<float> tVec,
  FBBatch<float> dtVec,
  FBBatch<float> pwVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
  FBSArray<float, FBSIMDFloatCount> pwArr;
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

static inline FBBatch<float>
WavePWSqr(
  FBBatch<float> tVec,
  FBBatch<float> dtVec,
  FBBatch<float> pwVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
  FBSArray<float, FBSIMDFloatCount> pwArr;
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

static inline FBBatch<float>
WavePWHWSaw(
  FBBatch<float> tVec,
  FBBatch<float> dtVec,
  FBBatch<float> pwVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
  FBSArray<float, FBSIMDFloatCount> pwArr;
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

static inline FBBatch<float>
WavePWTriSaw(
  FBBatch<float> tVec,
  FBBatch<float> dtVec,
  FBBatch<float> pwVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
  FBSArray<float, FBSIMDFloatCount> pwArr;
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

static inline FBBatch<float>
WavePWTriPls(
  FBBatch<float> tVec,
  FBBatch<float> dtVec,
  FBBatch<float> pwVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
  FBSArray<float, FBSIMDFloatCount> pwArr;
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

static inline FBBatch<float>
WavePWTrapTri(
  FBBatch<float> tVec,
  FBBatch<float> dtVec,
  FBBatch<float> pwVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
  FBSArray<float, FBSIMDFloatCount> pwArr;
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

static inline FBBatch<float>
WaveHSSaw(
  FBBatch<float> tVec,
  FBBatch<float> dtVec,
  FBBatch<float> freqRatioVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
  FBSArray<float, FBSIMDFloatCount> frArr;
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

    for (int j = 0; j < n; j++)
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

static inline FBBatch<float>
WaveHSSqr(
  FBBatch<float> tVec,
  FBBatch<float> dtVec,
  FBBatch<float> freqRatioVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
  FBSArray<float, FBSIMDFloatCount> frArr;
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

    for (int j = 0; j < 2 * n; j++)
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

static inline FBBatch<float>
WaveHSTri(
  FBBatch<float> tVec,
  FBBatch<float> dtVec,
  FBBatch<float> freqRatioVec)
{
  FBSArray<float, FBSIMDFloatCount> tArr;
  FBSArray<float, FBSIMDFloatCount> yArr;
  FBSArray<float, FBSIMDFloatCount> dtArr;
  FBSArray<float, FBSIMDFloatCount> frArr;
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

    for (int j = 0; j < n; j++)
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

static inline FBBatch<float>
GenerateWaveBasicCheck(
  FFOsciWaveBasicMode mode,
  FBBatch<float> phaseVec,
  FBBatch<float> incrVec)
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
  default: FB_ASSERT(false); return 0.0f;
  }
}

static inline FBBatch<float>
GenerateWavePWCheck(
  FFOsciWavePWMode mode,
  FBBatch<float> phaseVec,
  FBBatch<float> incrVec,
  FBBatch<float> pwVec)
{
  switch (mode)
  {
  case FFOsciWavePWMode::Rect: return WavePWRect(phaseVec, incrVec, pwVec);
  case FFOsciWavePWMode::Sqr: return WavePWSqr(phaseVec, incrVec, pwVec);
  case FFOsciWavePWMode::HWSaw: return WavePWHWSaw(phaseVec, incrVec, pwVec);
  case FFOsciWavePWMode::TriSaw: return WavePWTriSaw(phaseVec, incrVec, pwVec);
  case FFOsciWavePWMode::TriPls: return WavePWTriPls(phaseVec, incrVec, pwVec);
  case FFOsciWavePWMode::TrapTri: return WavePWTrapTri(phaseVec, incrVec, pwVec);
  default: FB_ASSERT(false); return 0.0f;
  }
}

static inline FBBatch<float>
GenerateWaveHSCheck(
  FFOsciWaveHSMode mode,
  FBBatch<float> phaseVec,
  FBBatch<float> incrVec,
  FBBatch<float> freqRatioVec)
{
  switch (mode)
  {
  case FFOsciWaveHSMode::Saw: return WaveHSSaw(phaseVec, incrVec, freqRatioVec);
  case FFOsciWaveHSMode::Sqr: return WaveHSSqr(phaseVec, incrVec, freqRatioVec);
  case FFOsciWaveHSMode::Tri: return WaveHSTri(phaseVec, incrVec, freqRatioVec);
  default: FB_ASSERT(false); return 0.0f;
  }
}

static inline FBBatch<float>
GenerateWaveBasic(
  FFOsciWaveBasicMode mode,
  FBBatch<float> phaseVec,
  FBBatch<float> incrVec)
{
  auto result = GenerateWaveBasicCheck(mode, phaseVec, incrVec);
  FBBatchNaNCheck(result);
  return result;
}

static inline FBBatch<float>
GenerateWavePW(
  FFOsciWavePWMode mode,
  FBBatch<float> phaseVec,
  FBBatch<float> incrVec,
  FBBatch<float> pwVec)
{
  auto result = GenerateWavePWCheck(mode, phaseVec, incrVec, pwVec);
  FBBatchNaNCheck(result);
  return result;
}

static inline FBBatch<float>
GenerateWaveHS(
  FFOsciWaveHSMode mode,
  FBBatch<float> phaseVec,
  FBBatch<float> incrVec,
  FBBatch<float> freqRatioVec)
{
  auto result = GenerateWaveHSCheck(mode, phaseVec, incrVec, freqRatioVec);
  FBBatchNaNCheck(result);
  return result;
}

static inline FBBatch<float>
GenerateDSFCheck(
  FBBatch<float> phaseVec, FBBatch<float> freqVec,
  FBBatch<float> decayVec, FBBatch<float> distFreqVec,
  FBBatch<float> overtoneVec)
{
  float const decayRange = 0.99f;
  float const scaleFactor = 0.975f;

  auto n = overtoneVec;
  auto w = decayVec * decayRange;
  auto wPowNp1 = xsimd::pow(FBBatch<float>(w), overtoneVec + 1.0f);
  auto u = 2.0f * FBPi * phaseVec;
  auto v = 2.0f * FBPi * distFreqVec * phaseVec / freqVec;
  auto a = w * xsimd::sin(u + n * v) - xsimd::sin(u + (n + 1.0f) * v);
  auto x = (w * xsimd::sin(v - u) + xsimd::sin(u)) + wPowNp1 * a;
  auto y = 1.0f + w * w - 2.0f * w * xsimd::cos(v);
  auto scale = (1.0f - wPowNp1) / (1.0f - w);
  return x * scaleFactor / (y * scale);
}

static inline FBBatch<float>
GenerateDSF(
  FBBatch<float> phaseVec, FBBatch<float> freqVec,
  FBBatch<float> decayVec, FBBatch<float> distFreqVec,
  FBBatch<float> overtoneVec)
{
  auto result = GenerateDSFCheck(phaseVec, freqVec, decayVec, distFreqVec, overtoneVec);
  FBBatchNaNCheck(result);
  return result;
}

static inline FBBatch<float>
GenerateDSFOvertones(
  FBBatch<float> phaseVec, FBBatch<float> freqVec,
  FBBatch<float> decayVec, FBBatch<float> distFreqVec,
  FBBatch<float> maxOvertoneVec, float overtones_)
{
  auto overtoneVec = xsimd::min(FBBatch<float>(overtones_), xsimd::floor(maxOvertoneVec));
  return GenerateDSF(phaseVec, freqVec, decayVec, distFreqVec, overtoneVec);
}

static inline FBBatch<float>
GenerateDSFBandwidth(
  FBBatch<float> phaseVec, FBBatch<float> freqVec,
  FBBatch<float> decayVec, FBBatch<float> distFreqVec,
  FBBatch<float> maxOvertoneVec, float bandwidth)
{
  auto overtoneVec = 1.0f + xsimd::floor(bandwidth * (maxOvertoneVec - 1.0f));
  overtoneVec = xsimd::min(overtoneVec, xsimd::floor(maxOvertoneVec));
  return GenerateDSF(phaseVec, freqVec, decayVec, distFreqVec, overtoneVec);
}

void 
FFOsciProcessor::BeginVoiceWave(
  FBModuleProcState& state,
  FBSArray<float, FFOsciUniMaxCount> const& uniPhaseInit)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];

  auto const& waveHSModeNorm = params.block.waveHSMode[0].Voice()[voice];
  auto const& waveDSFBWNorm = params.block.waveDSFBW[0].Voice()[voice];
  auto const& waveDSFModeNorm = params.block.waveDSFMode[0].Voice()[voice];
  auto const& waveDSFOverNorm = params.block.waveDSFOver[0].Voice()[voice];
  auto const& waveDSFDistanceNorm = params.block.waveDSFDistance[0].Voice()[voice];

  _waveHSMode = topo.NormalizedToListFast<FFOsciWaveHSMode>(FFOsciParam::WaveHSMode, waveHSModeNorm);
  _waveDSFBWPlain = topo.NormalizedToLog2Fast(FFOsciParam::WaveDSFBW, waveDSFBWNorm);
  _waveDSFMode = topo.NormalizedToListFast<FFOsciWaveDSFMode>(FFOsciParam::WaveDSFMode, waveDSFModeNorm);
  _waveDSFOver = static_cast<float>(topo.NormalizedToDiscreteFast(FFOsciParam::WaveDSFOver, waveDSFOverNorm));
  _waveDSFDistance = static_cast<float>(topo.NormalizedToDiscreteFast(FFOsciParam::WaveDSFDistance, waveDSFDistanceNorm));

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
  for (int u = 0; u < _uniCount; u++)
  {
    _uniWavePhaseGens[u] = FFOsciWavePhaseGenerator(uniPhaseInit.Get(u));
  }
}

void 
FFOsciProcessor::ProcessWave(
  FBModuleProcState& state,
  FBSArray<float, FFOsciFixedBlockOversamples> const& basePitchPlain,
  FBSArray<float, FFOsciFixedBlockOversamples> const& uniDetunePlain)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  float sampleRate = state.input->sampleRate;
  float oversampledRate = sampleRate * _oversampleTimes;
  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  auto& voiceState = procState->dsp.voice[voice];
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  auto& uniOutputOversampled = voiceState.osci[state.moduleSlot].uniOutputOversampled;

  auto const& waveHSGainNorm = procParams.acc.waveHSGain[0].Voice()[voice];
  auto const& waveHSSyncNorm = procParams.acc.waveHSSync[0].Voice()[voice];
  auto const& waveDSFGainNorm = procParams.acc.waveDSFGain[0].Voice()[voice];
  auto const& waveDSFDecayNorm = procParams.acc.waveDSFDecay[0].Voice()[voice];

  FBSArray<float, FFOsciFixedBlockOversamples> waveHSGainPlain;
  FBSArray<float, FFOsciFixedBlockOversamples> waveHSSyncPlain;
  FBSArray<float, FFOsciFixedBlockOversamples> waveDSFGainPlain;
  FBSArray<float, FFOsciFixedBlockOversamples> waveDSFDecayPlain;
  FBSArray2<float, FFOsciFixedBlockOversamples, FFOsciWavePWCount> wavePWPWPlain;
  FBSArray2<float, FFOsciFixedBlockOversamples, FFOsciWavePWCount> wavePWGainPlain;
  FBSArray2<float, FFOsciFixedBlockOversamples, FFOsciWaveBasicCount> waveBasicGainPlain;
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
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
      waveDSFGainPlain.Store(s, topo.NormalizedToLinearFast(FFOsciParam::WaveDSFGain, waveDSFGainNorm, s));
      waveDSFDecayPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::WaveDSFDecay, waveDSFDecayNorm, s));
    }    
  }
  if (_oversampleTimes != 1)
  {  
    for (int i = 0; i < FFOsciWaveBasicCount; i++)
      if (_waveBasicMode[i] != FFOsciWaveBasicMode::Off)
        waveBasicGainPlain[i].UpsampleStretch<FFOsciOversampleTimes>();
    for (int i = 0; i < FFOsciWavePWCount; i++)
      if (_wavePWMode[i] != FFOsciWavePWMode::Off)
      {
        wavePWPWPlain[i].UpsampleStretch<FFOsciOversampleTimes>();
        wavePWGainPlain[i].UpsampleStretch<FFOsciOversampleTimes>();
      }
    if (_waveHSMode != FFOsciWaveHSMode::Off)
    {
      waveHSGainPlain.UpsampleStretch<FFOsciOversampleTimes>();
      waveHSSyncPlain.UpsampleStretch<FFOsciOversampleTimes>();
    }
    if (_waveDSFMode != FFOsciWaveDSFMode::Off)
    {
      waveDSFGainPlain.UpsampleStretch<FFOsciOversampleTimes>();
      waveDSFDecayPlain.UpsampleStretch<FFOsciOversampleTimes>();
    }
  }

  float applyModMatrixExpoFM = _modMatrixExpoFM ? 1.0f : 0.0f;
  float applyModMatrixLinearFM = _modMatrixExpoFM ? 0.0f : 1.0f;
  for (int u = 0; u < _uniCount; u++)
  {
    float uniPosMHalfToHalf = _uniPosMHalfToHalf.Get(u);
    for (int s = 0; s < totalSamples; s += FBSIMDFloatCount)
    {
      FBBatch<float> matrixFMMod(0.0f);
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
      auto uniPhase = _uniWavePhaseGens[u].Next(uniIncr, matrixFMMod * applyModMatrixLinearFM);

      FBBatch<float> thisUniOutput = 0.0f;
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
          wavePWPW = (MinPW + (1.0f - 2.0f * MinPW) * wavePWPW) * 0.5f;
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
        auto waveDSFGain = waveDSFGainPlain.Load(s);
        auto waveDSFDecay = waveDSFDecayPlain.Load(s);
        auto distFreq = _waveDSFDistance * uniFreq;
        auto maxOvertones = xsimd::max(FBBatch<float>(0.0f), (sampleRate * 0.5f - uniFreq) / distFreq);
        if (_waveDSFMode == FFOsciWaveDSFMode::Over)
          thisUniOutput += GenerateDSFOvertones(uniPhase, uniFreq, waveDSFDecay, distFreq, maxOvertones, _waveDSFOver) * waveDSFGain;
        else if (_waveDSFMode == FFOsciWaveDSFMode::BW)
          thisUniOutput += GenerateDSFBandwidth(uniPhase, uniFreq, waveDSFDecay, distFreq, maxOvertones, _waveDSFBWPlain) * waveDSFGain;
        else
          FB_ASSERT(false);
      }
      uniOutputOversampled[u].Store(s, thisUniOutput);
      for (int s2 = 0; s2 < FBSIMDFloatCount; s2++)
        FB_ASSERT(!std::isnan(uniOutputOversampled[u].Get(s + s2)));
    }
  }  

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;
  auto& exchangeParams = exchangeToGUI->param.voice.osci[state.moduleSlot];
  exchangeParams.acc.waveHSGain[0][voice] = waveHSGainNorm.Last();
  exchangeParams.acc.waveHSSync[0][voice] = waveHSSyncNorm.Last();
  exchangeParams.acc.waveDSFGain[0][voice] = waveDSFGainNorm.Last();
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
}