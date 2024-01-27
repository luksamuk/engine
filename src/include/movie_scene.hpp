#ifndef MOVIE_SCENE_HPP_INCLUDED
#define MOVIE_SCENE_HPP_INCLUDED

#include "engine/scene.hpp"
#include "engine/resources.hpp"
#include <glm/glm.hpp>

class MovieScene : public Scenes::Scene
{
private:
    Resources::AnimatorPtr movie;
    glm::mat4 mvp, text_mvp;
    Resources::FontPtr font;
public:
    MovieScene();
    virtual ~MovieScene();
    virtual void load() override;
    virtual void update(double dt) override;
    virtual void draw() override;
    virtual void unload() override;
};

#endif
