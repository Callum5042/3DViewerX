#include "Model.h"
#include <imgui.h>

#undef min
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <vector>

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

Model::Model(Renderer* renderer) : m_Renderer(renderer)
{
}

bool Model::Load()
{
	// Drawing
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile("C:\\Users\\Callum\\Desktop\\3d models\\monkey.obj", aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return false;
	}

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		//Get the mesh
		aiMesh* mesh = scene->mMeshes[i];

		//Iterate over the vertices of the mesh
		for (unsigned int j = 0; j < mesh->mNumVertices; ++j)
		{
			//Set the positions
			float x = static_cast<float>(mesh->mVertices[j].x);
			float y = static_cast<float>(mesh->mVertices[j].y);
			float z = static_cast<float>(mesh->mVertices[j].z);

			//Create a vertex to store the mesh's vertices temporarily
			SimpleVertex tempVertex(x, y, z);
			tempVertex.nx = static_cast<float>(mesh->mNormals[j].x);
			tempVertex.ny = static_cast<float>(mesh->mNormals[j].y);
			tempVertex.nz = static_cast<float>(mesh->mNormals[j].z);

			//Add the vertex to the vertices vector
			m_Vertices.push_back(tempVertex);
		}

		//Iterate over the faces of the mesh
		for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
		{
			//Get the face
			aiFace face = mesh->mFaces[j];

			//Add the indices of the face to the vector
			for (unsigned int k = 0; k < face.mNumIndices; ++k) 
			{ 
				m_Indices.push_back(face.mIndices[k]); 
			}
		}
	}

	// Vertex duffer description
	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(SimpleVertex) * static_cast<UINT>(m_Vertices.size());
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vInitData = {};

	const SimpleVertex* vert = &m_Vertices[0];
	vInitData.pSysMem = vert;

	ID3D11Buffer* vertexBuffer = nullptr;
	HRESULT hr = m_Renderer->GetDevice()->CreateBuffer(&vbd, &vInitData, &vertexBuffer);
	if (FAILED(hr))
		return false;
	  
	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	m_Renderer->GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// Index buffer description
	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(WORD) * static_cast<UINT>(m_Indices.size());
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA iInitData = {};
	const WORD* index = &m_Indices[0];
	iInitData.pSysMem = index;

	ID3D11Buffer* indexBuffer = nullptr;
	hr = m_Renderer->GetDevice()->CreateBuffer(&ibd, &iInitData, &indexBuffer);
	if (FAILED(hr))
		return false;

	// Set vertex buffer
	m_Renderer->GetDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	m_Renderer->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = m_Renderer->GetDevice()->CreateBuffer(&bd, nullptr, &m_ConstantBuffer);
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
	ImGui::Begin("Model");
	ImGui::SetNextWindowSize(ImVec2(200, 200));

	ImGui::Text("Rotation");
	ImGui::SliderFloat("X-Axis", &m_AxisX, 0.0f, 360.0f);
	ImGui::SliderFloat("Y-Axis", &m_AxisY, 0.0f, 360.0f);
	ImGui::SliderFloat("Z-Axis", &m_AxisZ, 0.0f, 360.0f);

	ImGui::Checkbox("Wireframe", &m_Wireframe);

	m_World = XMMatrixRotationX(DirectX::XMConvertToRadians(m_AxisX));
	m_World *= XMMatrixRotationY(DirectX::XMConvertToRadians(m_AxisY));
	m_World *= XMMatrixRotationZ(DirectX::XMConvertToRadians(m_AxisZ));



	ImGui::End();
}

void Model::Render()
{
	// Shader thing
	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(m_World);
	cb.mView = XMMatrixTranspose(m_View);
	cb.mProjection = XMMatrixTranspose(m_Projection);

	m_Renderer->GetDeviceContext()->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
	m_Renderer->GetDeviceContext()->PSSetConstantBuffers(0, 1, &m_ConstantBuffer);

	m_Renderer->GetDeviceContext()->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cb, 0, 0);

	SetRasterState();

	// Render triangle
	m_Renderer->GetDeviceContext()->DrawIndexed(static_cast<UINT>(m_Indices.size()), 0, 0);
}

void Model::SetRasterState()
{
	D3D11_RASTERIZER_DESC rasterizerState;
	ZeroMemory(&rasterizerState, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerState.AntialiasedLineEnable = false;
	rasterizerState.CullMode = D3D11_CULL_NONE; // D3D11_CULL_FRONT or D3D11_CULL_NONE D3D11_CULL_BACK
	rasterizerState.FillMode = (m_Wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID); // D3D11_FILL_SOLID  D3D11_FILL_WIREFRAME
	rasterizerState.DepthBias = 0;
	rasterizerState.DepthBiasClamp = 0.0f;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.FrontCounterClockwise = true;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.ScissorEnable = false;
	rasterizerState.SlopeScaledDepthBias = 0.0f;

	ID3D11RasterizerState* m_pRasterState;
	HRESULT result = m_Renderer->GetDevice()->CreateRasterizerState(&rasterizerState, &m_pRasterState);
	m_Renderer->GetDeviceContext()->RSSetState(m_pRasterState);
}