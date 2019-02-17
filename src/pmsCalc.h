#ifndef PMS_CALC_h_
#define PMS_CALC_h_

#include <PMS.h>

class PmsCalc
{
public:
  PmsCalc(PMS &);
  bool updatePmReads();
  float getPm1();
  float getPm2();
  float getPm10();

protected:
private:
  PMS *_pms;
  PMS::DATA data;

  struct pms5003
  {
    float pm1, pm2, pm10;
  };

  pms5003 pm;

  int readIter = 3;
};

#endif
