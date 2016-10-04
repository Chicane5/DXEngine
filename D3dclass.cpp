////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "d3dclass.h"
#include <fstream>


D3DClass::D3DClass()
: m_swapChain(nullptr)
,m_device(nullptr)
,m_deviceContext(nullptr)
,m_renderTargetView(nullptr)
,m_depthStencilBuffer(nullptr)
,m_depthStencilState(nullptr)
,m_depthStencilView(nullptr)
,m_rasterState(nullptr)
{

}

D3DClass::D3DClass(const D3DClass& other)
{
}


D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen,
	float screenDepth, float screenNear)
{
	HRESULT result;
	std::shared_ptr<IDXGIFactory> factory;
	std::shared_ptr<IDXGIAdapter> adapter;
	std::shared_ptr<IDXGIOutput> adapterOutput;
	UINT numModes, numerator, denominator;
	size_t stringLength;
	std::unique_ptr<DXGI_MODE_DESC[]> displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	std::shared_ptr<ID3D11Texture2D> backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;


	// Store the vsync setting.
	m_vsync_enabled = vsync;

	// Create a DirectX graphics interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	// Use the factory to create an adapter for the primary graphics interface (video card).
	result = factory->EnumAdapters(0, (IDXGIAdapter**)&adapter);
	if (FAILED(result))
	{
		return false;
	}

	// Enumerate the primary adapter output (monitor).
	result = adapter->EnumOutputs(0, (IDXGIOutput**)&adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList.reset(new DXGI_MODE_DESC[numModes]);
	if (!displayModeList)
	{
		return false;
	}

	// Now fill the display mode list structures
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList.get());
	if (FAILED(result))
	{
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for (UINT i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (UINT)screenWidth)
		{
			if (displayModeList[i].Height == (UINT)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Get the adapter (video card) description.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);


	// Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	//write to file
	std::ofstream ofs("VideoCard.txt");
	ofs << m_videoCardDescription << std::endl;
	ofs << m_videoCardMemory << std::endl;

	ofs.close();

	// Release the display mode list
	// Release the adapter output.
	// Release the adapter.
	// Release the factory.

	//no need to release the above structures - smart pointer semantics innit

	//first things we'll do is fill out description of swap chain. Swap chain is the front and back buffer - do all drawing
	//to back buffer, then swap it to the front buffer

	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	
	//set it to a single back buffer
	swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//next part is refresh rate - how many times a second it draws the back buffer to the front buffer.
	//if vsync it will lock the rate to the system settings (60). Otherwise will draw as fast as it can.

	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	//set the usage of the back buffer
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = hwnd;

	// Turn multi sampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	/*
	Now that the swap chain description and feature level have been filled out we can create the swap chain, the Direct3D device, and the Direct3D device context. 
	The Direct3D device and Direct3D device context are very important, they are the interface to all of the Direct3D functions. 
	We will use the device and device context for almost everything from this point forward.
	*/

	// Create the swap chain, Direct3D device, and Direct3D device context

	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 
											nullptr,
											0,
											&featureLevel,
											1,
											D3D11_SDK_VERSION,
											&swapChainDesc,
											(IDXGISwapChain**)&m_swapChain,
											(ID3D11Device**)&m_device,
											nullptr,
											(ID3D11DeviceContext**)&m_deviceContext);

	if (FAILED(result))
	{
		return false;
	}

	// Get the pointer to the back buffer.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// Create the render target view with the back buffer pointer.
	result = m_device->CreateRenderTargetView(backBufferPtr.get(), NULL, (ID3D11RenderTargetView**)&m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	/*
	We will also need to set up a depth buffer description.
	We'll use this to create a depth buffer so that our polygons can be rendered properly in 3D space. 
	At the same time we will attach a stencil buffer to our depth buffer. The stencil buffer can be used to achieve effects
	such as motion blur, volumetric shadows, and other things.
	*/

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	/*
	Now we create the depth / stencil buffer using that description.
	You will notice we use the CreateTexture2D function to make the buffers, hence the buffer is just a 2D texture.
	The reason for this is that once your polygons are sorted and then rasterized they just end up being colored pixels in this 2D buffer.
	Then this 2D buffer is drawn to the screen.
	*/

	// Create the texture for the depth buffer using the filled out description.
	result = m_device->CreateTexture2D(&depthBufferDesc, nullptr, (ID3D11Texture2D**)&m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Now we need to setup the depth stencil description. This allows us to control what type of depth test Direct3D will do for each pixel.
	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	result = m_device->CreateDepthStencilState(&depthStencilDesc, (ID3D11DepthStencilState**)&m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	//set DSS, with the device context
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState.get(), 1);

	//now need to create description of the view of the depth stencil buffer. We do this so
	//Direct3D knows to use the depth buffer as a depth stencil texture After filling out the descripton we then 
	//call the func CreateDepthStencilView to create it

	//init the depth stencil view
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));


	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer.get(), &depthStencilViewDesc, (ID3D11DepthStencilView**)&m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	/*
	With that created we can now call OMSetRenderTargets. This will bind the render target view and the depth stencil buffer to the output render pipeline. 
	This way the graphics that the pipeline renders will get drawn to our back buffer that we previously created. With the graphics written to the back buffer,
	we can then swap it to the front and display our graphics on the user's screen.
	*/

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	m_deviceContext->OMSetRenderTargets(1, (ID3D11RenderTargetView**)&m_renderTargetView, m_depthStencilView.get());

	/*
	Now that the render targets are setup we can continue on to some extra functions that will give us more control over our scenes for future tutorials. 
	First thing is we'll create is a rasterizer state. This will give us control over how polygons are rendered. We can do things like make our scenes render in 
	wireframe mode or have DirectX draw both the front and back faces of polygons. By default DirectX already has a rasterizer state set up and working the exact 
	same as the one below but you have no control to change it unless you set up one yourself.
	*/

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = m_device->CreateRasterizerState(&rasterDesc, (ID3D11RasterizerState**)&m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	// Now set the rasterizer state.
	m_deviceContext->RSSetState(m_rasterState.get());

	//The viewport also needs to be setup so that Direct3D can map clip space coordinates to the render target space.Set this to be the entire size of the window.

	//setup the viewport for rendering
	viewport.Width = (float)screenWidth;
	viewport.Height = float(screenHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	m_deviceContext->RSSetViewports(1, &viewport);

	//Now we will create the projection matrix.The projection matrix is used to translate the 3D scene into the 2D viewport space that we previously created.
	//We will need to keep a copy of this matrix so that we can pass it to our shaders that will be used to render our scenes.

	//setup the projection matrix
	fieldOfView = (float)DirectX::XM_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	//create the projection matrix for 3d rendering
	DirectX::XMMATRIX lmatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
	DirectX::XMStoreFloat4x4(&m_projectionMatrix, lmatrix);

	/*
	We will also create another matrix called the world matrix. This matrix is used to convert the vertices of our objects into vertices in the 3D scene.
	This matrix will also be used to rotate, translate, and scale our objects in 3D space. From the start we will just initialize the matrix to the identity matrix 
	and keep a copy of it in this object. The copy will be needed to be passed to the shaders for rendering also.
	*/

	//init the world matrix to the identity matrix
	DirectX::XMMATRIX lworldMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMStoreFloat4x4(&m_worldMatrix, lworldMatrix);

	//This is where you would generally create a view matrix. The view matrix is used to calculate the position of where we are looking at the scene from (think of as camera)
	//we'll create it in a special camera class

	//finally we'll create orthographic projection matrix. Used for rendering 2d elements like UI allowing us to skip 3d rendering.

	DirectX::XMMATRIX lorthoMatrix = DirectX::XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);
	DirectX::XMStoreFloat4x4(&m_orthoMatrix, lorthoMatrix);

	return true;
}

void D3DClass::Shutdown()
{
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		//m_rasterState = 0;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		//m_depthStencilView = 0;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		//m_depthStencilState = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		//m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		//m_renderTargetView = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		//m_deviceContext = 0;
	}

	if (m_device)
	{
		m_device->Release();
		//m_device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		//m_swapChain = 0;
	}

	return;
	
}


void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView.get(), color);

	// Clear the depth buffer.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView.get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DClass::EndScene()
{
	// Present the back buffer to the screen since rendering is complete.
	if (m_vsync_enabled)
	{
		// Lock to screen refresh rate.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		m_swapChain->Present(0, 0);
	}

	return;
}

std::shared_ptr<ID3D11Device> D3DClass::GetDevice()
{
	return m_device;
}


std::shared_ptr<ID3D11DeviceContext> D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

void D3DClass::GetProjectionMatrix(DirectX::XMFLOAT4X4& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}


void D3DClass::GetWorldMatrix(DirectX::XMFLOAT4X4& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}


void D3DClass::GetOrthoMatrix(DirectX::XMFLOAT4X4& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}