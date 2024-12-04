#include <playground_base_vst3/FBVST3Utility.hpp>
#include <cstring>

void
FBVST3CopyToString128(std::string const& in, TChar* out)
{
  memset(out, 0, 128 * sizeof(TChar));
  for (int i = 0; i < 127 && i < in.size(); i++)
    out[i] = in[i];
}

std::string
FBVST3LoadIBStream(IBStream* stream)
{
  int32 read;
  char buffer[1024];
  std::string result = {};
  while (stream->read(buffer, sizeof(buffer), &read) == kResultTrue && read > 0)
    result.append(buffer, read);
  return result;
}