#include "Headers/Runtime.h"

namespace mainClass{
    class mainClass {
        public:
        Engine::Runtime runtime{};
    };
}


int main() {
    mainClass::mainClass runtime;
    runtime.runtime.Run();
}