////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTSHADERCLASS_H_
#define _LIGHTSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////



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
class LightShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct LightBufferType
	{
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;  // Added extra padding so structure is a multiple of 16 for CreateBuffer function requirements.
			
	};

public:
	LightShaderClass();
	LightShaderClass(const LightShaderClass&);
	~LightShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);


private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);
	void RenderShader(ID3D11DeviceContext*, int);

	//utils
	void ConvertMatrixType(const DirectX::XMFLOAT4X4&, DirectX::XMMATRIX&);

private:
	std::shared_ptr<ID3D11VertexShader> m_vertexShader;
	std::shared_ptr<ID3D11PixelShader> m_pixelShader;
	std::shared_ptr<ID3D11InputLayout> m_layout;
	std::shared_ptr<ID3D11Buffer> m_matrixBuffer;
	std::shared_ptr<ID3D11SamplerState> m_sampleState;
	//There is a new private constant buffer for the light information (color and direction). The light buffer will be used by this class to set the global light variables inside the HLSL pixel shader.
	std::shared_ptr<ID3D11Buffer> m_lightBuffer;
};

#endif
