#include <playground_base_vst3/FBVST3Utility.hpp>
#include <cstring>

void
FBVST3CopyToString128(std::string const& in, TChar* out)
{
  memset(out, 0, 128 * sizeof(TChar));
  for (int i = 0; i < 127 && i < in.size(); i++)
    out[i] = in[i];
}