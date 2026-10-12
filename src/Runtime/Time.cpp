#include "../Headers/Runtime.h"
#include "chrono"

    float Engine::TimeCalculation::CalculateDeltaTime() {
        t1 = std::chrono::steady_clock::now();
        delta = std::chrono::duration<float>(t1 - t0).count();
        t0 = t1;
        return delta;
    }

    float Engine::TimeCalculation::CalculateTime() {
        t1t = std::chrono::steady_clock::now();
        time = std::chrono::duration<float>(t1t - t0t).count();
        return time;
    }