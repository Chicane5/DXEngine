#pragma once

////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "d3dclass.h"
#include "modelclass.h"
#include "textureshaderclass.h"
#include "cameraclass.h"
#include "lightshaderclass.h"
#include "lightclass.h"
#include <memory>

/////////////
// GLOBALS //
/////////////

const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;
const bool FULL_SCREEN = false;



////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
	std::shared_ptr<CameraClass> GetCamera();

private:
	bool Render(float);

private:
	std::shared_ptr<D3DClass> m_D3D;
	std::shared_ptr<ModelClass> m_Model;
	std::shared_ptr<TextureShaderClass> m_TextureShader;
	std::shared_ptr<CameraClass> m_Camera;
	std::shared_ptr<LightShaderClass> m_LightShader;
	std::shared_ptr<LightClass> m_Light;


};

#endif