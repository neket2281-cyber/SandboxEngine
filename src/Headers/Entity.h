#include "Runtime.h"
#include <iostream>

#ifndef DEV_ENTITY_H
#define DEV_ENTITY_H

namespace Entities {
    class Square : public Engine::EngineObjects::Entity {
    public:
        void Update(float deltaTime) override;
        ~Square() override;
    };

    class Triangle : public Engine::EngineObjects::Entity {
    public:
        void Update(float deltaTime) override;

    };

    class Circle : public Engine::EngineObjects::Entity {
    public:
        void Update(float deltaTime) override;

    };
}

#endif //DEV_ENTITY_H