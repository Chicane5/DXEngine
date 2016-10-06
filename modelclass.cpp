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

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename, char* modelFilename)
{
	auto result = false;

	//load in the model data
	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}

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

	/*
	well no longer manually set the vertex and index count here - we'll read it from the file
	m_vertexCount = 4;
	m_indexCount = 6;
	*/

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

	//loading the vertex arrays has changed a bit - instead of setting the values manually we loop thru all the elements in the 
	//new m_model vector and copy that data into vertex array. The index array is easy to build since each vertex we load has the same index number
	//as the position in the array it was loaded into

	//load the vertex array and index array with data
	for (auto i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}
	
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

//model loading function
bool ModelClass::LoadModel(char* filename)
{
	std::ifstream fin;
	char input;
	
	//open the file
	fin.open(filename);

	//if it cant be opened then bail
	if (fin.fail())
	{
		return false;
	}

	//read up to the value of vertex count
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	//read the vertex count
	fin >> m_vertexCount;
	m_indexCount = m_vertexCount;



	// Read up to the beginning of the data.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	m_model.clear();

	//read in the vertex data
	for (auto i = 0; i < m_vertexCount; i++)
	{
		ModelType lTempModel;
		fin >> lTempModel.x >> lTempModel.y >> lTempModel.z;
		fin >> lTempModel.tu >> lTempModel.tv;
		fin >> lTempModel.nx >> lTempModel.ny >> lTempModel.nz;

		m_model.push_back(lTempModel);
	}

	// Close the model file.
	fin.close();

	return true;
	
}

void ModelClass::ReleaseModel()
{

	m_model.clear();
	return;
}