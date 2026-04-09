#include <chrono>

extern "C" {

double समय() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration<double>(now.time_since_epoch()).count();
}

double गणन(double a, double b) {
    return a + b;
}

}
