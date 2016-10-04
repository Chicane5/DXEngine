#include "graphicsclass.h"



GraphicsClass::GraphicsClass()
	: m_D3D(nullptr)
	, m_Model(nullptr)
	, m_TextureShader(nullptr)
	, m_Camera(nullptr)
	, m_LightShader(nullptr)
	, m_Light(nullptr)
{
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	auto result = false;

	//create the Direct3D object
	m_D3D.reset(new D3DClass());
	if (!m_D3D)
	{
		return false;
	}

	//initialize direct3d object
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}



	//create the camera object
	m_Camera.reset(new CameraClass());
	if (!m_Camera)
	{
		return false;
	}

	//set the initial position of the camera
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
	m_Camera->SetRotation(0.0f, 0.0f, 0.0f);

	//create the model object
	m_Model.reset(new ModelClass());
	if (!m_Model)
	{
		return false;
	}

	//init the model object
	result = m_Model->Initialize(m_D3D->GetDevice().get(), m_D3D->GetDeviceContext().get(), "uv_checker.tga");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	/*

	// Create the texture shader object.
	m_TextureShader.reset(new TextureShaderClass());
	if (!m_TextureShader)
	{
		return false;
	}
	//init the texture shader object
	result = m_TextureShader->Initialize(m_D3D->GetDevice().get(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}
	*/

	// Create the light shader object.
	m_LightShader.reset(new LightShaderClass());
	if (!m_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader->Initialize(m_D3D->GetDevice().get(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	//The new light object is created here.

	// Create the light object.
	m_Light.reset(new LightClass());
	if (!m_Light)
	{
		return false;
	}

	//The color of the light is set to purple and the light direction is set to point down the positive Z axis.

	// Initialize the light object.
	m_Light->SetDiffuseColor(1.0f, 0.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the color shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
	}

	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
	}


	if (m_D3D)
	{
		m_D3D->Shutdown();
	}

	if (m_LightShader)
	{
		m_LightShader->Shutdown();
	}

	return;
}


bool GraphicsClass::Frame()
{
	//We add a new static variable to hold an updated rotation value each frame that will be passed into the Render function.
	static float rotation = 0.0f;

	//update the rotation variable each frame
	rotation += (float)DirectX::XM_PI * 0.01f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}
	
	

	auto result = false;
	//render the graphics scene
	result = Render(rotation);
	if (!result)
	{
		return false;
	}

	return true;
}


bool GraphicsClass::Render(float rotation)
{
	DirectX::XMFLOAT4X4  viewMatrix, projectionMatrix, worldMatrix;
	bool result;

	//clear the buffers to begin the scene
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//generate the view matrix based on the camera's position
	m_Camera->Render();

	//get the world, view and projection matrices from the camera and d3d objects
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	//put the model vertex and index buffers on the graphics pipeline to prepare them for drawing
	m_Model->Render(m_D3D->GetDeviceContext().get());

	//render the model using the color shader
	/*
	result = m_ColorShader->Render(m_D3D->GetDeviceContext().get(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}
	*/

	//The texture shader is called now instead of the color shader to render the model.Notice it also takes the texture resource pointer from the model,
	//so the texture shader has access to the texture from the model object.

	// Render the model using the texture shader.
	XMMATRIX w;
	XMMATRIX v;
	XMMATRIX p;

	w = XMLoadFloat4x4(&worldMatrix);
	v = XMLoadFloat4x4(&viewMatrix);
	p = XMLoadFloat4x4(&projectionMatrix);

	//here we rotate the WORLD matrix by the rotation value so when we render the primitive using this updated world matrix it will spin it by the rot amount
	w = DirectX::XMMatrixMultiply(w, DirectX::XMMatrixRotationY(rotation));

	result = m_LightShader->Render(m_D3D->GetDeviceContext().get(), m_Model->GetIndexCount(), w, v, p, m_Model->GetTexture(),
		m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}


	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}