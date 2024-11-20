#include <playground_plug/base/FFBlock.hpp>

void boo()
{
  FFDynamicMonoBlock const xx(90);
  FFDynamicMonoBlock xx1(91);
  //xx[90] = 3;
  float z = xx[0];
  xx.CopyTo(xx1);

  FFFixedMonoBlock const zz;
  //auto zz1 = zz;
  zz[4] = 9;
  zz[99] = 90;

  std::array<float, 1> zork;
  zork[9] = 3;
}