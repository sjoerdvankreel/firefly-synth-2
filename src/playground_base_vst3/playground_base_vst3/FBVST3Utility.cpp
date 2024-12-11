#include <playground_base_vst3/FBVST3Utility.hpp>
#include <cstring>

void
FBVST3CopyToString128(std::string const& in, TChar* out)
{
  memset(out, 0, 128 * sizeof(TChar));
  for (int i = 0; i < 127 && i < in.size(); i++)
    out[i] = in[i];
}

bool
FBVST3LoadIBStream(IBStream* stream, std::string& state)
{
  int32 read;
  state = {};
  tresult result;
  char buffer[1024];
  while ((result = stream->read(buffer, sizeof(buffer), &read)) == kResultTrue && read > 0)
    state.append(buffer, read);
  return result == kResultTrue;
}

bool
FBVST3SaveIBStream(IBStream* stream, std::string const& state)
{
  tresult result;
  int32 numWritten;
  int32 written = 0;
  char* data = const_cast<char*>(state.data());
  while ((result = stream->write(data + written, state.size() - written, &numWritten)) == kResultTrue && numWritten > 0)
    written += numWritten;
  return result == kResultTrue;
}