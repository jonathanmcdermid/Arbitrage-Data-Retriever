#include <iostream>
#include "Data.h"

int main() {
    std::string buffer;
    float ml = -1, mv = -1;
    std::cout << "Enter the minimum liquidity rating from 0 - 1000\n";
    while (ml < 0) {
        try {
            std::cin >> buffer;
            ml = std::stof(buffer);
            if (ml < 0) {
                std::cout << "That is not a valid number";
            }
        }
        catch (std::exception e) {
            std::cout << "That is not a valid number";
        }
    }
    std::cout << "Enter the minimum volume in USD\n";
    while (mv < 0) {
        try {
            std::cin >> buffer;
            mv = std::stof(buffer);
            if (mv < 0) {
                std::cout << "That is not a valid number";
            }
        }
        catch (std::exception e) {
            std::cout << "That is not a valid number";
        }
    }
    Data data(ml, mv);
    data.getData();
    return 0;
}