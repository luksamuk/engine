#ifndef RESOURCES_HPP_INCLUDED
#define RESOURCES_HPP_INCLUDED

#include "sprite.hpp"
#include <memory>

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

typedef std::shared_ptr<Animator> AnimatorPtr;
typedef std::shared_ptr<LevelDataManager> LevelDataManagerPtr;

AnimatorPtr         resourcesLoadAnimator(const char *path);
LevelDataManagerPtr resourcesLoadLevelDataManager(const char *path);



#endif
