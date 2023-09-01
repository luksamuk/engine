#ifndef SPRITE_HPP_INCLUDED
#define SPRITE_HPP_INCLUDED

#include "render.hpp"
#include <map>
#include <glm/glm.hpp>
#include <string>


class SpriteAtlas
{
private:
    Texture texture;
    unsigned int frame = 0;
    glm::vec2 framesize, framesize_shader, framecoord;
    float framesperline;
    GLuint mvp_loc, framecoord_loc, framesize_loc, tex_loc, vpos_loc;
    GLuint vao;
    
    // TODO: The following should be shared resources
    ShaderProgram program;
    GLuint vbo, ebo;
public:
    SpriteAtlas(const char *texturePath, glm::vec2 framesize);
    ~SpriteAtlas();
    
    glm::vec2  getFramesize() const;
    void       setFrame(unsigned int);
    void       draw(glm::mat4& mvp);
};

typedef std::vector<unsigned int> FrameIndexArray;

struct AnimationData
{
    std::string     name;
    FrameIndexArray frames;
    float           frameDuration;
    int             minFrameIndex = -1;
};

class Animator
{
private:
    int          _currentAnimation = -1;
    double       _frameDuration    = 1.0f;
    double       _lastCheck        = 0.0f;
    unsigned int _minFrame         = 0,
                 _iterFrame        = 0;
    SpriteAtlas                            *atlas;
    std::map<unsigned int, AnimationData>   _data;
    AnimationData                          *_currentData;
    
public:
    Animator(const char *atlaspath, glm::vec2 framesize);
    Animator(const char *atlaspath, glm::vec2 framesize,
             std::vector<AnimationData> data);
    ~Animator();
    
    void          setAnimation(int);
    unsigned int  getAnimation(void);
    unsigned int  numAnimations(void);
    void          update();
    void          draw(glm::mat4& mvp);
};

class SpriteFont
{
private:
    SpriteAtlas *atlas;
public:
    SpriteFont(const char *atlaspath, glm::vec2 glyphsize);
    ~SpriteFont();
    void draw(glm::mat4& mvp, const char *text);
};

#endif
