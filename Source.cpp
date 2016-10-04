#include <memory>
#include "systemclass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
	
	auto result = false;

	//create the system class
	std::unique_ptr<SystemClass> System(new SystemClass());

	if (System == nullptr) 
	{
		return 0;
	}

	//init the system 
	result = System->Initialize();
	if (result) 
	{
		System->Run();
	}

	//Shutdown & release
	System->Shutdown();

	return 0;
}

