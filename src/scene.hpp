#ifndef SCENE_HPP_INCLUDED
#define SCENE_HPP_INCLUDED

#include <queue>

namespace Scenes
{
    class Scene
    {
    private:
        bool shouldUnload = false;
    public:
        virtual ~Scene() = 0;
        virtual void load() = 0;
        virtual void update() = 0;
        virtual void draw() = 0;
        virtual void unload() = 0;

        void setShouldUnload(bool);
        bool getShouldUnload(void);
    };

    inline Scene::~Scene() {}

    class Manager
    {
    private:
        std::deque<Scene*> _scenes;
    
        Manager() {}
        void init();
    public:
        static Manager&      get();

        static void          add(Scene*);
        static void          pop();

        static void          updateAll();
        static void          drawAll();
        static void          unloadAll();
    
        static void          dispose();
    };
}

#endif
