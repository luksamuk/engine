#ifndef GAMEOBJECT_HPP_INCLUDED
#define GAMEOBJECT_HPP_INCLUDED

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <functional>
#include <optional>

class GameObject;

typedef std::shared_ptr<GameObject>  ObjPtr;

class GameObject
{
private:
    glm::vec2 _oldCenter;
    glm::vec2 _center;
    float     _radius;
public:
    virtual ~GameObject() = 0;
    // TODO
    virtual void init() = 0;
    virtual void update(double dt) = 0;
    virtual void draw(glm::mat4& vp) = 0;

    virtual void onCollision(ObjPtr other, glm::vec2 collisionPoint);

    glm::vec2 getOldCenter() const;
    glm::vec2 getCenter() const;
    float     getRadius() const;
    void      setCenter(glm::vec2);
    void      setRadius(float);
};

inline      GameObject::~GameObject() {}
inline void GameObject::onCollision(ObjPtr, glm::vec2) {}

typedef std::vector<ObjPtr>          ObjArray;
typedef std::function<std::optional<glm::vec2>(ObjPtr, ObjPtr)> CollisionTestFn;

#endif
