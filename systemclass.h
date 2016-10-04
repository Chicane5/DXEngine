#pragma once
////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_


///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define WIN32_LEAN_AND_MEAN

//////////////
// INCLUDES //
//////////////
#include <windows.h>
#include <memory>

#include "inputclass.h"
#include "graphicsclass.h"


class SystemClass {
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	std::unique_ptr<InputClass> m_Input;
	std::unique_ptr<GraphicsClass> m_Graphics;


};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SystemClass* ApplicationHandle(nullptr);

#endif