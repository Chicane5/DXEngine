#pragma once

////////////////////////////////////////////////////////////////////////////////
// Filename: textureshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTURESHADERCLASS_H_
#define _TEXTURESHADERCLASS_H_



#pragma comment(lib, "D3DCompiler.lib")

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
#include <memory>

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: TextureShaderClass
////////////////////////////////////////////////////////////////////////////////
class TextureShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:
	TextureShaderClass();
	TextureShaderClass(const TextureShaderClass&);
	~TextureShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);


private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int);

	//utils
	void ConvertMatrixType(const DirectX::XMFLOAT4X4&, DirectX::XMMATRIX&);

private:
	std::shared_ptr<ID3D11VertexShader> m_vertexShader;
	std::shared_ptr<ID3D11PixelShader> m_pixelShader;
	std::shared_ptr<ID3D11InputLayout> m_layout;
	std::shared_ptr<ID3D11Buffer> m_matrixBuffer;

	std::shared_ptr<ID3D11SamplerState> m_sampleState;

};

#endif
