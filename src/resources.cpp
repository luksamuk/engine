#include "resources.hpp"
#include <toml++/toml.h>
#include <iostream>
#include <vector>

Animator *
resourcesLoadAnimator(const char *path)
{
    toml::table tbl;

    try {
        tbl = toml::parse_file(path);

        glm::vec2 frame_size;

        auto atlas_path = *tbl["atlas"].value<std::string>();

        auto frame_size_arr = tbl["frame_size"].as_array();
        frame_size.x = frame_size_arr->at(0).as_floating_point()->get();
        frame_size.y = frame_size_arr->at(1).as_floating_point()->get();

        std::vector<AnimationData> data;
        auto animations_tbl = tbl["animations"].as_array();
        animations_tbl->for_each([&](auto&& el) {
            toml::table tb = *el.as_table();
            AnimationData datum;
            datum.name = tb["name"].value_or("");
            datum.frameDuration = tb["frame_duration"].value_or<float>(1.0);
            datum.minFrameIndex = tb["min_frame"].value_or<int>(-1);
            tb["frames"].as_array()->for_each([&](auto&& eln) {
                auto val = eln.value_or(0);
                datum.frames.push_back(val);
            });
            data.push_back(datum);
        });

        return new Animator(atlas_path.c_str(), frame_size, data);
        
    } catch(const toml::parse_error& err) {
        std::cerr << "Error loading animator config \""
                  << path
                  << "\": "
                  << err
                  << std::endl;
        return nullptr;
    }
}
