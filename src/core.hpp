#ifndef CORE_HPP_INCLUDED
#define CORE_HPP_INCLUDED

#include <string>

namespace Core
{
    void    init(std::string windowCaption);
    void    init(void);
    void    gameLoop(void);
    void    dispose(void);
    void    queryClose(void);
    void    setWindowCaptionAppendix(std::string appendix);
}

#endif
