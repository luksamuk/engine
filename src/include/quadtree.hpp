#ifndef QUADTREE_HPP_INCLUDED
#define QUADTREE_HPP_INCLUDED

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <functional>
#include <optional>

#include "game_object.hpp"

class Quadtree
{
private:
    struct Node;
    typedef std::shared_ptr<Node> NodePtr;

    struct Node
    {
        glm::vec2 center;
        float     halfWidth;
        NodePtr   child[4];
        ObjArray  objects;

        void insert(ObjPtr obj);
    };
    
    NodePtr _root;

    static NodePtr build(glm::vec2 center, float halfWidth, int stopDepth);
    void testAll(CollisionTestFn fn, NodePtr node);
public:
    Quadtree(glm::vec2 center, float width);
    void insert(ObjPtr obj);
    void testAll(CollisionTestFn fn);
};

#endif

