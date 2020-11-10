#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Vertex.h"
#include "BindableBase.h"
#include "DrawableBase.h"

namespace dx = DirectX;

class Mesh : public DrawableBase<Mesh>
{
private:
	mutable dx::XMFLOAT4X4 transform;
public:
	Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bindable>> bindPtrs)
	{
		if (!IsStaticInitialized())
		{
			AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		}

		for(auto& pb : bindPtrs)
		{
			if( auto pIndexBuffer = dynamic_cast<IndexBuffer*>(pb.get()) )
			{
				AddIndexBuffer(std::unique_ptr<IndexBuffer>{ pIndexBuffer });
				pb.release();
			}
			else
			{
				AddBind( std::move( pb ) );
			}
		}
		AddBind(std::make_unique<TransformCBuffer>(gfx, *this));
	}
	void Draw(Graphics& gfx, dx::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
	{
		dx::XMStoreFloat4x4(&transform, accumulatedTransform);
		Drawable::Draw(gfx);
	}
	dx::XMMATRIX GetTransformXM() const noexcept override
	{
		return dx::XMLoadFloat4x4(&transform);
	}
};


class Node
{
	friend class Model;
private:
	std::vector<std::unique_ptr<Node>> childNodesPtrs;
	std::vector<Mesh*> meshPtrs;
	dx::XMFLOAT4X4 transform;

public:
	Node(std::vector<Mesh*> meshPtrs, const dx::XMMATRIX& transf)
		:
		meshPtrs(std::move(meshPtrs))
	{
		dx::XMStoreFloat4x4(&transform, transf);
	}
	void AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG)
	{
		assert("Trying to add an incalid/empty child node" && pChild);
		childNodesPtrs.push_back(std::move(pChild));
	}
	void Draw(Graphics& gfx, dx::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
	{
		const auto builtTransform = dx::XMLoadFloat4x4(&transform) * accumulatedTransform;
		for (const auto mesh : meshPtrs)
		{
			mesh->Draw(gfx, builtTransform);
		}
		for (const auto& childNode : childNodesPtrs)
		{
			childNode->Draw(gfx, builtTransform);
		}
	}
};

class Model
{
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;

public:
	Model(Graphics& gfx, const std::string fileName)
	{
		Assimp::Importer imp;
		const auto pScene = imp.ReadFile(fileName.c_str(),
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices
		);

		for (size_t i = 0; i < pScene->mNumMeshes; i++)
		{
			meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i]));
		}

		pRoot = ParseNode(*pScene->mRootNode);
	}

	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh)
	{
		std::vector<std::unique_ptr<Bindable>> bindablePtrs;	// collects the mesh's bindables 

		// Vertex Buffer
		using AderVertex::VertexLayout;
		AderVertex::VertexBuffer vbuf(
			std::move(VertexLayout{}
				.Append(VertexLayout::Position3D)
				.Append(VertexLayout::Normal)
			)
		);
		for (size_t i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i])
			);
		}
		bindablePtrs.push_back(std::make_unique<VertexBuffer>(gfx, vbuf));

		// Index Buffer
		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3u);
		for (size_t i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}
		bindablePtrs.push_back(std::make_unique<IndexBuffer>(gfx, indices));
 
		// Vertex Shader
		auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		bindablePtrs.push_back(std::move(pvs));

		// Pixel Shader
		bindablePtrs.push_back(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

		// Input Layout
		bindablePtrs.push_back(std::make_unique<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc));

		// Material Constant Buffer :: Only contains an arbitrary color (for now)
		struct PSMaterialConstant
		{
			DirectX::XMFLOAT3 color = { 0.6f,0.6f,0.8f };
			float padding;
		} pmc;
		bindablePtrs.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));

		return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
	}

	std::unique_ptr<Node> ParseNode(const aiNode& node)
	{
		const auto& nodeTransform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)));
		std::vector<Mesh*> nodeMeshPtrs;
		nodeMeshPtrs.reserve(node.mNumMeshes);

		for (size_t i = 0; i < node.mNumMeshes; i++)
		{
			const auto& meshIdx = node.mMeshes[i];
			nodeMeshPtrs.push_back(meshPtrs[meshIdx].get());
		}

		auto pNode = std::make_unique<Node>(std::move(nodeMeshPtrs), nodeTransform);
		for (size_t i = 0; i < node.mNumChildren; i++)
		{
			pNode->AddChild(ParseNode(*node.mChildren[i]));
		}

		return pNode;
	}

	void Draw(Graphics& gfx, dx::FXMMATRIX transform) const
	{
		pRoot->Draw(gfx, transform);
	}
};