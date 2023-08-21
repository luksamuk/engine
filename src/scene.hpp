#ifndef SCENE_HPP_INCLUDED
#define SCENE_HPP_INCLUDED

#include <queue>

class Scene
{
private:
    bool shouldUnload = false;
public:
    virtual ~Scene() {}
    virtual void load() = 0;
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual void unload() = 0;

    void setShouldUnload(bool);
    bool getShouldUnload(void);
};

class SceneManager
{
private:
    std::deque<Scene*> _scenes;
    
    SceneManager() {}
    void init();
public:
    static SceneManager& get();

    static void          add(Scene*);
    static void          pop();

    static void          updateAll();
    static void          drawAll();
    
    static void          unloadAll();
    
    static void          dispose();
};


#endif
