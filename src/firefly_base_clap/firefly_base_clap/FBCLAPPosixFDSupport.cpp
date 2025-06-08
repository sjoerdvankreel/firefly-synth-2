#if (defined __linux__) || (defined  __FreeBSD__)
#include <firefly_base_clap/FBCLAPPlugin.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/dsp/host/FBHostProcessor.hpp>

#include <juce_events/native/juce_EventLoopInternal_linux.h>

using namespace juce;

bool 
FBCLAPPlugin::implementsPosixFdSupport() const noexcept
{
  return true;
}

void 
FBCLAPPlugin::onPosixFd(int fd, clap_posix_fd_flags_t flags) noexcept
{
  FBWithLogException([fd]() { LinuxEventLoopInternal::invokeEventLoopCallbackForFd(fd); });
}
#endif