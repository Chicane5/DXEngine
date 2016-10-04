#pragma once

////////////////////////////////////////////////////////////////////////////////
// Filename: colorshaderclass.h
////////////////////////////////////////////////////////////////////////////////

//The colorshaderclass is what we will use to invoke our HLSL shaders for drawing the 3d models that are on the GPU

#ifndef _COLORSHADERCLASS_H_
#define _COLORSHADERCLASS_H_

#pragma comment(lib, "D3DCompiler.lib")

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Class name: ColorShaderClass
////////////////////////////////////////////////////////////////////////////////
class ColorShaderClass
{
private:

	//create definition of the cBuffer type to be used with vertex shader. This typedef must be exactly the same as the one in the 
	//vertex shader as the model data needs to match the typedefs in the shader for proper rendering
	struct MatrixBufferType
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};

public:
	ColorShaderClass();
	ColorShaderClass(const ColorShaderClass&);
	~ColorShaderClass();

//The functions here handle initializing and shutdown of the shader. The render function sets the shader parameters and then draws the prepared model vertices using the shader.

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, DirectX::XMFLOAT4X4, DirectX::XMFLOAT4X4, DirectX::XMFLOAT4X4);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, DirectX::XMFLOAT4X4, DirectX::XMFLOAT4X4, DirectX::XMFLOAT4X4);
	void RenderShader(ID3D11DeviceContext*, int);

	//utils
	void ConvertMatrixType(const DirectX::XMFLOAT4X4&, DirectX::XMMATRIX&);

private:
	std::shared_ptr<ID3D11VertexShader> m_vertexShader;
	std::shared_ptr<ID3D11PixelShader> m_pixelShader;
	std::shared_ptr<ID3D11InputLayout> m_layout;
	std::shared_ptr<ID3D11Buffer> m_matrixBuffer;

};

#endif
