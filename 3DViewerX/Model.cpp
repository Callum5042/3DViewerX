#include "Model.h"
#include <imgui.h>
#include <DDSTextureLoader.h>

#undef min
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <vector>

#include "Engine.h"
#include "MainWindow.h"
#include <sstream>

#include <locale>
#include <codecvt>
#include <string>

std::string textype;
std::string determineTextureType(const aiScene* scene, aiMaterial* mat) 
{
	aiString textypeStr;
	mat->GetTexture(aiTextureType_DIFFUSE, 0, &textypeStr);
	std::string textypeteststr = textypeStr.C_Str();
	if (textypeteststr == "*0" || textypeteststr == "*1" || textypeteststr == "*2" || textypeteststr == "*3" || textypeteststr == "*4" || textypeteststr == "*5") {
		if (scene->mTextures[0]->mHeight == 0) {
			return "embedded compressed texture";
		}
		else {
			return "embedded non-compressed texture";
		}
	}
	if (textypeteststr.find('.') != std::string::npos) {
		return "textures are on disk";
	}

	return ".";
}

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

Model::Model(Renderer* renderer) : m_Renderer(renderer)
{
}

bool Model::Load(std::string&& filename)
{
	std::string texture_filename;

	// Drawing
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return false;
	}

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		//Get the mesh
		aiMesh* mesh = scene->mMeshes[i];

		m_Name = mesh->mName.C_Str();

		//Iterate over the vertices of the mesh
		for (unsigned int j = 0; j < mesh->mNumVertices; ++j)
		{
			//Set the positions
			float x = static_cast<float>(mesh->mVertices[j].x);
			float y = static_cast<float>(mesh->mVertices[j].y);
			float z = static_cast<float>(mesh->mVertices[j].z);

			//Create a vertex to store the mesh's vertices temporarily
			SimpleVertex vertex(x, y, z);
			vertex.nx = static_cast<float>(mesh->mNormals[j].x);
			vertex.ny = static_cast<float>(mesh->mNormals[j].y);
			vertex.nz = static_cast<float>(mesh->mNormals[j].z);

			if (mesh->mTextureCoords[0]) 
			{
				vertex.u = (float)mesh->mTextureCoords[0][j].x;
				vertex.v = (float)mesh->mTextureCoords[0][j].y;
			}

			//Add the vertex to the vertices vector
			m_Vertices.push_back(vertex);
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

		// Materials
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			if (textype.empty()) 
			{
				textype = determineTextureType(scene, material);
			}

			aiTextureType type = aiTextureType_DIFFUSE;
			for (UINT i = 0; i < material->GetTextureCount(type); i++) 
			{
				aiString str;
				material->GetTexture(type, i, &str);
				
				texture_filename = std::string(str.C_Str());
				std::cout << texture_filename << '\n';
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

	XMVECTOR eye = XMVectorSet(0.0f, 0.0f, -4.0f, 0.0f);
	XMVECTOR at = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = XMMatrixLookAtLH(eye, at, up);

	MainWindow* window = Engine::GetInstance()->GetWindow();

	// Create the projection matrix for 3D rendering.
	float fieldOfView = 85 * DirectX::XM_PI / 180;
	float screenAspect = static_cast<float>(window->GetWidth()) / static_cast<float>(window->GetHeight());
	m_Projection = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.01f, 100.0f);

	// Texture
	if (!texture_filename.empty())
	{
		ID3D11Resource* texResource = nullptr;
		std::wstringstream texture_filename_stream;
		texture_filename_stream << texture_filename.c_str();
		std::wstring texture_filename_narrow = texture_filename_stream.str();

		DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(m_Renderer->GetDevice(), texture_filename_narrow.c_str(), &texResource, &m_DiffuseMapSRV));
	}

	m_IsLoaded = true;
	return true;
}

void Model::Unload()
{
	m_Vertices.clear();
	m_Indices.clear();

	m_AxisX = 0.0f;
	m_AxisY = 0.0f;
	m_AxisZ = 0.0f;

	m_DiffuseMapSRV->Release();
	m_DiffuseMapSRV = nullptr;

	m_ConstantBuffer->Release();
	m_ConstantBuffer = nullptr;

	m_IsLoaded = false;
}

void Model::Update()
{
	ImGui::Begin("Model");
	ImGui::SetNextWindowSize(ImVec2(200, 200));

	ImGui::Text("Name: %s", m_Name.c_str());
	ImGui::Text("Vertices: %i", m_Vertices.size());

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
	m_Renderer->GetDeviceContext()->PSSetShaderResources(0, 1, &m_DiffuseMapSRV);

	m_Renderer->GetDeviceContext()->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cb, 0, 0);

	SetRasterState();

	// Render triangle
	m_Renderer->GetDeviceContext()->DrawIndexed(static_cast<UINT>(m_Indices.size()), 0, 0);
}

void Model::OnResize(int width, int height)
{
	float fieldOfView = 85 * DirectX::XM_PI / 180;
	float screenAspect = static_cast<float>(width) / static_cast<float>(height);
	m_Projection = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.01f, 100.0f);
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