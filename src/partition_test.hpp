#ifndef PARTITION_TEST_INCLUDED_HPP
#define PARTITION_TEST_INCLUDED_HPP

#include "scene.hpp"
#include "partitioning.hpp"
#include "resources.hpp"
#include <memory>

class TestObject : public GameObject
{
private:
    glm::mat4            model;
    Resources::AtlasPtr  atlas;
    Resources::FontPtr   font;
    int                  idx;
    bool                 colliding;
    int                  collisionCount;
public:
    TestObject(int i);
    virtual ~TestObject();
    virtual void init() override;
    virtual void update(double dt) override;
    virtual void draw(glm::mat4& vp) override;

    virtual void onCollision(ObjPtr o, glm::vec2 p) override;

    int          getIdx() const;
    int          getCollisionCount() const;
};

class MouseHoverObject : public GameObject
{
public:
    MouseHoverObject();
    virtual ~MouseHoverObject();
    virtual void init() override;
    virtual void update(double dt) override;
    virtual void draw(glm::mat4&) override;

    virtual void onCollision(ObjPtr o, glm::vec2 p) override;
};

class PartitionTest : public Scenes::Scene
{
private:
    Resources::FontPtr          font;
    std::unique_ptr<Grid>       grid;
    ObjArray                    objs;
    glm::mat4                   vp;

    // ObjPtr                      mouseobj;
    void draw_debug_window();
public:
    PartitionTest();
    virtual ~PartitionTest();
    virtual void load() override;
    virtual void unload() override;
    virtual void update(double dt) override;
    virtual void draw() override;
};

#endif
