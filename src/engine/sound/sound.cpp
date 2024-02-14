#include "engine/sound.hpp"
#include <cstdint>
#include <cstring>
#include "engine/io.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <set>
#include <stack>
#include "engine/resources.hpp"

#include <vorbis/vorbisfile.h>

#define MAX_CHANNELS 32

// Global data
static ALCdevice                                       *_device    = nullptr;
static ALCcontext                                      *_context   = nullptr;
static bool                                             _shouldHalt = false;
static std::set<Sound::AudioSourcePtr>                  _looping;
static std::stack<Sound::AudioSourcePtr>                _addstack;
static std::stack<Sound::AudioSourcePtr>                _removestack;
static size_t                                           _newestChannel;
static Sound::AudioSourcePtr                            _channels[MAX_CHANNELS];
static std::stack<int>                                  _availableChannels;

void
Sound::init(void)
{
    _device = alcOpenDevice(nullptr);
    _context = alcCreateContext(_device, nullptr);
    alcMakeContextCurrent(_context);
    while(!_addstack.empty()) _addstack.pop();
    while(!_removestack.empty()) _removestack.pop();
    _looping.clear();
    std::fill_n(_channels, MAX_CHANNELS, nullptr);
    _newestChannel = 0;
    while(!_availableChannels.empty()) _availableChannels.pop();
}

void
Sound::dispose(void)
{
    while(!_addstack.empty()) _addstack.pop();
    while(!_removestack.empty()) _removestack.pop();
    _looping.clear();
    std::fill_n(_channels, MAX_CHANNELS, nullptr);
    _newestChannel = 0;
    while(!_availableChannels.empty()) _availableChannels.pop();
    alcDestroyContext(_context);
    alcCloseDevice(_device);
    _context    = nullptr;
    _device     = nullptr;
    _shouldHalt = false;
}

void
Sound::loop()
{
    // TODO
    while(!_shouldHalt) {
        while(!_addstack.empty()) {
            auto src = _addstack.top();
            _addstack.pop();
            _looping.insert(src);
        }

        while(!_removestack.empty()) {
            auto target = _removestack.top();
            _removestack.pop();
            auto itr = _looping.find(target);
            if(itr != _looping.end())
                _looping.erase(itr);
        }

        for(auto src : _looping) {
            if(src->current != nullptr
               && src->current->loops
               && src->current->loopend > 0.0f) {
                if(src->getElapsedTime() >= src->current->loopend)
                    src->setElapsedTime(
                        (src->current->loopstart > 0.0f)
                        ? src->current->loopstart
                        : 0.0f);
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void
Sound::halt(void)
{
    _shouldHalt = true;
}



// 32kb of buffer data
#define OGG_BUFFER_SIZE 32768

struct oggfile
{
    uint8_t *curptr;
    uint8_t *fileptr;
    size_t  filesize;
};

Resources::AudioPtr
Sound::rawLoadAudio(const char *path, bool loops, float loopend, float loopstart)
{
    auto rawdata = IO::slurp_file(path);
    Resources::AudioPtr audio;
    vorbis_info    *info;
    OggVorbis_File *file = new OggVorbis_File;
    ov_callbacks    callbacks;
    oggfile        ogf;

    ogf.curptr = ogf.fileptr = (uint8_t*)rawdata.c_str();
    ogf.filesize = rawdata.size();
    memset(file, 0, sizeof(OggVorbis_File));

    callbacks.read_func = [](void* dst, size_t size1, size_t size2, void* fh) -> size_t
        {
            oggfile* of = reinterpret_cast<oggfile*>(fh);
            size_t len = size1 * size2;
            if (of->curptr + len > of->fileptr + of->filesize) {
                len = of->fileptr + of->filesize - of->curptr;
            }
            memcpy(dst, of->curptr, len);
            of->curptr += len;
            return len;
        };

    callbacks.seek_func = [](void *fh, ogg_int64_t to, int type) -> int
        {
            oggfile* of = reinterpret_cast<oggfile*>(fh);

            switch (type)
            {
            case SEEK_CUR:
                of->curptr += to;
                break;
            case SEEK_END:
                of->curptr = of->fileptr + of->filesize - to;
                break;
            case SEEK_SET:
                of->curptr = of->fileptr + to;
                break;
            default:
                return -1;
            }
            if (of->curptr < of->fileptr) {
                of->curptr = of->fileptr;
                return -1;
            }
            if (of->curptr > of->fileptr + of->filesize) {
                of->curptr = of->fileptr + of->filesize;
                return -1;
            }
            return 0;
        };

    callbacks.close_func = [](void*) -> int { return 0; };

    callbacks.tell_func = [](void *fh) -> long {
        oggfile* of = reinterpret_cast<oggfile*>(fh);
        return (of->curptr - of->fileptr);
    };

    if(int ret = ov_open_callbacks((void*)&ogf, file, nullptr, -1, callbacks) < 0) {
        switch(ret) {
        case OV_EREAD:
            std::cerr << "Ogg Vorbis: Error trying to read media" << std::endl;
            break;
        case OV_ENOTVORBIS:
            std::cerr << "Ogg Vorbis: Media does not contain Vorbis data" << std::endl;
            break;
        case OV_EVERSION:
            std::cerr << "Ogg Vorbis: Vorbis version mismatch" << std::endl;
            break;
        case OV_EBADHEADER:
            std::cerr << "Ogg Vorbis: Invalid Vorbis bitstream header" << std::endl;
            break;
        case OV_EFAULT:
            std::cerr << "Ogg Vorbis: Internal error" << std::endl;
            break;
        default:
            std::cerr << "Ogg Vorbis: Unknown error" << std::endl;
            break;
        }

        return nullptr;
    }

    audio = std::make_shared<Audio>();
    info = ov_info(file, -1);
    audio->format = (info->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    audio->frequency = info->rate;
    audio->loops = loops;
    audio->loopstart = loopstart;
    audio->loopend = loopend;

    int bitstream;
    long bytes;
    char smallbuffer[OGG_BUFFER_SIZE];
    std::vector<char> buffer;
    do {
        bytes = ov_read(file, smallbuffer, OGG_BUFFER_SIZE, 0, 2, 1, &bitstream);
        if(bytes < 0) {
            ov_clear(file);
            delete file;
            std::cerr << "Ogg Vorbis: Unable to decode" << std::endl;
            return nullptr;
        }
        buffer.insert(buffer.end(), smallbuffer, smallbuffer + bytes);
    } while(bytes > 0);

    ov_clear(file);
    delete file;

    alGenBuffers(1, &audio->buffer);
    alBufferData(audio->buffer,
                 audio->format,
                 &buffer[0],
                 static_cast<ALsizei>(buffer.size()),
                 audio->frequency);
    
    return audio;
}

Sound::Audio::~Audio()
{
    if(buffer != (ALuint)-1) {
        alDeleteBuffers(1, &buffer);
    }
}


// ================= AudioSource ======================

Sound::AudioSourcePtr
Sound::makeSource()
{
    auto src = std::make_shared<AudioSource>();
    alGenSources(1, &src->source);
    src->position = glm::vec3(0.0f, 0.0f, 0.0f);
    src->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    src->gain     = 1u;
    src->pitch    = 1u;
    src->current  = nullptr;
    return src;
}

Sound::AudioSource::~AudioSource()
{
    this->stop();
    if(source != (ALuint)-1) {
        alDeleteSources(1, &source);
    }
}

void
Sound::AudioSource::play(Resources::AudioPtr audio)
{
    alSourcei(source, AL_BUFFER, audio->buffer);
    alSourcei(source, AL_GAIN, this->gain);
    alSourcei(source, AL_LOOPING, audio->loops);
    alSourcePlay(source);
    current = audio;

    // TODO: Does this work as intended?
    if((current != nullptr) && (managedIdx >= 0))
        _addstack.push(Sound::channelOf(managedIdx));
}

void
Sound::AudioSource::play(void)
{
    if(current) alSourcePlay(source);
}

void
Sound::AudioSource::stop(void)
{
    alSourceStop(source);
    // TODO: Does this work as intended?
    if((current != nullptr) && (managedIdx >= 0))
        _removestack.push(Sound::channelOf(managedIdx));
    current = nullptr;
}

void
Sound::AudioSource::pause(void)
{
    alSourcePause(source);
}

void
Sound::AudioSource::rewind(void)
{
    alSourceRewind(source);
}

float
Sound::AudioSource::getElapsedTime()
{
    float seconds;
    alGetSourcef(source, AL_SEC_OFFSET, &seconds);
    return seconds;
}

void
Sound::AudioSource::setElapsedTime(float seconds)
{
    alSourcef(source, AL_SEC_OFFSET, seconds);
}

bool
Sound::AudioSource::isPlaying() const
{
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}

// ==================== Audio Table (Info only) ======================

Resources::AudioPtr
Sound::BGMTable::load(std::string key)
{
    auto it = this->entries.find(key);
    if(it == this->entries.end()) return nullptr;
    Resources::Manager::loadAudio(
        it->second.file,
        it->second.loopend.value_or(-1.0f),
        it->second.loopstart.value_or(-1.0f));
    return Resources::Manager::getAudio(it->second.file);
}

// =================== Managed channels ==============================

Sound::AudioSourceIndex
Sound::getChannel()
{
    int idx;
    if(!_availableChannels.empty()) {
        idx = _availableChannels.top();
        _availableChannels.pop();
        goto makechannel;
    }

    if(_newestChannel >= MAX_CHANNELS) return -1;

    idx = _newestChannel;
    _newestChannel++;
    
makechannel:
    _channels[idx] = Sound::makeSource();
    _channels[idx]->managedIdx = idx;
    return idx;
}


void
Sound::releaseChannel(Sound::AudioSourceIndex channel)
{
    if(channel < 0 || channel >= MAX_CHANNELS)
        return;

    _channels[channel]->stop();
    _channels[channel] = nullptr;
    _availableChannels.push(channel);
}

Sound::AudioSourcePtr
Sound::channelOf(Sound::AudioSourceIndex channel)
{
    if(channel < 0 || channel >= MAX_CHANNELS)
        return nullptr;

    return _channels[channel];
}
