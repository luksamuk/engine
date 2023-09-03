#include "scene.hpp"

#include <stack>
#include <algorithm>
#include <queue>
#include "resources.hpp"

namespace Scenes
{
    static Manager *_manager = nullptr;
    static std::queue<Scene*> newScenes;

    void
    Scene::setShouldUnload(bool state)
    {
        this->shouldUnload = state;
    }

    bool
    Scene::getShouldUnload(void)
    {
        return this->shouldUnload;
    }

    void
    Manager::init(void)
    {
        // TODO
    }

    Manager&
    Manager::get()
    {
        if(!_manager) {
            _manager = new Manager();
            _manager->init();
        }
        return *_manager;
    }

    void
    Manager::add(Scene* s)
    {
        if(s) {
            // TODO: Queue for addition on updateAll
            //s->load();
            //Manager::get()._scenes.push_back(s);
            newScenes.push(s);
        }
    }

    void
    Manager::pop()
    {
        Manager::get()._scenes.pop_front();
    }

    void
    Manager::updateAll()
    {
        std::stack<Scene*> shouldRemove;
        bool shouldRunGC = false;
        for(auto scene : Manager::get()._scenes) {
            if(scene->getShouldUnload()) {
                shouldRemove.push(scene);
                shouldRunGC = true;
            }
            else scene->update();
        }

        while(!shouldRemove.empty()) {
            Scene *s = shouldRemove.top();
            shouldRemove.pop();
            s->unload();
        
            Manager::get()
                ._scenes
                .erase(
                    std::remove(
                        Manager::get()._scenes.begin(),
                        Manager::get()._scenes.end(),
                        s),
                    Manager::get()._scenes.end());

            delete s;
        }

        // load and add enqueued scenes
        while(!newScenes.empty()) {
            shouldRunGC = true;
            auto s = newScenes.front(); newScenes.pop();
            s->load();
            Manager::get()._scenes.push_back(s);
        }

        // At this point, if we added/removed a scene, run resource GC
        if(shouldRunGC) Resources::Manager::garbageCollect();
    }

    void
    Manager::drawAll()
    {
        for(auto scene : Manager::get()._scenes) {
            scene->draw();
        }
    }

    void
    Manager::unloadAll()
    {
        for(auto scene : Manager::get()._scenes) {
            scene->unload();
            delete scene;
        }

        Manager::get()._scenes.clear();
    }

    void
    Manager::dispose()
    {
        if(_manager) {
            Manager::unloadAll();
            delete _manager;
            _manager = nullptr;
        }
    }
}
