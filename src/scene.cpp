#include "scene.hpp"

#include <stack>
#include <algorithm>

namespace Scenes
{
    static Manager *_manager = nullptr;

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
            s->load();
            // TODO: is this safe? Should we queue for addition?
            Manager::get()._scenes.push_back(s);
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
        for(auto scene : Manager::get()._scenes) {
            if(scene->getShouldUnload())
                shouldRemove.push(scene);
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
