#include "AssimpTest.h"
#include "BindableBase.h"
#include "GfxThrowMacros.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "AderMath.h"

namespace dx = DirectX;

AssimpTest::AssimpTest(Graphics& gfx, std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	float scale,
	DirectX::XMFLOAT3 material)
	:
	r(rdist(rng)),
	droll(ddist(rng)),
	dpitch(ddist(rng)),
	dyaw(ddist(rng)),
	dphi(odist(rng)),
	dtheta(odist(rng)),
	dchi(odist(rng)),
	chi(adist(rng)),
	theta(adist(rng)),
	phi(adist(rng))
{
	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			dx::XMFLOAT3 pos;
			dx::XMFLOAT3 n;
		};

		// Load model vertices and indices using Assimp instead of procedurally generating them 
		Assimp::Importer imp;
		const auto pModel = imp.ReadFile("models\\suzanne.obj",
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices
		);
		const auto pMesh = pModel->mMeshes[0];

		std::vector<Vertex> vertices;
		vertices.reserve(pMesh->mNumVertices);
		for (unsigned int i = 0; i < pMesh->mNumVertices; i++)
		{
			vertices.push_back({
				{ pMesh->mVertices[i].x * scale,pMesh->mVertices[i].y * scale,pMesh->mVertices[i].z * scale },
				*reinterpret_cast<dx::XMFLOAT3*>(&pMesh->mNormals[i])
				});
		}

		std::vector<unsigned short> indices;
		indices.reserve(pMesh->mNumFaces * 3);
		for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
		{
			const auto& face = pMesh->mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}
		// Model vertices loading completed


		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));

		auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		struct PSMaterialConstant
		{
			DirectX::XMFLOAT3 color;
			float padding;
		};
		PSMaterialConstant colorConst{ material };
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, colorConst, 1u));
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCBuffer>(gfx, *this));
}

void AssimpTest::Update(float dt) noexcept
{
	roll = wrap_angle(roll + droll * dt);
	pitch = wrap_angle(pitch + dpitch * dt);
	yaw = wrap_angle(yaw + dyaw * dt);
	theta = wrap_angle(theta + dtheta * dt);
	phi = wrap_angle(phi + dphi * dt);
	chi = wrap_angle(chi + dchi * dt);
}

DirectX::XMMATRIX AssimpTest::GetTransformXM() const noexcept
{
	return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
