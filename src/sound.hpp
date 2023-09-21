#ifndef SOUND_HPP_INCLUDED
#define SOUND_HPP_INCLUDED

#include <AL/al.h>
#include <AL/alc.h>
#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include <map>
#include "resources.hpp"

namespace Sound
{
    struct Audio
    {
        ALuint  buffer      = -1;
        ALenum  format;
        ALsizei frequency;
        ALuint  buffersize;
        bool    loops;
        float   loopstart;
        float   loopend;

        ~Audio();
    };
    
    struct AudioSource
    {
        ALuint    source    = -1;
        ALuint    pitch;
        ALuint    gain;
        glm::vec3 position;
        glm::vec3 velocity;
        
        Resources::AudioPtr  current = nullptr;

        ~AudioSource();
        void play(Resources::AudioPtr);
        void play(void);
        void stop(void);
        void pause(void);
        void rewind(void);
    };

    typedef std::shared_ptr<AudioSource> AudioSourcePtr;

    struct BGMTableEntry
    {
        std::string file;
        std::optional<float> loopstart;
        std::optional<float> loopend;
    };
    
    struct BGMTable
    {
        std::map<std::string, BGMTableEntry> entries;

        Resources::AudioPtr load(std::string key);
    };

    void init(void);
    void dispose(void);
    void loop();
    void halt(void);

    Resources::AudioPtr rawLoadAudio(const char*, bool, float, float);
    AudioSourcePtr      makeSource();
}

#endif
