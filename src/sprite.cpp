#include "sprite.hpp"

#include <glm/ext.hpp>
#include <iostream>

namespace Sprite
{
    Atlas::Atlas(const char *texturePath, glm::vec2 framesize)
    {
        Resources::Manager::loadTexture(texturePath);
        Resources::Manager::loadShaderProgram(
            "spriteatlas", {
                "resources/shaders/spriteatlas/spriteatlas.vs.glsl",
                "resources/shaders/spriteatlas/spriteatlas.fs.glsl",
            });
        
        texture = Resources::Manager::getTexture(texturePath);
        program = Resources::Manager::getShaderProgram("spriteatlas");
        this->framesize = framesize;
        framesize_shader = framesize / this->texture->getSize();
        framesperline = glm::floor(texture->getSize().x / framesize.x);

        vbo = Render::make_vbo(Render::QuadGeometry::vertices(), Render::QuadGeometry::verticesSize(), GL_STATIC_DRAW);
        ebo = Render::make_ebo(Render::QuadGeometry::elements(), Render::QuadGeometry::elementsSize(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        vao = Render::make_vao();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        glBindVertexArray(vao);
        mvp_loc        = program->getUniformLocation("mvp");
        framecoord_loc = program->getUniformLocation("framecoord");
        framesize_loc  = program->getUniformLocation("framesize");
        tex_loc        = program->getUniformLocation("tex");
        vpos_loc       = program->getAttribLocation("vpos");

        glEnableVertexAttribArray(vpos_loc);
        glVertexAttribPointer(
            vpos_loc,
            Render::QuadGeometry::numVertexComponents(),
            GL_FLOAT,
            GL_FALSE,
            Render::QuadGeometry::singleVertexSize(),
            0);
    }

    Atlas::~Atlas()
    {
        //texture.dispose();
        //program.dispose();
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

    glm::vec2
    Atlas::getFramesize() const
    {
        return this->framesize;
    }

    void
    Atlas::setFrame(unsigned int frame)
    {
        this->frame = frame;
    }

    void
    Atlas::draw(glm::mat4& mvp)
    {
        // calculate framecoord
        framecoord.x = (float)frame;
        framecoord.y = glm::floor(framecoord.x / framesperline);
        framecoord.x = glm::mod(framecoord.x, framesperline);
    
        program->use();
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glActiveTexture(GL_TEXTURE0);
        texture->bind();
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform2fv(framecoord_loc, 1, glm::value_ptr(framecoord));
        glUniform2fv(framesize_loc, 1, glm::value_ptr(framesize_shader));
        glUniform1i(tex_loc, 0);
        glDrawElements(GL_TRIANGLES, Render::QuadGeometry::numElements(), GL_UNSIGNED_INT, 0);
    }


    Animator::Animator(const char *atlaspath, glm::vec2 framesize)
    {
        Resources::Manager::loadAtlas(atlaspath, framesize);
        atlas = Resources::Manager::getAtlas(atlaspath);
    }

    Animator::Animator(const char *atlaspath,
                       glm::vec2 framesize,
                       std::vector<AnimationData> data)
    {
        new (this) Animator(atlaspath, framesize);
        for(unsigned int i = 0; i < data.size(); i++)
            _data[i] = data[i];
    }

    Animator::~Animator()
    {
        //delete atlas;
    }

    void
    Animator::update()
    {
        if(!_currentData) return;
    
        double currentTime = glfwGetTime();
        if(_currentData->frames.size() > 1) {
            if(currentTime - _lastCheck > _frameDuration) {
                _lastCheck = currentTime;
                _iterFrame++;
                if(_iterFrame > (_currentData->frames.size() - 1))
                    _iterFrame = std::max(_currentData->minFrameIndex, 0);
            }
        }
    
        atlas->setFrame(_currentData->frames[_iterFrame]);
    }

    void
    Animator::draw(glm::mat4& mvp)
    {
        if(_currentData)
            atlas->draw(mvp);
    }

    void
    Animator::setAnimation(int index)
    {
        if(index < 0) {
            _currentAnimation = -1;
            _currentData = nullptr;
            _minFrame = 0;
            _iterFrame = 0;
            _frameDuration = _currentData->frameDuration;
            return;
        }

        if((index < (int)_data.size()) && (index != _currentAnimation)) {
            _currentAnimation = index;
            _currentData = &_data[index];
            _minFrame = _currentData->minFrameIndex;
            _frameDuration = _currentData->frameDuration;
            _iterFrame = 0;
        }
    }

    unsigned int
    Animator::getAnimation(void)
    {
        return _currentAnimation;
    }

    unsigned int
    Animator::numAnimations(void)
    {
        return _data.size();
    }


    Font::Font(const char *atlaspath, glm::vec2 glyphsize)
    {
        Resources::Manager::loadAtlas(atlaspath, glyphsize);
        atlas = Resources::Manager::getAtlas(atlaspath);
    }

    Font::~Font()
    {
        //delete atlas;
    }

    void
    Font::draw(glm::mat4& mvp, const char *text)
    {
        const char *ptr = text;
        int atlascoord;
        glm::vec2 pos(0.0f, 0.0f);
        glm::mat4 charmodel;
        glm::vec2 glyphsize = atlas->getFramesize();

    
        while(*ptr != '\0') {
            switch(*ptr) {
            case '\n':
                pos.x = 0.0f;
                pos.y += glyphsize.y;
                break;
            case '\r': break;
            default:
                atlascoord = (int)(*ptr) - 31;
                charmodel =  glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f));
                charmodel = glm::scale(charmodel, glm::vec3(glyphsize / 2.0f, 1.0f));
                charmodel = mvp * charmodel;
                atlas->setFrame(atlascoord);
                atlas->draw(charmodel);
                pos.x += glyphsize.x;
                break;
            }
            ptr++;
        }
    }

    void
    Animator::setAnimationByName(std::string name)
    {
        setAnimation(getAnimationByName(name));
    }

    unsigned int
    Animator::getAnimationByName(std::string name)
    {
        for(unsigned i = 0; i < _data.size(); i++) {
            if(_data[i].name == name) {
                return i;
            }
        }
        return 0;
    }

    glm::vec2
    Animator::getFramesize() const
    {
        return atlas->getFramesize();
    }
}
