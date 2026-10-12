#include "../Headers/Runtime.h"
#include "../Headers/Entity.h"
#include <iostream>

void Entities::Square::Update (float deltaTime) {
transform.position.x += 1 * deltaTime;
//std::cout << transform.position.x << "Square transform.position.x" << std::endl;
    if (transform.position.x > 5) {
        Destroy();
    }
}
Entities::Square::~Square() {
    //std::cout << transform.position.x << "SQUARE WAS DESTROED" << std::endl;
}

void Entities::Triangle::Update (float deltaTime) {
    transform.position.x += 1 * deltaTime;
  //  std::cout << transform.position.x << "Triangle transform.position.x" << std::endl;
}

void Entities::Circle::Update (float deltaTime) {
    transform.position.x += 1 * deltaTime;
    //std::cout << transform.position.x << "Circle transform.position.x" << std::endl;
}