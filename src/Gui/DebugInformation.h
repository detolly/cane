#pragma once
#include <Utility/Singleton.h>

class Editor;
class DebugInformation : public Singleton<DebugInformation> {

public:
	void render();
	void on_load();
	void on_close();

private:
};

