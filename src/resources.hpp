#ifndef RESOURCES_HPP_INCLUDED
#define RESOURCES_HPP_INCLUDED

//#include "sprite.hpp"
#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace Sprite {
    class Atlas;
    class Animator;
    class Font;
}

namespace Render {
    class Texture;
    class ShaderProgram;
}

namespace Resources
{
    struct LevelData
    {
        std::string name;
        std::string atlas_path;
        std::string tiles_path;
        std::vector<std::string> maps_path;
    };

    struct LevelDataManager
    {
        std::vector<LevelData> data;

        const LevelData *getLevel(std::string name) const;
        // TODO: loadLevel
    };
    
    typedef std::shared_ptr<Render::Texture>       TexturePtr;
    typedef std::shared_ptr<Sprite::Animator>      AnimatorPtr;
    typedef std::shared_ptr<Sprite::Atlas>         AtlasPtr;
    typedef std::shared_ptr<LevelDataManager>      LevelDataManagerPtr;
    typedef std::shared_ptr<Render::ShaderProgram> ShaderProgramPtr;
    typedef std::shared_ptr<Sprite::Font>          FontPtr;

    class Manager
    {
    public:
        static void dispose();
        static void garbageCollect();
        
        static void loadTexture(std::string path);
        static void loadAtlas(std::string path, glm::vec2 framesize);
        static void loadAnimator(std::string path);
        static void loadLevelDataManager(std::string path);
        static void loadShaderProgram(std::string name, std::vector<const char *> shaders);
        static void loadFont(std::string path, glm::vec2 glyphsize);

        static TexturePtr          getTexture(std::string path);
        static AtlasPtr            getAtlas(std::string path);
        static AnimatorPtr         getAnimator(std::string path);
        static LevelDataManagerPtr getLevelDataManager(std::string path);
        static ShaderProgramPtr    getShaderProgram(std::string path);
        static FontPtr             getFont(std::string path);
    };

    //AnimatorPtr         loadAnimator(const char *path);
    //LevelDataManagerPtr loadLevelDataManager(const char *path);
}

#endif
