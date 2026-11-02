#include <firefly_synth_diskwriter/FFDiskWriter.hpp>

int main(int argc, char** argv)
{
  (void)argc;
  (void)argv;
  new FFDiskWriter(FFDiskWriterOptions());
  return 0;
}