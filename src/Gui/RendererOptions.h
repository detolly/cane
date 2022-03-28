//
// Created by Thomas on 1/21/2021.
//
#ifndef CANE_RENDEREROPTIONS_H
#define CANE_RENDEREROPTIONS_H

#include <Utility/Singleton.h>

class RendererOptions final : public Singleton<RendererOptions> {

public:
    void render();
    void on_load();
    void on_close();

};


#endif //CANE_RENDEREROPTIONS_H

