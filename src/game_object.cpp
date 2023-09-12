#include "game_object.hpp"

glm::vec2
GameObject::getOldCenter() const
{
    return this->_oldCenter;
}

glm::vec2
GameObject::getCenter() const
{
    return this->_center;
}

float
GameObject::getRadius() const
{
    return this->_radius;
}

void
GameObject::setCenter(glm::vec2 c)
{
    this->_oldCenter = this->_center;
    this->_center = c;
}

void
GameObject::setRadius(float r)
{
    this->_radius = r;
}
