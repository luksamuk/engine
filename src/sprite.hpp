#ifndef SPRITE_HPP_INCLUDED
#define SPRITE_HPP_INCLUDED

#include "render.hpp"
#include <map>
#include <glm/glm.hpp>
#include <string>
#include "resources.hpp"

namespace Sprite
{
    class Atlas
    {
    private:
        Resources::TexturePtr texture;
        unsigned int frame = 0;
        glm::vec2 framesize, framesize_shader, framecoord;
        float framesperline;
        GLuint mvp_loc, framecoord_loc, framesize_loc, tex_loc, vpos_loc;
        GLuint vao;
    
        // TODO: The following should be shared resources
        Resources::ShaderProgramPtr program;
        GLuint vbo, ebo;
    public:
        Atlas(const char *texturePath, glm::vec2 framesize);
        ~Atlas();
    
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
        Resources::AtlasPtr                     atlas;
        std::map<unsigned int, AnimationData>   _data;
        AnimationData                          *_currentData;
    
    public:
        Animator(const char *atlaspath, glm::vec2 framesize);
        Animator(const char *atlaspath, glm::vec2 framesize,
                 std::vector<AnimationData> data);
        ~Animator();
    
        void          setAnimation(int);
        void          setAnimationByName(std::string name);
        void          setFrameDuration(float);
        unsigned int  getAnimation(void);
        unsigned int  getAnimationByName(std::string name);
        glm::vec2     getFramesize() const;
        unsigned int  numAnimations(void);
        void          update();
        void          draw(glm::mat4& mvp);
    };

    class Font
    {
    private:
        Resources::AtlasPtr atlas;
    public:
        Font(const char *atlaspath, glm::vec2 glyphsize);
        ~Font();

        glm::vec2 getGlyphsize() const;
        glm::vec2 measure(const char *text);
        void      draw(glm::mat4& mvp, const char *text);
    };
}
    
#endif
