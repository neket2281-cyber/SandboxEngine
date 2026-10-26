#include "../Headers/Runtime.h"
#include "../Headers/Entity.h"


void Engine::Runtime::Run() {
    Start();
    while (isRunning) {
        Update();
    }
    End();
}

void Engine::Runtime::Start() {
    CreateEntity<Entities::Circle>();
    hinstance = GetModuleHandle(nullptr);
    window.StartWindowCreation(this);
    renderer.start(&hinstance, &window.handle);
}
void Engine::Runtime::Update() {
    Platform::ProcessMessages();
    delta = timeCalculation.calculateDeltaTime();
    updateDelay -= delta;
    deltaTime += delta;

    if (updateDelay > 0) {return;}

    //renderer.triangleSlideToRight();

    for (auto& element: entities) {
        element->Update(deltaTime);
    }

    for (int i = entities.size() - 1; i >= 0 ; i--) {
        if(entities[i]->isMarkedForDestruction)
            entities.erase(entities.begin() + i);
    }

    updateDelay += 0.016;
    deltaTime = 0;
}


void Engine::Runtime::Terminate() {
    isRunning = false;
}


void Engine::Runtime::End() {
}