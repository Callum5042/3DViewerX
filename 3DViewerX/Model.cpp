#include "Model.h"
#include <imgui.h>

struct SimpleVertex
{
	SimpleVertex(float x, float y, float z) : x(x), y(y), z(z) {}

	float x;
	float y;
	float z;
};

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

bool Model::Load(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	// Drawing
	SimpleVertex vertices[] =
	{
		{ -1.0f, 1.0f, -1.0f },
		{ 1.0f, 1.0f, -1.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ -1.0f, 1.0f, 1.0f },

		{ -1.0f, -1.0f, -1.0f },
		{ 1.0f, -1.0f, -1.0f },
		{ 1.0f, -1.0f, 1.0f },
		{ -1.0f, -1.0f, 1.0f },

		{ -1.0f, -1.0f, 1.0f },
		{ -1.0f, -1.0f, -1.0f },
		{ -1.0f, 1.0f, -1.0f },
		{ -1.0f, 1.0f, 1.0f },

		{ 1.0f, -1.0f, 1.0f },
		{ 1.0f, -1.0f, -1.0f },
		{ 1.0f, 1.0f, -1.0f },
		{ 1.0f, 1.0f, 1.0f },

		{ -1.0f, -1.0f, -1.0f },
		{ 1.0f, -1.0f, -1.0f },
		{ 1.0f, 1.0f, -1.0f },
		{ -1.0f, 1.0f, -1.0f },

		{ -1.0f, -1.0f, 1.0f },
		{ 1.0f, -1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ -1.0f, 1.0f, 1.0f },
	};

	WORD indices[] =
	{
		3,1,0,
		2,1,3,

		6,4,5,
		7,4,6,

		11,9,8,
		10,9,11,

		14,12,13,
		15,12,14,

		19,17,16,
		18,17,19,

		22,20,21,
		23,20,22
	};

	// Vertex duffer description
	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(vertices);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vInitData = {};
	vInitData.pSysMem = vertices;

	ID3D11Buffer* vertexBuffer = nullptr;
	HRESULT hr = device->CreateBuffer(&vbd, &vInitData, &vertexBuffer);
	if (FAILED(hr))
		return false;

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// Index buffer description
	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(indices);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA iInitData = {};
	iInitData.pSysMem = indices;

	ID3D11Buffer* indexBuffer = nullptr;
	hr = device->CreateBuffer(&ibd, &iInitData, &indexBuffer);
	if (FAILED(hr))
		return false;

	// Set vertex buffer
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = device->CreateBuffer(&bd, nullptr, &m_ConstantBuffer);
	if (FAILED(hr))
	{
		// SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "CreateBuffer failed", nullptr);
		return false;
	}

	// Perspective View
	m_World = XMMatrixIdentity();

	XMVECTOR eye = XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
	XMVECTOR at = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = XMMatrixLookAtLH(eye, at, up);

	// Create the projection matrix for 3D rendering.
	float fieldOfView = 85 * DirectX::XM_PI / 180;
	float screenAspect = (float)800 / (float)600;
	m_Projection = XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, screenAspect, 0.01f, 100.0f);

	return true;
}

void Model::Update()
{
	bool my_tool_active;
	ImGui::Begin("Model", &my_tool_active, ImGuiWindowFlags_None);
	ImGui::SetNextWindowSize(ImVec2(200, 200));

	ImGui::Text("Rotation");
	ImGui::SliderFloat("X-Axis", &m_AxisX, 0.0f, 360.0f);
	ImGui::SliderFloat("Y-Axis", &m_AxisY, 0.0f, 360.0f);
	ImGui::SliderFloat("Z-Axis", &m_AxisZ, 0.0f, 360.0f);

	m_World = XMMatrixRotationX(DirectX::XMConvertToRadians(m_AxisX));
	m_World *= XMMatrixRotationY(DirectX::XMConvertToRadians(m_AxisY));
	m_World *= XMMatrixRotationZ(DirectX::XMConvertToRadians(m_AxisZ));

	ImGui::End();
}

void Model::Render(ID3D11DeviceContext* deviceContext)
{
	// Shader thing
	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(m_World);
	cb.mView = XMMatrixTranspose(m_View);
	cb.mProjection = XMMatrixTranspose(m_Projection);

	deviceContext->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &m_ConstantBuffer);

	deviceContext->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cb, 0, 0);

	// Render triangle
	deviceContext->DrawIndexed(36, 0, 0);
}