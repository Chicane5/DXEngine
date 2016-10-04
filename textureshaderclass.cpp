////////////////////////////////////////////////////////////////////////////////
// Filename: textureshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "textureshaderclass.h"


TextureShaderClass::TextureShaderClass()
	: m_vertexShader(nullptr)
	, m_pixelShader(nullptr)
	, m_layout(nullptr)
	, m_matrixBuffer(nullptr)
	, m_sampleState(nullptr)
{

}

TextureShaderClass::TextureShaderClass(const TextureShaderClass& other)
{
}


TextureShaderClass::~TextureShaderClass()
{
}

//The Initialize function will call the initialization function for the shaders.We pass in the name of the HLSL shader files

bool TextureShaderClass::Initialize(ID3D11Device * device, HWND hwnd)
{
	bool result;

	//init the vertex and pixel shaders
	result = this->InitializeShader(device, hwnd, L"TextureVS.hlsl", L"TexturePS.hlsl");
	if (!result)
	{
		return false;
	}
	return true;
}

//The Shutdown function will call the shutdown of the shader.

void TextureShaderClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

/*
The Render function now takes a new parameter called texture which is the pointer to the texture resource. 
This is then sent into the SetShaderParameters function so that the texture can be set in the shader and then used for rendering.
*/

bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	bool result;


	//set the shader params to use for rendering
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result)
	{
		return false;
	}

	//now render the prepared buffers with the shader
	this->RenderShader(deviceContext, indexCount);

	return true;

}

//One of the most important functions > InitializeShader(). Loads the shader files and makes it useable to DirectX and teh GPU. 
//Also setup of the layout and how the vertex buffer data is going to look on the graphics pipeline in the GPU. The layout will need to match
//the VertexType in the modelclass.h as well as the one defined in the vertex shader file.

bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;

	std::shared_ptr<ID3D10Blob> errorMessage(nullptr);
	std::shared_ptr<ID3D10Blob> vertexShaderBuffer(nullptr);
	std::shared_ptr<ID3D10Blob> pixelShaderBuffer(nullptr);
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	UINT numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	D3D11_SAMPLER_DESC samplerDesc;

	//here is where we compile the shader programs into buffers. We pass it the name of the file, the name of the shader, the shader version (5.0 in 11) and the buffer
	//to compile the shader into. If it fails, we'll get an error in the error message string. 

	//COMPILE THE VERTEX SHADER
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, (ID3D10Blob**)&vertexShaderBuffer, (ID3D10Blob**)&errorMessage);

	if (FAILED(result))
	{
		// If the shader failed to compile it should have written something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage.get(), hwnd, vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	//COMPILE THE PIXEL SHADER
	result = D3DCompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, (ID3D10Blob**)&pixelShaderBuffer, (ID3D10Blob**)&errorMessage);

	if (FAILED(result))
	{
		// If the shader failed to compile it should have written something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage.get(), hwnd, psFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	//Once the vertex shader and pixel shader code has successfully compiled into buffers we then use those buffers to create the shader objects themselves. 
	//We will use these pointers to interface with the vertex and pixel shader from this point forward.

	//create the vertex shader from the buffer
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, (ID3D11VertexShader**)&m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, (ID3D11PixelShader**)&m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	/*
	The input layout has changed as we now have a texture element instead of color. The first position element stays unchanged but the SemanticName and Format of the second element have been changed 
	to TEXCOORD and DXGI_FORMAT_R32G32_FLOAT. These two changes will now align this layout with our new VertexType in both the ModelClass definition and the typedefs in the shader files.
	*/

	//Create vertex input layout desctiption. Needs to match the VertexType struct in the model class and the HLSL
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//Once the layout description has been setup we can get the size of it and then create the input layout using the D3D device.
	//Also release the vertex and pixel shader buffers since they are no longer needed once the layout has been created.

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//create the vertex input layout
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), (ID3D11InputLayout**)&m_layout);

	if (FAILED(result))
	{
		return false;
	}

	//release the buffers
	vertexShaderBuffer->Release();
	pixelShaderBuffer->Release();

	//final thing to setup is the constant buffer. In teh vertex shader program we only have one cbuffer, so only need to setup one here so we can interface with the shader
	//the buffer usage needs to be set to dynamic since we'll be updating it each frame. The bind flags indicate it will be a constant buffer. The CPU access flags need to match up
	//with the usage so it is set to D3D11_CPU_ACCESS_WRITE

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//create the constant buffer pointer so we can access the vertex shader constant buffer from within this class
	result = device->CreateBuffer(&matrixBufferDesc, NULL, (ID3D11Buffer**)&m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	/*
	The sampler state description is setup here and can then be passed to the pixel shader. The most important element of texture sampler description is filter. 
	Filter will determine which pixels are used or combined to create final look on the poly face. Here we use D3D11_FILTER_MIN_MAG_MIP_LINEAR
	which is more expensive but gives best look - uses linear interp for 
	minification, magnification, and mip-level sampling.
	*/

	//create texture sampler state description
	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, (ID3D11SamplerState**)&m_sampleState);

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	ULONG bufferSize, i;
	std::ofstream fout;

	//get a pointer to the error message text buffer
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	//get length
	bufferSize = errorMessage->GetBufferSize();

	//open a file to write the error to
	fout.open("shader-error.txt");

	//write out
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	fout.close();

	errorMessage->Release();

	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

/*
SetShaderParameters function now takes in a pointer to a texture resource and then assigns it to the shader using the new texture resource pointer. 
Note that the texture has to be set before rendering of the buffer occurs.
*/

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	UINT bufferNumber;

	//Make sure to transpose matrices before sending them into the shader, this is a requirement for DirectX 11.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//Lock the m_matrixBuffer, set the new matrices inside it, and then unlock it.

	result = deviceContext->Map(m_matrixBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//get a pointer to the data in the constant buffer
	dataPtr = (MatrixBufferType*)mappedResource.pData;


	//copy the matrices into the constant buffer
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	//unlock the cbuffer
	deviceContext->Unmap(m_matrixBuffer.get(), 0);

	//now set the updated matrix in the HLSL vertex shader

	//set the postion of the constant buffer in the shader
	bufferNumber = 0;

	//finally set the cbuffer in the vertex shader with updated values
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, (ID3D11Buffer**)&m_matrixBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;

}

/*
RenderShader is the second function called in the Render function. SetShaderParameters is called before this to ensure the shader parameters are setup correctly.

The first step in this function is to set our input layout to active in the input assembler. This lets the GPU know the format of the data in the vertex buffer. 
The second step is to set the vertex shader and pixel shader we will be using to render this vertex buffer. 
Once the shaders are set we render the triangle by calling the DrawIndexed DirectX 11 function using the D3D device context. Once this function is called it will render the green triangle

*/

void TextureShaderClass::RenderShader(ID3D11DeviceContext * deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout.get());

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader.get(), NULL, 0);
	deviceContext->PSSetShader(m_pixelShader.get(), NULL, 0);

	//The RenderShader function has been changed to include setting the sample state in the pixel shader before rendering.
	deviceContext->PSSetSamplers(0, 1, (ID3D11SamplerState**)&m_sampleState);

	//draw tri
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;

}

void TextureShaderClass::ConvertMatrixType(const DirectX::XMFLOAT4X4 & inMatrix, DirectX::XMMATRIX & outMatrix)
{
	ZeroMemory(&outMatrix, sizeof(outMatrix));
	outMatrix = DirectX::XMLoadFloat4x4(&inMatrix);

}

void TextureShaderClass::ShutdownShader()
{
	// Release the matrix constant buffer.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		
	}

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		
	}

	// Release the pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
	
	}

	// Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
	}

	return;
}


