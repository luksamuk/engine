#include "engine/components.hpp"
#include "engine/resources.hpp"
#include "engine/sound.hpp"

#include <flecs.h>
#include <iostream>

namespace Components
{
    void
    SoundEmitter::play(std::string name)
    {
        if(name == current) return;
        
        auto audio = table->load(name);
        if(audio != nullptr) {
            Sound::channelOf(chn)->play(audio);
        }
    }

    void
    RegisterSoundComponents(flecs::world &ecs)
    {
        ecs.system<SoundEmitter>("SoundEmitterUpdate")
            .each([](SoundEmitter &emitter) {
                if((emitter.current != "")
                   && !Sound::channelOf(emitter.chn)->isPlaying())
                    emitter.current = "";
            });
    }
}
