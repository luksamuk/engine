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

namespace Tiled {
    struct TileData;
    struct TileMap;
    struct Level;
    struct LevelData;
    struct LevelDataManager;
}

namespace Sound {
    struct Audio;
    struct AudioSource;
    struct BGMTable;
}

namespace Resources
{   
    typedef std::shared_ptr<Render::Texture>         TexturePtr;
    typedef std::shared_ptr<Sprite::Animator>        AnimatorPtr;
    typedef std::shared_ptr<Sprite::Atlas>           AtlasPtr;
    typedef std::shared_ptr<Tiled::LevelDataManager> LevelDataManagerPtr;
    typedef std::shared_ptr<Tiled::TileData>         TileDataPtr;
    typedef std::shared_ptr<Tiled::TileMap>          TileMapPtr;
    typedef std::shared_ptr<Tiled::Level>            LevelPtr;
    typedef std::shared_ptr<Render::ShaderProgram>   ShaderProgramPtr;
    typedef std::shared_ptr<Sprite::Font>            FontPtr;
    typedef std::shared_ptr<Sound::Audio>            AudioPtr;
    typedef std::shared_ptr<Sound::AudioSource>      AudioSourcePtr;
    typedef std::shared_ptr<Sound::BGMTable>         BGMTablePtr;

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
        static void loadTileData(std::string path);
        static void loadTileMap(std::string path);
        static void loadAudio(std::string path, float loopend = -1.0f, float loopstart = -1.0f);
        static void loadBGMTable(std::string path);
        
        
        static TexturePtr          getTexture(std::string path);
        static AtlasPtr            getAtlas(std::string path);
        static AnimatorPtr         getAnimator(std::string path);
        static LevelDataManagerPtr getLevelDataManager(std::string path);
        static ShaderProgramPtr    getShaderProgram(std::string path);
        static FontPtr             getFont(std::string path);
        static TileDataPtr         getTileData(std::string path);
        static TileMapPtr          getTileMap(std::string path);
        static AudioPtr            getAudio(std::string path);
        static BGMTablePtr         getBGMTable(std::string path);
    };
}

#endif
