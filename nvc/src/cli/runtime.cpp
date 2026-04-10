#include <iostream>
#include <chrono>
#include <cmath>

extern "C" {

double समय() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration<double>(now.time_since_epoch()).count();
}

double गणन(double a, double b) {
    return a + b;
}

double ज्या(double x) {
    return std::sin(x);
}

double कोटिज्या(double x) {
    return std::cos(x);
}

double वर्गमूलम्(double x) {
    return std::sqrt(x);
}

}
