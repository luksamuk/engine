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
public:
    TestObject(int i);
    virtual ~TestObject();
    virtual void init() override;
    virtual void update(double dt) override;
    virtual void draw(glm::mat4& vp) override;

    int          getIdx() const;
    void         collide();
};

class PartitionTest : public Scenes::Scene
{
private:
    std::unique_ptr<Grid>       grid;
    std::shared_ptr<TestObject> obj[5];
    glm::mat4                   vp;
public:
    PartitionTest();
    virtual ~PartitionTest();
    virtual void load() override;
    virtual void unload() override;
    virtual void update(double dt) override;
    virtual void draw() override;
};

#endif
