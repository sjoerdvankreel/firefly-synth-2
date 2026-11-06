#pragma once

#include <firefly_base/base/shared/FBHostContext.hpp>
#include <firefly_synth_diskwriter/FFDiskWriterOptions.hpp>

class FFDiskWriterContext :
public FBHostContext
{
  FFDiskWriterOptions const _options;
  FBScalarStateContainer _audioParams;

protected:
  void OnReceivedExchangeFromDSP() override {}
  void DoEndAudioParamChange(int) override {}
  void DoBeginAudioParamChange(int) override {}
  void DoPerformAudioParamEdit(int, double) override {}

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFDiskWriterContext);
  FFDiskWriterContext(FFDiskWriterOptions const& options);

  void ClearAudioParamNameOverrides() override {}
  void ClearAudioParamNameOverride(int) override {}
  void NotifyHostOfParamNameChangesOnly() override {}
  void AudioParamContextMenuClicked(int, int) override {}
  void NotifyHostOfBulkUpdateAndParamNameChanges() override {}
  void SetAudioParamNameOverride(int, std::string const&) override {}

  std::string OnlineManualLocation() const override { return {}; }
  bool GetAudioParamNameOverride(int, std::string&) const override { return {}; }
  std::vector<FBHostContextMenuItem> MakeAudioParamContextMenu(int) override { return {}; }

  float SampleRate() const override;
  double GetAudioParamNormalized(int index) const override;
  std::unique_ptr<IFBPlugProcessor> MakePlugProcessor() override;
  void BulkUpdateAudioParamsToGUI(FBScalarStateContainer const& audioParams, bool patchOnly) override;
};