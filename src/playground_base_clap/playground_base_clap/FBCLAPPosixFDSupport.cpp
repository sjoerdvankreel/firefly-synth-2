#if (defined __linux__) || (defined  __FreeBSD__)
#include <playground_base_clap/FBCLAPPlugin.hpp>
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
  LinuxEventLoopInternal::invokeEventLoopCallbackForFd(fd);
}
#endif