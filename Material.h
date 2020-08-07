#pragma once
#include <assimp\scene.h>
#include "BindableCommon.h"
#include <vector>
#include <filesystem>
#include "Technique.h"

struct aiMaterial;
struct aiMesh;

class Material
{
public:
	Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noxnd;
	Dynamic::VertexBuffer ExtractVertices(const aiMesh& mesh) const noexcept;
	std::vector<unsigned short> ExtractIndices(const aiMesh& mesh) const noexcept;

	std::shared_ptr<Bind::VertexBuffer> MakeVertexBindable(Graphics& gfx, const aiMesh& mesh, float scale = 1.0f) const noxnd;
	std::shared_ptr<Bind::IndexBuffer> MakeIndexBindable(Graphics& gfx, const aiMesh& mesh) const noxnd;
	std::vector<Technique> GetTechniques() const noexcept;
private:
	std::string MakeMeshTag(const aiMesh& mesh) const noexcept;
private:
	Dynamic::VertexLayout m_vtxLayout;
	std::vector<Technique> m_Techniques;
	std::string m_ModelPath;
	std::string m_Name;
};