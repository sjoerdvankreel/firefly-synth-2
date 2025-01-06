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
FBVST3CopyFromString128(TChar const* in, std::string& out)
{
  out.clear();
  for (int i = 0; i < 127 && in[i] != (TChar)0; i++)
    if (in[i] > 127)
      return false;
    else
      out.push_back((char)in[i]);
  return true;
}

bool
FBVST3LoadIBStream(IBStream* stream, std::string& state)
{
  state = {};
  int32 read = 0;
  char buffer[1024];
  tresult result = kResultFalse;
  while ((result = stream->read(buffer, sizeof(buffer), &read)) == kResultTrue && read > 0)
    state.append(buffer, read);
  return result == kResultTrue;
}

bool
FBVST3SaveIBStream(IBStream* stream, std::string const& state)
{
  int32 written = 0;
  int32 numWritten = 0;
  tresult result = kResultFalse;
  char* data = const_cast<char*>(state.data());
  while ((result = stream->write(data + written, static_cast<int32>(state.size() - written), &numWritten)) == kResultTrue && numWritten > 0)
  {
    written += numWritten;
    numWritten = 0;
  }
  return result == kResultTrue;
}