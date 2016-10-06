#pragma once

////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

//As stated previously the ModelClass is responsible for encapsulating the geometry for 3D models

#include <d3d11.h>
#include <DirectXMath.h>
#include "textureclass.h"
#include <memory>
#include <vector>
#include <fstream>

using namespace DirectX;

class ModelClass
{

private:

	//Here is the definition of our vertex type that will be used with the vertex buffer in this ModelClass. Also take note that this typedef must match the layout in the ColorShaderClass 

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	//structure representing 3d model type

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

//The functions here handle initializing and shutdown of the model's vertex and index buffers. The Render function puts the model geometry on the video card to prepare it for drawing by the color shader.

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*, char*); //adding filename for model to be loaded
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

	//model loading/unloading from text file
	bool LoadModel(char*);
	void ReleaseModel();

private:
	std::shared_ptr<ID3D11Buffer> m_vertexBuffer;
	std::shared_ptr<ID3D11Buffer> m_indexBuffer;
	int m_vertexCount;
	int m_indexCount;

	std::shared_ptr<TextureClass> m_Texture;
	std::vector<ModelType> m_model;



};


#endif
