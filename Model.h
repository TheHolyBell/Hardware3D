#pragma once
#include "Graphics.h"
#include "ConditionalNoexcept.h"
#include <string>
#include <memory>
#include <filesystem>

class Node;
class Mesh;
class FrameCommander;
class ModelWindow;
struct aiMesh;
struct aiMaterial;
struct aiNode;

class Model
{
public:
	Model(Graphics& gfx, const std::string& pathString, float scale = 1.0f);
	void Submit(FrameCommander& frame) const noxnd;
	void SetRootTransform(DirectX::FXMMATRIX tf) noexcept;
	void Accept(class ModelProbe& probe);
	~Model() noexcept;
private:
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale);
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node, float scale) noexcept;
private:
	std::unique_ptr<Node> m_pRoot;
	// sharing meshes here perhaps dangerous?
	std::vector<std::unique_ptr<Mesh>> m_MeshPtrs;
	//std::unique_ptr<ModelWindow> pWindow;
};