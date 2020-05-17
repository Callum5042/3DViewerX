#include "Sky.h"
#include "GeometryGenerator.h"
#include <DirectXMath.h>
#include "Application.h"
#include <DDSTextureLoader.h>
#include <fstream>
#include <SDL_messagebox.h>

_declspec(align(16)) struct ConstantBuffer
{
	DirectX::XMMATRIX mWorldViewProj;
};

Sky::Sky(Renderer* renderer) : m_Renderer(renderer)
{
}

void Sky::Load()
{
	m_Mesh = new Mesh();
	GeometryGenerator::CreateSphere(2.0f, 30, 30, m_Mesh);

	// Vertex duffer description
	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(SimpleVertex) * static_cast<UINT>(m_Mesh->vertices.size());
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vInitData = {};

	const SimpleVertex* vert = &m_Mesh->vertices[0];
	vInitData.pSysMem = vert;

	DX::ThrowIfFailed(m_Renderer->GetDevice()->CreateBuffer(&vbd, &vInitData, &m_VertexBuffer));

	// Index buffer description
	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(WORD) * static_cast<UINT>(m_Mesh->indices.size());
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA iInitData = {};
	const WORD* index = &m_Mesh->indices[0];
	iInitData.pSysMem = index;

	DX::ThrowIfFailed(m_Renderer->GetDevice()->CreateBuffer(&ibd, &iInitData, &m_IndexBuffer));

	// Set primitive topology
	m_Renderer->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	DX::ThrowIfFailed(m_Renderer->GetDevice()->CreateBuffer(&bd, nullptr, &m_ConstantBuffer));

	// Texture
	ID3D11Resource* texResource = nullptr;
	DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(m_Renderer->GetDevice(), L"D:\\3d models\\grasscube1024.dds", &texResource, &m_CubeMapSRV));
	texResource->Release();

	// Create shaders
	CreateVertexShader("D:\\Sources\\3DViewerX\\bin\\3DViewerX\\Debug-x64\\SkyVertexShader.cso");
	CreatePixelShader("D:\\Sources\\3DViewerX\\bin\\3DViewerX\\Debug-x64\\SkyPixelShader.cso");
}

void Sky::Render()
{
	D3D11_RASTERIZER_DESC rasterizerState;
	ZeroMemory(&rasterizerState, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerState.AntialiasedLineEnable = true;
	rasterizerState.CullMode = D3D11_CULL_NONE; ; // D3D11_CULL_FRONT or D3D11_CULL_NONE D3D11_CULL_BACK
	rasterizerState.FillMode = D3D11_FILL_SOLID; // D3D11_FILL_SOLID  D3D11_FILL_WIREFRAME
	rasterizerState.DepthBias = 0;
	rasterizerState.DepthBiasClamp = 0.0f;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.FrontCounterClockwise = true;
	rasterizerState.MultisampleEnable = true;
	rasterizerState.ScissorEnable = false;
	rasterizerState.SlopeScaledDepthBias = 0.0f;

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	m_Renderer->GetDevice()->CreateDepthStencilState(&dssDesc, &DSLessEqual);

	ID3D11RasterizerState* m_pRasterState;
	HRESULT result = m_Renderer->GetDevice()->CreateRasterizerState(&rasterizerState, &m_pRasterState);
	m_Renderer->GetDeviceContext()->RSSetState(m_pRasterState);
	m_Renderer->GetDeviceContext()->OMSetDepthStencilState(DSLessEqual, 0);

	Application* app = reinterpret_cast<Application*>(Application::GetInstance());
	Camera* camera = app->GetCamera();

	DirectX::XMMATRIX world = camera->GetPosition();
	DirectX::XMMATRIX view = camera->GetView();
	DirectX::XMMATRIX projection = camera->GetProjection();

	DirectX::XMMATRIX WVP = DirectX::XMMatrixMultiply(world, DirectX::XMMatrixMultiply(view, projection));

	// Shader thing
	ConstantBuffer cb;
	cb.mWorldViewProj = DirectX::XMMatrixTranspose(WVP);

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	// Bind shader
	m_Renderer->GetDeviceContext()->IASetInputLayout(m_VertexLayout);
	m_Renderer->GetDeviceContext()->VSSetShader(m_VertexShader, nullptr, 0);
	m_Renderer->GetDeviceContext()->PSSetShader(m_PixelShader, nullptr, 0);

	m_Renderer->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	m_Renderer->GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	m_Renderer->GetDeviceContext()->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);

	m_Renderer->GetDeviceContext()->PSSetConstantBuffers(0, 1, &m_ConstantBuffer);
	m_Renderer->GetDeviceContext()->PSSetShaderResources(0, 1, &m_CubeMapSRV);

	m_Renderer->GetDeviceContext()->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cb, 0, 0);

	m_Renderer->GetDeviceContext()->DrawIndexed((UINT)m_Mesh->indices.size(), 0, 0);
}

void Sky::CreateVertexShader(std::string&& vertexShaderPath)
{
	auto m_Device = m_Renderer->GetDevice();
	auto m_DeviceContext = m_Renderer->GetDeviceContext();

	std::ifstream vertexFile(vertexShaderPath, std::fstream::in | std::fstream::binary);
	if (!vertexFile.is_open())
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not read VertexShader.cso", nullptr);
	}

	vertexFile.seekg(0, vertexFile.end);
	int vertexsize = (int)vertexFile.tellg();
	vertexFile.seekg(0, vertexFile.beg);

	char* vertexbuffer = new char[vertexsize];
	vertexFile.read(vertexbuffer, vertexsize);

	DX::ThrowIfFailed(m_Device->CreateVertexShader(vertexbuffer, vertexsize, nullptr, &m_VertexShader));

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = ARRAYSIZE(layout);
	m_Device->CreateInputLayout(layout, numElements, vertexbuffer, vertexsize, &m_VertexLayout);
	//m_DeviceContext->IASetInputLayout(vertexLayout);

	delete[] vertexbuffer;
}

void Sky::CreatePixelShader(std::string&& pixelShaderPath)
{
	auto m_Device = m_Renderer->GetDevice();

	std::ifstream pixelFile(pixelShaderPath, std::fstream::in | std::fstream::binary);
	if (!pixelFile.is_open())
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not read PixelShader.cso", nullptr);
	}

	pixelFile.seekg(0, pixelFile.end);
	int pixelsize = (int)pixelFile.tellg();
	pixelFile.seekg(0, pixelFile.beg);

	char* pixelbuffer = new char[pixelsize];
	pixelFile.read(pixelbuffer, pixelsize);

	DX::ThrowIfFailed(m_Device->CreatePixelShader(pixelbuffer, pixelsize, nullptr, &m_PixelShader));

	delete[] pixelbuffer;
}
