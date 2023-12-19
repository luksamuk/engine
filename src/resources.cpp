#include "resources.hpp"
#include "sprite.hpp"
#include "render.hpp"
#include "tiled.hpp"
#include "sound.hpp"
#include <toml++/toml.h>
#include <iostream>
#include <vector>
#include <map>
#include <stack>

namespace Resources
{
    AnimatorPtr
    rawLoadAnimator(const char *path)
    {
        toml::table tbl;

        try {
            tbl = toml::parse_file(path);

            glm::vec2 frame_size;

            auto atlas_path = *tbl["atlas"].value<std::string>();

            auto frame_size_arr = tbl["frame_size"].as_array();
            frame_size.x = frame_size_arr->at(0).as_floating_point()->get();
            frame_size.y = frame_size_arr->at(1).as_floating_point()->get();

            std::vector<Sprite::AnimationData> data;
            auto animations_tbl = tbl["animations"].as_array();
            animations_tbl->for_each([&](auto&& el) {
                toml::table tb = *el.as_table();
                Sprite::AnimationData datum;
                datum.name = tb["name"].value_or("");
                datum.frameDuration = tb["frame_duration"].value_or<float>(1.0);
                datum.minFrameIndex = tb["min_frame"].value_or<int>(-1);
                tb["frames"].as_array()->for_each([&](auto&& eln) {
                    auto val = eln.value_or(0);
                    datum.frames.push_back(val);
                });
                data.push_back(datum);
            });

            return std::make_shared<Sprite::Animator>(atlas_path.c_str(), frame_size, data);
        
        } catch(const toml::parse_error& err) {
            std::cerr << "Error loading animator config \""
                      << path
                      << "\": "
                      << err
                      << std::endl;
            return nullptr;
        }
    }

    LevelDataManagerPtr
    rawLoadLevelDataManager(const char *path)
    {
        LevelDataManagerPtr mng = std::make_shared<Tiled::LevelDataManager>();
        toml::table tbl;
        try {
            tbl = toml::parse_file(path);
            auto levels_tbl = tbl["levels"].as_array();
            levels_tbl->for_each([&](auto&& el) {
                toml::table tb = *el.as_table();
                Tiled::LevelData datum;
                datum.name = tb["name"].value_or("");
                datum.bgm  = tb["bgm"].value_or("00");
                datum.atlas_path = tb["atlas"].value_or("");
                datum.tiles_path = tb["tiles"].value_or("");
                auto maps_arr = tb["maps"].as_array();
                maps_arr->for_each([&](auto&& e) {
                    auto val = e.value_or("");
                    datum.maps_path.push_back(val);
                });
                mng->data.push_back(datum);
            });
        } catch(const toml::parse_error& err) {
            std::cerr << "Error loading level data manager \""
                      << path
                      << "\": "
                      << err
                      << std::endl;
            return nullptr;
        }
        return mng;
    }

    BGMTablePtr
    rawLoadBGMTable(const char *path)
    {
        BGMTablePtr ptr = std::make_shared<Sound::BGMTable>();
        toml::table tbl;
        try {
            tbl = toml::parse_file(path);
            tbl.for_each([&](const toml::key& key, auto&& val)
                {
                    Sound::BGMTableEntry entry;
                    auto tb = *val.as_table();
                    entry.file = tb["file"].value_or("");
                    entry.loopend = tb["loop-end"].value_or(-1.0f);
                    entry.loopstart = tb["loop-start"].value_or(-1.0f);
                    std::string bgmname(key.data());
                    ptr->entries[bgmname] = entry;
                });
        } catch(const toml::parse_error& err) {
            std::cerr << "Error loading BGM table \""
                      << path
                      << "\": "
                      << err
                      << std::endl;
            return nullptr;
        }
        return ptr;
    }

    static std::map<std::string, TexturePtr>          _textures;
    static std::map<std::string, AtlasPtr>            _atlases;
    static std::map<std::string, AnimatorPtr>         _animators;
    static std::map<std::string, LevelDataManagerPtr> _levelmanagers;
    static std::map<std::string, ShaderProgramPtr>    _shaderprograms;
    static std::map<std::string, FontPtr>             _fonts;
    static std::map<std::string, TileDataPtr>         _tiledata;
    static std::map<std::string, TileMapPtr>          _tilemap;
    static std::map<std::string, AudioPtr>            _audios;
    static std::map<std::string, BGMTablePtr>         _bgmtables;
    
    void
    Manager::dispose()
    {
        _textures.clear();
        _atlases.clear();
        _animators.clear();
        _levelmanagers.clear();
        _shaderprograms.clear();
        _fonts.clear();
        _tiledata.clear();
        _tilemap.clear();
        _bgmtables.clear();
        _audios.clear();
    }

    template<typename P>
    void
    _rawGC(std::map<std::string, std::shared_ptr<P>>& collection)
    {
        std::stack<std::string> garbage;
        for(auto& pair : collection) {
            if(pair.second.use_count() == 1)
                garbage.push(pair.first);
        }

        if(!garbage.empty()) {
            std::cout << "Removing " << garbage.size() << " garbage assets" << std::endl;
        }
        
        while(!garbage.empty()) {
            auto name = garbage.top(); garbage.pop();
            collection.erase(name);
        }
    }

    void
    Manager::garbageCollect()
    {
        std::cout << "Running resources garbage collector" << std::endl;
        _rawGC(_textures);
        _rawGC(_atlases);
        _rawGC(_animators);
        _rawGC(_levelmanagers);
        _rawGC(_shaderprograms);
        _rawGC(_fonts);
        _rawGC(_tiledata);
        _rawGC(_tilemap);
        _rawGC(_bgmtables);
        _rawGC(_audios);
    }
    
    void
    Manager::loadTexture(std::string path)
    {
        if(_textures.find(path) == _textures.end()) {
            std::cout << "Loading texture \"" << path << "\"" << std::endl;
            _textures[path] = TexturePtr(Render::Texture::load(path.c_str()));
        }
    }

    void
    Manager::loadAtlas(std::string path, glm::vec2 framesize)
    {
        if(_atlases.find(path) == _atlases.end()) {
            std::cout << "Loading atlas \"" << path << "\"" << std::endl;
            _atlases[path] = std::make_shared<Sprite::Atlas>(path.c_str(), framesize);
        }
    }
    
    void
    Manager::loadAnimator(std::string path)
    {
        if(_animators.find(path) == _animators.end()) {
            std::cout << "Loading animator \"" << path << "\"" << std::endl;
            _animators[path] = rawLoadAnimator(path.c_str());
        }
    }
    
    void
    Manager::loadLevelDataManager(std::string path)
    {
        if(_levelmanagers.find(path) == _levelmanagers.end()) {
            std::cout << "Loading level data manager \"" << path << "\"" << std::endl;
            _levelmanagers[path] = rawLoadLevelDataManager(path.c_str());
        }
    }

    void
    Manager::loadShaderProgram(std::string name, std::vector<const char *> shaders)
    {
        if(_shaderprograms.find(name) == _shaderprograms.end()) {
            std::cout << "Loading shader program \"" << name << "\"" << std::endl;
            _shaderprograms[name] = ShaderProgramPtr(Render::ShaderProgram::link(shaders));
        }
    }

    void
    Manager::loadFont(std::string path, glm::vec2 glyphsize)
    {
        if(_fonts.find(path) == _fonts.end()) {
            std::cout << "Loading font \"" << path << "\"" << std::endl;
            _fonts[path] = std::make_shared<Sprite::Font>(path.c_str(), glyphsize);
        }
    }

    void
    Manager::loadTileData(std::string path)
    {
        if(_tiledata.find(path) == _tiledata.end()) {
            std::cout << "Loading tile data \"" << path << '"' << std::endl;
            _tiledata[path] = std::make_shared<Tiled::TileData>(path.c_str());
        }
    }

    void
    Manager::loadTileMap(std::string path)
    {
        if(_tilemap.find(path) == _tilemap.end()) {
            std::cout << "Loading tile map \"" << path << '"' << std::endl;
            _tilemap[path] = std::make_shared<Tiled::TileMap>(path.c_str());
        }
    }

    void
    Manager::loadAudio(std::string path, float loopend, float loopstart)
    {
        if(_audios.find(path) == _audios.end()) {
            std::cout << "Loading audio \"" << path << '"' << std::endl;
            // An audio loops if there is a valid loop end
            auto audio = Sound::rawLoadAudio(
                path.c_str(),
                (loopend >= 0.0f),
                loopend,
                loopstart);
            if(audio != nullptr) _audios[path] = audio;
        }
    }

    void
    Manager::loadBGMTable(std::string path)
    {
        if(_bgmtables.find(path) == _bgmtables.end()) {
            std::cout << "Loading BGM table \"" << path << '"' << std::endl;
            auto table = rawLoadBGMTable(path.c_str());
            if(table != nullptr) _bgmtables[path] = table;
        }
    }

    

    TexturePtr
    Manager::getTexture(std::string path)
    {
        auto itr = _textures.find(path);
        return itr == _textures.end() ? nullptr : itr->second;
    }

    AtlasPtr
    Manager::getAtlas(std::string path)
    {
        auto itr = _atlases.find(path);
        return itr == _atlases.end() ? nullptr : itr->second;
    }

    AnimatorPtr
    Manager::makeAnimator(std::string path)
    {
        return rawLoadAnimator(path.c_str());
    }
    
    AnimatorPtr
    Manager::getAnimator(std::string path)
    {
        auto itr = _animators.find(path);
        return itr == _animators.end() ? nullptr : itr->second;
    }
    
    LevelDataManagerPtr
    Manager::getLevelDataManager(std::string path)
    {
        auto itr = _levelmanagers.find(path);
        return itr == _levelmanagers.end() ? nullptr : itr->second;
    }

    ShaderProgramPtr
    Manager::getShaderProgram(std::string path)
    {
        auto itr = _shaderprograms.find(path);
        return itr == _shaderprograms.end() ? nullptr : itr->second;
    }

    FontPtr
    Manager::getFont(std::string path)
    {
        auto itr = _fonts.find(path);
        return itr == _fonts.end() ? nullptr : itr->second;
    }

    TileDataPtr
    Manager::getTileData(std::string path)
    {
        auto itr = _tiledata.find(path);
        return itr == _tiledata.end() ? nullptr : itr->second;
    }

    TileMapPtr
    Manager::getTileMap(std::string path)
    {
        auto itr = _tilemap.find(path);
        return itr == _tilemap.end() ? nullptr : itr->second;
    }

    AudioPtr
    Manager::getAudio(std::string path)
    {
        auto itr = _audios.find(path);
        return itr == _audios.end() ? nullptr : itr->second;
    }

    BGMTablePtr
    Manager::getBGMTable(std::string path)
    {
        auto itr = _bgmtables.find(path);
        return itr == _bgmtables.end() ? nullptr : itr->second;
    }
}
