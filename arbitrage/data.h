#ifndef Data_h
#define Data_h

#include "CurlObj.h"

class Data {
    struct arbitrageData {
        std::string buyPair = "";
        std::string sellExchange = "";
        std::string sellPair = "";
        std::string buyExchange = "";
        float buyPrice = 0;
        float sellPrice = 0;
        float profitRatio = 0;
    };
private:
    std::string html;
    arbitrageData trades[100];
    void beginPrint();
    void printData(struct arbitrageData ad);
    float minLiquidity;
    float minVolume;
public:
    Data(float ml, float mv);
    void getData();
    int getHighestIndex(int id);
    int getLowestIndex(int id);
    };

#endif