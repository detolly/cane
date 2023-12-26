#pragma once

#include <Utility/Singleton.h>

class RendererOptions final : public Singleton<RendererOptions> {

public:
    void render();
    void on_load();
    void on_close();

};