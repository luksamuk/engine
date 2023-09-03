#include "resources.hpp"
#include "sprite.hpp"
#include "render.hpp"
#include <toml++/toml.h>
#include <iostream>
#include <vector>

#include <map>

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
        LevelDataManagerPtr mng = std::make_shared<LevelDataManager>();
        toml::table tbl;
        try {
            tbl = toml::parse_file(path);
            auto levels_tbl = tbl["levels"].as_array();
            levels_tbl->for_each([&](auto&& el) {
                toml::table tb = *el.as_table();
                LevelData datum;
                datum.name = tb["name"].value_or("");
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

    const LevelData *
    LevelDataManager::getLevel(std::string name) const
    {
        for(unsigned i = 0; i < data.size(); i++)
        {
            if(data[i].name == name) {
                return &data[i];
            }
        }
        return nullptr;
    }

    static std::map<std::string, TexturePtr>          _textures;
    static std::map<std::string, AtlasPtr>            _atlases;
    static std::map<std::string, AnimatorPtr>         _animators;
    static std::map<std::string, LevelDataManagerPtr> _levelmanagers;
    static std::map<std::string, ShaderProgramPtr>    _shaderprograms;
    static std::map<std::string, FontPtr>             _fonts;
    
    void
    Manager::dispose()
    {
        _textures.clear();
        _atlases.clear();
        _animators.clear();
        _levelmanagers.clear();
        _shaderprograms.clear();
        _fonts.clear();
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
}
