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

#include "Application.h"

#include <iostream>
#include <filesystem>
#include <assimp\postprocess.h>
#include <SDL_messagebox.h>

#include "GeometryGenerator.h"
#include <GeometricPrimitive.h>

ID3D11SamplerState* g_Sampler = nullptr;

namespace
{
	std::string textype;
	std::string determineTextureType(const aiScene* scene, aiMaterial* mat)
	{
		aiString textypeStr;
		mat->GetTexture(aiTextureType_DIFFUSE, 0, &textypeStr);
		std::string textypeteststr = textypeStr.C_Str();
		if (textypeteststr == "*0" || textypeteststr == "*1" || textypeteststr == "*2" || textypeteststr == "*3" || textypeteststr == "*4" || textypeteststr == "*5")
		{
			if (scene->mTextures[0]->mHeight == 0)
			{
				return "embedded compressed texture";
			}
			else
			{
				return "embedded non-compressed texture";
			}
		}

		if (textypeteststr.find('.') != std::string::npos)
		{
			return "textures are on disk";
		}

		return ".";
	}
}

_declspec(align(16)) struct ConstantBuffer
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
	int mUseDiffuseTexture;
	int mUseNormalTexture;
};

Model::Model(Renderer* renderer) : m_Renderer(renderer)
{
}

bool Model::Load(std::string&& filename)
{
	// Drawing
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename.c_str(), aiProcessPreset_TargetRealtime_Fast);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return false;
	}

	m_Name = filename;

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		//Get the mesh
		aiMesh* mesh = scene->mMeshes[i];

		Mesh* obj_mesh = new Mesh();
		obj_mesh->name = mesh->mName.C_Str();

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

			if (mesh->HasTangentsAndBitangents())
			{
				vertex.tx = mesh->mTangents[j].x;
				vertex.ty = mesh->mTangents[j].y;
				vertex.tz = mesh->mTangents[j].z;

				vertex.bx = mesh->mBitangents[j].x;
				vertex.by = mesh->mBitangents[j].y;
				vertex.bz = mesh->mBitangents[j].z;
			}

			if (mesh->mTextureCoords[0])
			{
				vertex.u = (float)mesh->mTextureCoords[0][j].x;
				vertex.v = (float)mesh->mTextureCoords[0][j].y;
			}

			//Add the vertex to the vertices vector
			obj_mesh->vertices.push_back(vertex);
		}

		//Iterate over the faces of the mesh
		for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
		{
			//Get the face
			aiFace face = mesh->mFaces[j];

			//Add the indices of the face to the vector
			for (unsigned int k = 0; k < face.mNumIndices; ++k) 
			{ 
				obj_mesh->indices.push_back(face.mIndices[k]);
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
			for (UINT j = 0; j < material->GetTextureCount(type); j++) 
			{
				aiString str;
				material->GetTexture(type, j, &str);
				
				std::string texture_diffuse = std::string(str.C_Str());
				std::cout << texture_diffuse << '\n';

				std::filesystem::path diffuse_file_path = std::filesystem::path(filename).parent_path() /= std::filesystem::path(texture_diffuse);

				std::wstringstream texture_filename_stream;
				texture_filename_stream << diffuse_file_path.string().c_str();
				std::wstring texture_filename_narrow = texture_filename_stream.str();

				ID3D11Resource* texResource = nullptr;
				DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(m_Renderer->GetDevice(), texture_filename_narrow.c_str(), &texResource, &obj_mesh->m_DiffuseMapSRV));
				texResource->Release();

				m_UseDiffuseTexture = true;
			}

			// WAVEFRONT .OBJ maps the normal texture to the height for some reason
			type = aiTextureType_HEIGHT;
			for (UINT j = 0; j < material->GetTextureCount(type); j++) 
			{
				aiString str;
				material->GetTexture(type, j, &str);
				
				std::string texture_normal = std::string(str.C_Str());
				std::cout << texture_normal << '\n';

				std::filesystem::path normal_file_path = std::filesystem::path(filename).parent_path() /= std::filesystem::path(texture_normal);

				std::wstringstream texture_filename_stream1;
				texture_filename_stream1 << normal_file_path.string().c_str();
				std::wstring texture_filename_narrow1 = texture_filename_stream1.str();

				ID3D11Resource* texResource = nullptr;
				DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(m_Renderer->GetDevice(), texture_filename_narrow1.c_str(), &texResource, &obj_mesh->m_NormalMapSRV));
				texResource->Release();

				m_UseNormalTexture = true;
			}

			type = aiTextureType_NORMALS;
			for (UINT j = 0; j < material->GetTextureCount(type); j++) 
			{
				aiString str;
				material->GetTexture(type, j, &str);

				std::string texture_normal = std::string(str.C_Str());
				std::cout << texture_normal << '\n';

				std::filesystem::path normal_file_path = std::filesystem::path(filename).parent_path() /= std::filesystem::path(texture_normal);

				std::wstringstream texture_filename_stream1;
				texture_filename_stream1 << normal_file_path.string().c_str();
				std::wstring texture_filename_narrow1 = texture_filename_stream1.str();

				ID3D11Resource* texResource = nullptr;
				DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(m_Renderer->GetDevice(), texture_filename_narrow1.c_str(), &texResource, &obj_mesh->m_NormalMapSRV));
				texResource->Release();

				m_UseNormalTexture = true;
			}
		}

		if (!m_Meshes.empty())
		{
			Mesh* lastMesh = m_Meshes.back();
			if (lastMesh != nullptr)
			{
				obj_mesh->startIndex = lastMesh->startIndex + (WORD)lastMesh->indices.size();
				obj_mesh->startVertex = lastMesh->startVertex + (WORD)lastMesh->vertices.size();
			}
		}

		m_Meshes.push_back(obj_mesh);
	}

	std::vector<SimpleVertex> vertices;
	std::vector<WORD> indices;

	for (auto& mesh : m_Meshes)
	{
		vertices.insert(vertices.end(), mesh->vertices.begin(), mesh->vertices.end());
		indices.insert(indices.end(), mesh->indices.begin(), mesh->indices.end());
	}

	// Vertex duffer description
	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(SimpleVertex) * static_cast<UINT>(vertices.size());
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vInitData = {};

	const SimpleVertex* vert = &vertices[0];
	vInitData.pSysMem = vert;
	
	DX::ThrowIfFailed(m_Renderer->GetDevice()->CreateBuffer(&vbd, &vInitData, &m_VertexBuffer));

	// Index buffer description
	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(WORD) * static_cast<UINT>(indices.size());
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA iInitData = {};
	const WORD* index = &indices[0];
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

	// Perspective View
	m_World = DirectX::XMMatrixIdentity();

	// Texture
	/*if (!texture_diffuse.empty())
	{
		std::filesystem::path diffuse_file_path = std::filesystem::path(filename).parent_path() /= std::filesystem::path(texture_diffuse);
		std::filesystem::path normal_file_path = std::filesystem::path(filename).parent_path() /= std::filesystem::path(texture_normal);

		ID3D11Resource* texResource = nullptr;

		if (!diffuse_file_path.string().empty())
		{
			std::wstringstream texture_filename_stream;
			texture_filename_stream << diffuse_file_path.string().c_str();
			std::wstring texture_filename_narrow = texture_filename_stream.str();

			DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(m_Renderer->GetDevice(), texture_filename_narrow.c_str(), &texResource, &m_DiffuseMapSRV));

			m_UseDiffuseTexture = true;
		}

		if (!normal_file_path.string().empty())
		{
			std::wstringstream texture_filename_stream1;
			texture_filename_stream1 << normal_file_path.string().c_str();
			std::wstring texture_filename_narrow1 = texture_filename_stream1.str();

			DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(m_Renderer->GetDevice(), texture_filename_narrow1.c_str(), &texResource, &m_NormalMapSRV));

			m_UseNormalTexture = true;
		}
	}*/

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 8;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = 0;

	m_Renderer->GetDevice()->CreateSamplerState(&samplerDesc, &g_Sampler);

	m_IsLoaded = true;
	return true;
}

void Model::Unload()
{
	for (auto& mesh : m_Meshes)
	{
		delete mesh;
	}

	m_Meshes.clear();

	m_Pitch = 0.0f;
	m_Yaw = 0.0f;
	m_Roll = 0.0f;

	/*DX::SafeRelease(m_DiffuseMapSRV);
	DX::SafeRelease(m_NormalMapSRV);*/
	DX::SafeRelease(m_ConstantBuffer);

	m_IsLoaded = false;

	/*texture_diffuse.clear();
	texture_normal.clear();*/

	m_UseDiffuseTexture = false;
	m_UseNormalTexture = false;
}

void Model::Gui()
{
	ImGui::Text("File: %s", m_Name.c_str());

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	for (auto& mesh : m_Meshes)
	{
		ImGui::Text("Name: %s", mesh->name.c_str());
		ImGui::Text("Vertices: %i", mesh->vertices.size());
		ImGui::Text("Indices: %i", mesh->indices.size());

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}

	ImGui::Text("Position");
	ImGui::DragFloat("Position X", &m_PositionX, 0.1f);
	ImGui::DragFloat("Position Y", &m_PositionY, 0.1f);
	ImGui::DragFloat("Position Z", &m_PositionZ, 0.1f);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Rotation");
	ImGui::DragFloat("Rotate X", &m_Pitch, 0.1f);
	ImGui::DragFloat("Rotate Y", &m_Yaw, 0.1f);
	ImGui::DragFloat("Rotate Z", &m_Roll, 0.1f);

	ImGui::Spacing();  
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Scaling");
	ImGui::DragFloat("Scale X", &m_ScaleX, 0.1f);
	ImGui::DragFloat("Scale Y", &m_ScaleY, 0.1f);
	ImGui::DragFloat("Scale Z", &m_ScaleZ, 0.1f);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Checkbox("Wireframe", &m_Wireframe);
	ImGui::Checkbox("Diffuse Texture", &m_UseDiffuseTexture);
	ImGui::Checkbox("Normal Texture", &m_UseNormalTexture);
}

void Model::Update()
{
	m_World = DirectX::XMMatrixIdentity();

	// Scale
	m_World *= DirectX::XMMatrixScaling(m_ScaleX, m_ScaleY, m_ScaleZ);

	// Rotation
	float pitch = DirectX::XMConvertToRadians(m_Pitch);
	float yaw = DirectX::XMConvertToRadians(m_Yaw);
	float roll = DirectX::XMConvertToRadians(m_Roll);
	m_World *= DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Translation
	m_World *= DirectX::XMMatrixTranslation(m_PositionX, m_PositionY, m_PositionZ);
}

void Model::Render()
{
	Application* app = reinterpret_cast<Application*>(Application::GetInstance());
	Camera* camera = app->GetCamera();

	DirectX::XMMATRIX view = camera->GetView();
	DirectX::XMMATRIX projection = camera->GetProjection();

	// Shader thing
	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(m_World);
	cb.mView = DirectX::XMMatrixTranspose(view);
	cb.mProjection = DirectX::XMMatrixTranspose(projection);
	cb.mUseDiffuseTexture = m_UseDiffuseTexture;
	cb.mUseNormalTexture = m_UseNormalTexture;

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	m_Renderer->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	m_Renderer->GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	m_Renderer->GetDeviceContext()->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);

	m_Renderer->GetDeviceContext()->PSSetConstantBuffers(0, 1, &m_ConstantBuffer);
	m_Renderer->GetDeviceContext()->PSSetSamplers(0, 1, &g_Sampler);

	m_Renderer->GetDeviceContext()->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cb, 0, 0);

	SetRasterState();

	// Render triangle
	for (auto& mesh : m_Meshes)
	{

		m_Renderer->GetDeviceContext()->PSSetShaderResources(0, 1, &mesh->m_DiffuseMapSRV);
		m_Renderer->GetDeviceContext()->PSSetShaderResources(1, 1, &mesh->m_NormalMapSRV);
		m_Renderer->GetDeviceContext()->DrawIndexed((UINT)mesh->indices.size(), (UINT)mesh->startIndex, mesh->startVertex);
	}
}

void Model::OnMouseMotion(MouseData&& data)
{

}

void Model::SetRasterState()
{
	D3D11_RASTERIZER_DESC rasterizerState;
	ZeroMemory(&rasterizerState, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerState.AntialiasedLineEnable = true;
	rasterizerState.CullMode = (m_Wireframe ? D3D11_CULL_NONE : D3D11_CULL_FRONT); ; // D3D11_CULL_FRONT or D3D11_CULL_NONE D3D11_CULL_BACK
	rasterizerState.FillMode = (m_Wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID); // D3D11_FILL_SOLID  D3D11_FILL_WIREFRAME
	rasterizerState.DepthBias = 0;
	rasterizerState.DepthBiasClamp = 0.0f;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.FrontCounterClockwise = true;
	rasterizerState.MultisampleEnable = true;
	rasterizerState.ScissorEnable = false;
	rasterizerState.SlopeScaledDepthBias = 0.0f;

	ID3D11RasterizerState* m_pRasterState;
	HRESULT result = m_Renderer->GetDevice()->CreateRasterizerState(&rasterizerState, &m_pRasterState);
	m_Renderer->GetDeviceContext()->RSSetState(m_pRasterState);
}