#include "quadtree.hpp"

Quadtree::NodePtr
Quadtree::build(glm::vec2 center, float halfWidth, int stopDepth)
{
    if(stopDepth < 0) return nullptr;

    auto ptr = std::make_shared<Node>();
    ptr->center = center;
    ptr->halfWidth = halfWidth;

    glm::vec2 offset;
    float step = halfWidth * 0.5f;
    for(int i = 0; i < 4; i++) {
        offset.x = ((i & 1) ? step : -step);
        offset.y = ((i & 2) ? step : -step);
        ptr->child[i] = Quadtree::build(center + offset, step, stopDepth - 1);
    }

    return ptr;
}

Quadtree::Quadtree(glm::vec2 center, float width)
{
    // Use a fixed max depth of 3.
    // A d-ary tree has (d^n - 1) / (d - 1) nodes. A quadtree with depth 3
    // gives 21 nodes in total, which is enough for our use case
    _root = Quadtree::build(center, width * 0.5f, 3);
}

void
Quadtree::insert(ObjPtr obj)
{
    _root->insert(obj);
}

void
Quadtree::Node::insert(ObjPtr obj)
{
    int index = 0;
    bool straddle = false;

    // Compute quadrant number [0..3] the object circle center is in.
    // If straddling any of the dividing x or y axes, exit directly
    for(int i = 0; i < 2; i++) {
        float delta = obj->getCenter()[i] - this->center[i];
        if(std::abs(delta) <= obj->getRadius()) {
            straddle = true;
            break;
        }
        if(delta > 0.0f) {
            index |= (1 << i); // YX
        }
    }

    if(!straddle && this->child[index]) {
        // Fully contained in existing child node, insert in subtree
        this->child[index]->insert(obj);
        return;
    }

    // Straddling, or no child node to descend into, so add object to this
    // node's list of objects
    this->objects.push_back(obj);
}

void
Quadtree::testAll(CollisionTestFn fn)
{
    this->testAll(fn, _root);
}

void
Quadtree::testAll(CollisionTestFn fn, Quadtree::NodePtr tree)
{
    static std::vector<NodePtr> ancestorStack;

    // Check collision between all objects on this level and all ancestor
    // objects. The current level is included as its own ancestor so all
    // necessary pairwise tests are done
    ancestorStack.push_back(tree);
    for(auto& ancestor : ancestorStack) {
        for(auto pA : ancestor->objects) {
            for(auto pB : tree->objects) {
                if(pA == pB) break;
                // TODO: Report collision result to these objects
                fn(pA, pB);
            }
        }
    }

    // Recursively visit all existing children
    for(int i = 0; i < 4; i++) {
        if(tree->child[i])
            testAll(fn, tree->child[i]);
    }

    // Remove current node from ancestor stack before returning
    ancestorStack.pop_back();
}
