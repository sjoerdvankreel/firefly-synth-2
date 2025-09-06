#include <firefly_base/dsp/shared/FBTuning.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>

#include <libMTSClient.h>

static MTSClient* _MTSClient = nullptr;

MTSClient*
FBTuningGetMTSClient()
{
  return _MTSClient;
}

void 
FBTuningInit()
{
  FB_LOG_INFO("Initializing MTS-ESP client.");
  _MTSClient = MTS_RegisterClient();
  FB_LOG_INFO("Initialized MTS-ESP client.");
}

void 
FBTuningTerminate()
{
  FB_LOG_INFO("Terminating MTS-ESP client.");
  MTS_DeregisterClient(_MTSClient);
  _MTSClient = nullptr;
  FB_LOG_INFO("Terminated MTS-ESP client.");
}