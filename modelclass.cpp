#include "modelclass.h"

template< typename T >
struct array_deleter
{
	void operator ()(T const * p)
	{
		delete[] p;
	}
};

ModelClass::ModelClass()
	: m_vertexBuffer(nullptr)
	, m_indexBuffer(nullptr)
	, m_Texture(nullptr)
{

}

ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}

//The Initialize function will call the initialization functions for the vertex and index buffers.
//Initialize now takes as input the file name of the texture that the model will be using as well as the device context.

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename)
{
	auto result = false;

	//init the vertex and index buffer that will hold the geo for the triangle
	result = this->InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Release the vertex and index buffers.
	ShutdownBuffers();

	return;
}

//render is called from the Graphics Class render function. This function calls RenderBuffers() to put the vertex 
// and index buffers on the graphics pipeline so the color shader will be able to redner them

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	this->RenderBuffers(deviceContext);

	return;
}

//GetIndexCount returns the number of indexes in the model.The color shader will need this information to draw this model.

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}
/*
The InitializeBuffers function is where we handle creating the vertex and index buffers. Usually you would read in a model and create the buffers from that data file. 
For this tutorial we will just set the points in the vertex and index buffer manually since it is only a single triangle.
*/

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	std::unique_ptr<VertexType[]> vertices(nullptr);
	std::unique_ptr<ULONG[]> indices(nullptr);
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	//First create two temporary arrays to hold the vertex and index data that we will use later to populate the final buffers wit

	m_vertexCount = 4;
	m_indexCount = 6;

	//create the vertex array
	vertices.reset(new VertexType[m_vertexCount]);
	if (!vertices)
	{
		return false;
	}

	//create th indices array
	indices.reset(new ULONG[m_indexCount]);
	if (!indices)
	{
		return false;
	}

	//now fill both the index and vertex array with the 3 points of the triangle, and well as the index to each of the points. Note points are created in a clockwise order of drawing them
	//If yo9u do this counter clockwise, it will think the triangle is facing the opposite way and not draw it due to backface culling. REMEMBER the order you send vertices to the GPU is very 
	//important. Color is set too, since its part of the vertex description.

	//now has a texture coordinate component instead of a color. 
	//adding a normal comopnent for lighting

	//load the vertex array with data:
	// Load the vertex array with data.
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[0].normal = XMFLOAT3(0.0f, 0.0f, -1.0f); //normal facing viewer

	vertices[1].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);  // Top left.
	vertices[1].texture = XMFLOAT2(0.0f, 0.0f);
	vertices[0].normal = XMFLOAT3(0.0f, 0.0f, -1.0f); //normal facing viewer

	vertices[2].position = XMFLOAT3(1.0f, 1.0f, 0.0f);  // top right.
	vertices[2].texture = XMFLOAT2(1.0f, 0.0f);
	vertices[0].normal = XMFLOAT3(0.0f, 0.0f, -1.0f); //normal facing viewer

	vertices[3].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[3].texture = XMFLOAT2(1.0f, 1.0f);
	vertices[0].normal = XMFLOAT3(0.0f, 0.0f, -1.0f); //normal facing viewer


	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top left.
	indices[2] = 2;  // top right.

	indices[3] = 0;  // Bottom left.
	indices[4] = 2;  // Top right.
	indices[5] = 3;  // Bottom right.
	
	/*
	With the vertex array and index array filled out we can now use those to create the vertex buffer and index buffer.
	Creating both buffers is done in the same fashion.First fill out a description of the buffer.In the description the ByteWidth(size of the buffer) and the BindFlags(type of buffer) 
	are what you need to ensure are filled out correctly.After the description is filled out you need to also fill out a subresource pointer which will point to either your 
	vertex or index array you previously created.With the description and subresource pointer you can call CreateBuffer using the D3D device and it will return a pointer to your new buffer.
	*/

	//setup the description of the static vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//give the subresource structure a pointer to the vertex data (transfer ownership)
	vertexData.pSysMem = vertices.release();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//now create the vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, (ID3D11Buffer**)&m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(ULONG) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices.release();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//create the index buffer
	result = device->CreateBuffer(&indexBufferDesc, &indexData, (ID3D11Buffer**)&m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}


	return true;
}

void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
	}

	return;
}
/*
RenderBuffers is called from the render function. This sets the vertex buffer and index buffer as active on the input assembler on the GPU
Once the GPU has an active vertex buffer it can then use the shader to render that buffer. This function also defines how those buffers should
be drawn such as triangles, lines, fans and so forth. 
*/
void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	UINT stride;
	UINT offset;

	//set the vertex buffer stride and offset
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, (ID3D11Buffer**)&m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer.get(), DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

//LoadTexture is a new private function that will create the texture object and then initialize it with the input file name provided.This function is called during initialization.

bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;


	// Create the texture object.
	m_Texture.reset(new TextureClass());
	if (!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

//The ReleaseTexture function will release the texture object that was created and loaded during the LoadTexture function.

void ModelClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
	}

	return;
}