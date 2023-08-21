#include "scene.hpp"

#include <stack>
#include <algorithm>

static SceneManager *_manager = nullptr;

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
SceneManager::init(void)
{
    // TODO
}

SceneManager&
SceneManager::get()
{
    if(!_manager) {
        _manager = new SceneManager();
        _manager->init();
    }
    return *_manager;
}

void
SceneManager::add(Scene* s)
{
    if(s) {
        s->load();
        // TODO: is this safe? Should we queue for addition?
        SceneManager::get()._scenes.push_back(s);
    }
}

void
SceneManager::pop()
{
    SceneManager::get()._scenes.pop_front();
}

void
SceneManager::updateAll()
{
    std::stack<Scene*> shouldRemove;
    for(auto scene : SceneManager::get()._scenes) {
        if(scene->getShouldUnload())
            shouldRemove.push(scene);
        else scene->update();
    }

    while(!shouldRemove.empty()) {
        Scene *s = shouldRemove.top();
        shouldRemove.pop();
        s->unload();
        
        SceneManager::get()
            ._scenes
            .erase(
                std::remove(
                    SceneManager::get()._scenes.begin(),
                    SceneManager::get()._scenes.end(),
                    s),
                SceneManager::get()._scenes.end());

        delete s;
    }
}

void
SceneManager::drawAll()
{
    for(auto scene : SceneManager::get()._scenes) {
        scene->draw();
    }
}

void
SceneManager::unloadAll()
{
    for(auto scene : SceneManager::get()._scenes) {
        scene->unload();
        delete scene;
    }

    SceneManager::get()._scenes.clear();
}

void
SceneManager::dispose()
{
    if(_manager) {
        SceneManager::unloadAll();
        delete _manager;
        _manager = nullptr;
    }
}
