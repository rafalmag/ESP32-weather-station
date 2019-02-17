#include "Arduino.h"
#include "pmsCalc.h"

PmsCalc::PmsCalc(PMS &pms)
{
    _pms = &pms;
}

float PmsCalc::getPm1()
{
    return pm.pm1;
}
float PmsCalc::getPm2()
{
    return pm.pm2;
}
float PmsCalc::getPm10()
{
    return pm.pm10;
}

bool PmsCalc::updatePmReads()
{
    int reads = 0;
    uint16_t min1 = 0, max1 = 0, sum1 = 0,
             min2 = 0, max2 = 0, sum2 = 0,
             min10 = 0, max10 = 0, sum10 = 0;
    for (int i = 0; i < readIter; i++)
    {
        _pms->requestRead();
        if (_pms->readUntil(data, 10000))
        {
            if (reads == 0)
            {
                min1 = data.PM_AE_UG_1_0;
                max1 = data.PM_AE_UG_1_0;
                sum1 = data.PM_AE_UG_1_0;
                min2 = data.PM_AE_UG_2_5;
                max2 = data.PM_AE_UG_2_5;
                sum2 = data.PM_AE_UG_2_5;
                min10 = data.PM_AE_UG_10_0;
                max10 = data.PM_AE_UG_10_0;
                sum10 = data.PM_AE_UG_10_0;
            }
            else
            {
                if (data.PM_AE_UG_1_0 < min1)
                    min1 = data.PM_AE_UG_1_0;
                if (max1 < data.PM_AE_UG_1_0)
                    max1 = data.PM_AE_UG_1_0;
                sum1 += data.PM_AE_UG_1_0;
                if (data.PM_AE_UG_2_5 < min2)
                    min2 = data.PM_AE_UG_2_5;
                if (max2 < data.PM_AE_UG_2_5)
                    max2 = data.PM_AE_UG_2_5;
                sum2 += data.PM_AE_UG_2_5;
                if (data.PM_AE_UG_10_0 < min10)
                    min10 = data.PM_AE_UG_1_0;
                if (max10 < data.PM_AE_UG_10_0)
                    max10 = data.PM_AE_UG_10_0;
                sum10 += data.PM_AE_UG_10_0;
            }
            reads++;
        }
    }
    if (reads > 2)
    {
        pm.pm1 = (float)(sum1 - min1 - max1) / (float)(reads - 2);
        pm.pm2 = (float)(sum2 - min2 - max2) / (float)(reads - 2);
        pm.pm10 = (float)(sum10 - min10 - max10) / (float)(reads - 2);
    }
    else
    {
        pm.pm1 = min1;
        pm.pm2 = min2;
        pm.pm10 = min10;
    }
    return reads > 0;
}
