#include "Material.h"
#include "DynamicConstant.h"
#include "ConstantBuffersEx.h"
#include "ShaderReflector.h"

Material::Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noxnd
	:
	m_ModelPath(path.string())
{
	using namespace Bind;
	const auto rootPath = path.parent_path().string() + "\\";
	{
		aiString tempName;
		material.Get(AI_MATKEY_NAME, tempName);
		m_Name = tempName.C_Str();
	}
	// phong technique
	{
		Technique phong{ "Phong" };
		Step step(0);
		std::string shaderCode = "Phong";
		aiString texFileName;

		// common (pre)
		bool hasTexture = false;
		bool hasGlossAlpha = false;

		// diffuse
		{
			bool hasAlpha = false;
			if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				shaderCode += "Dif";
				auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str());
				if (tex->HasAlpha())
				{
					hasAlpha = true;
					shaderCode += "Msk";
				}
				step.AddBindable(std::move(tex));
			}
			step.AddBindable(Rasterizer::Resolve(gfx, hasAlpha));
		}
		// specular
		{
			if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				shaderCode += "Spc";
				auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 1);
				hasGlossAlpha = tex->HasAlpha();
				step.AddBindable(std::move(tex));
			}
		}
		// normal
		{
			if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				shaderCode += "Nrm";
				step.AddBindable(Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 2));
			}
		}
		// common (post)
		{
			step.AddBindable(std::make_shared<TransformCbuf>(gfx, 0u));
			step.AddBindable(Blender::Resolve(gfx, false));
			auto pvs = VertexShader::Resolve(gfx, shaderCode + "_VS.cso");
			auto pvsbc = pvs->GetByteCode();
			step.AddBindable(std::move(pvs));
			
			m_vtxLayout = ShaderReflector::GetLayoutFromShader(pvsbc);

			auto pps = PixelShader::Resolve(gfx, shaderCode + "_PS.cso");
			auto ppsbc = pps->GetByteCode();
			step.AddBindable(std::move(pps));
			
			step.AddBindable(InputLayout::Resolve(gfx, m_vtxLayout, pvsbc));
			if (hasTexture)
			{
				step.AddBindable(Bind::Sampler::Resolve(gfx));
			}
			// PS material params (cbuf)
			Dynamic::Buffer buf = ShaderReflector::GetBufferByRegister(ppsbc, 1);
			if (auto r = buf["materialColor"]; r.Exists())
			{
				aiColor3D color = { 0.45f,0.45f,0.85f };
				material.Get(AI_MATKEY_COLOR_DIFFUSE, color);
				r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}
			buf["useGlossAlpha"].SetIfExists(hasGlossAlpha);
			buf["useSpecularMap"].SetIfExists(true);
			if (auto r = buf["specularColor"]; r.Exists())
			{
				aiColor3D color = { 0.18f,0.18f,0.18f };
				material.Get(AI_MATKEY_COLOR_SPECULAR, color);
				r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}
			buf["specularWeight"].SetIfExists(1.0f);
			if (auto r = buf["specularGloss"]; r.Exists())
			{
				float gloss = 8.0f;
				material.Get(AI_MATKEY_SHININESS, gloss);
				r = gloss;
			}
			buf["useNormalMap"].SetIfExists(true);
			buf["normalMapWeight"].SetIfExists(1.0f);
			step.AddBindable(std::make_unique<Bind::CachingPixelConstantBufferEx>(gfx, std::move(buf), 1u));
		}
		phong.AddStep(std::move(step));
		m_Techniques.push_back(std::move(phong));
	}
	// outline technique
	{
		Technique outline("Outline", false);
		{
			Step mask(1);

			auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
			auto pvsbc = pvs->GetByteCode();
			mask.AddBindable(std::move(pvs));

			// TODO: better sub-layout generation tech for future consideration maybe
			mask.AddBindable(InputLayout::Resolve(gfx, m_vtxLayout, pvsbc));

			mask.AddBindable(std::make_shared<TransformCbuf>(gfx));

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			outline.AddStep(std::move(mask));
		}
		{
			Step draw(2);

			// these can be pass-constant (tricky due to layout issues)
			auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
			auto pvsbc = pvs->GetByteCode();
			draw.AddBindable(std::move(pvs));

			// this can be pass-constant
			//draw.AddBindable(PixelShader::Resolve(gfx, "Solid_PS.cso"));

			auto pps = PixelShader::Resolve(gfx, "Solid_PS.cso");
			auto ppsbc = pps->GetByteCode();
			draw.AddBindable(std::move(pps));

			//auto buf = Dynamic::Buffer(std::move(lay));
			auto buf = ShaderReflector::GetBufferByRegister(ppsbc, 1);
			buf["color"] = DirectX::XMFLOAT4{ 1.0f,0.0f,0.0f, 1.0f };
			draw.AddBindable(std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 1u));
			
			// TODO: better sub-layout generation tech for future consideration maybe
			draw.AddBindable(InputLayout::Resolve(gfx, m_vtxLayout, pvsbc));


			draw.AddBindable(std::make_shared<TransformCbuf>(gfx));

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			outline.AddStep(std::move(draw));
		}
		m_Techniques.push_back(std::move(outline));
	}

/*using namespace Bind;
const auto rootPath = path.parent_path().string() + "\\";
{
	aiString tempName;
	material.Get(AI_MATKEY_NAME, tempName);
	m_Name = tempName.C_Str();
}
// phong technique
{
	Technique phong{ "Phong" };
	Step step(0);
	std::string shaderCode = "Phong";
	aiString texFileName;

	// common (pre)
	m_vtxLayout.Append(Dynamic::VertexLayout::Position3D);
	m_vtxLayout.Append(Dynamic::VertexLayout::Normal);
	Dynamic::RawLayout pscLayout;
	bool hasTexture = false;
	bool hasGlossAlpha = false;

	// diffuse
	{
		bool hasAlpha = false;
		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
		{
			hasTexture = true;
			shaderCode += "Dif";
			m_vtxLayout.Append(Dynamic::VertexLayout::Texture2D);
			auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str());
			if (tex->HasAlpha())
			{
				hasAlpha = true;
				shaderCode += "Msk";
			}
			step.AddBindable(std::move(tex));
		}
		else
		{
			pscLayout.Add<Dynamic::Float3>("materialColor");
		}
		step.AddBindable(Rasterizer::Resolve(gfx, hasAlpha));
	}
	// specular
	{
		if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
		{
			hasTexture = true;
			shaderCode += "Spc";
			m_vtxLayout.Append(Dynamic::VertexLayout::Texture2D);
			auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 1);
			hasGlossAlpha = tex->HasAlpha();
			step.AddBindable(std::move(tex));
			pscLayout.Add<Dynamic::Bool>("useGlossAlpha");
			pscLayout.Add<Dynamic::Bool>("useSpecularMap");
		}
		pscLayout.Add<Dynamic::Float3>("specularColor");
		pscLayout.Add<Dynamic::Float>("specularWeight");
		pscLayout.Add<Dynamic::Float>("specularGloss");
	}
	// normal
	{
		if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
		{
			hasTexture = true;
			shaderCode += "Nrm";
			m_vtxLayout.Append(Dynamic::VertexLayout::Texture2D);
			m_vtxLayout.Append(Dynamic::VertexLayout::Tangent);
			m_vtxLayout.Append(Dynamic::VertexLayout::Bitangent);
			step.AddBindable(Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 2));
			pscLayout.Add<Dynamic::Bool>("useNormalMap");
			pscLayout.Add<Dynamic::Float>("normalMapWeight");
		}
	}
	// common (post)
	{
		step.AddBindable(std::make_shared<TransformCbuf>(gfx, 0u));
		step.AddBindable(Blender::Resolve(gfx, false));
		auto pvs = VertexShader::Resolve(gfx, shaderCode + "_VS.cso");
		auto pvsbc = pvs->GetByteCode();
		step.AddBindable(std::move(pvs));
		step.AddBindable(PixelShader::Resolve(gfx, shaderCode + "_PS.cso"));
		step.AddBindable(InputLayout::Resolve(gfx, m_vtxLayout, pvsbc));
		if (hasTexture)
		{
			step.AddBindable(Bind::Sampler::Resolve(gfx));
		}
		// PS material params (cbuf)
		Dynamic::Buffer buf{ std::move(pscLayout) };
		if (auto r = buf["materialColor"]; r.Exists())
		{
			aiColor3D color = { 0.45f,0.45f,0.85f };
			material.Get(AI_MATKEY_COLOR_DIFFUSE, color);
			r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
		}
		buf["useGlossAlpha"].SetIfExists(hasGlossAlpha);
		buf["useSpecularMap"].SetIfExists(true);
		if (auto r = buf["specularColor"]; r.Exists())
		{
			aiColor3D color = { 0.18f,0.18f,0.18f };
			material.Get(AI_MATKEY_COLOR_SPECULAR, color);
			r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
		}
		buf["specularWeight"].SetIfExists(1.0f);
		if (auto r = buf["specularGloss"]; r.Exists())
		{
			float gloss = 8.0f;
			material.Get(AI_MATKEY_SHININESS, gloss);
			r = gloss;
		}
		buf["useNormalMap"].SetIfExists(true);
		buf["normalMapWeight"].SetIfExists(1.0f);
		step.AddBindable(std::make_unique<Bind::CachingPixelConstantBufferEx>(gfx, std::move(buf), 1u));
	}
	phong.AddStep(std::move(step));
	m_Techniques.push_back(std::move(phong));
}
// outline technique
{
Technique outline("Outline",false);
{
	Step mask(1);

	auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
	auto pvsbc = pvs->GetByteCode();
	mask.AddBindable(std::move(pvs));

	// TODO: better sub-layout generation tech for future consideration maybe
	mask.AddBindable(InputLayout::Resolve(gfx, m_vtxLayout, pvsbc));

	mask.AddBindable(std::make_shared<TransformCbuf>(gfx));

	// TODO: might need to specify rasterizer when doubled-sided models start being used

	outline.AddStep(std::move(mask));
}
{
	Step draw(2);

	// these can be pass-constant (tricky due to layout issues)
	auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
	auto pvsbc = pvs->GetByteCode();
	draw.AddBindable(std::move(pvs));

	// this can be pass-constant
	//draw.AddBindable(PixelShader::Resolve(gfx, "Solid_PS.cso"));

	auto pps = PixelShader::Resolve(gfx, "Solid_PS.cso");
	auto ppsbc = pps->GetByteCode();
	draw.AddBindable(std::move(pps));

	Dynamic::RawLayout lay;
	lay.Add<Dynamic::Float4>("color");
	//auto buf = Dynamic::Buffer(std::move(lay));
	auto buf = ShaderReflector::GetBufferByRegister(ppsbc, 1);
	buf["color"] = DirectX::XMFLOAT4{ 1.0f,0.0f,0.0f, 1.0f };
	draw.AddBindable(std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 1u));

	// TODO: better sub-layout generation tech for future consideration maybe
	draw.AddBindable(InputLayout::Resolve(gfx, m_vtxLayout, pvsbc));


	draw.AddBindable(std::make_shared<TransformCbuf>(gfx));

	// TODO: might need to specify rasterizer when doubled-sided models start being used

	outline.AddStep(std::move(draw));
}
	m_Techniques.push_back(std::move(outline));
}*/
}
Dynamic::VertexBuffer Material::ExtractVertices(const aiMesh& mesh) const noexcept
{
	return { m_vtxLayout,mesh };
}
std::vector<unsigned short> Material::ExtractIndices(const aiMesh& mesh) const noexcept
{
	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}
	return indices;
}
std::shared_ptr<Bind::VertexBuffer> Material::MakeVertexBindable(Graphics& gfx, const aiMesh& mesh, float scale) const noxnd
{
	auto vtc = ExtractVertices(mesh);
	if (scale != 1.0f)
	{
		for (auto i = 0u; i < vtc.Size(); i++)
		{
			DirectX::XMFLOAT3& pos = vtc[i].Attr<Dynamic::VertexLayout::ElementType::Position3D>();
			pos.x *= scale;
			pos.y *= scale;
			pos.z *= scale;
		}
	}
	return Bind::VertexBuffer::Resolve(gfx, MakeMeshTag(mesh), std::move(vtc));
}
std::shared_ptr<Bind::IndexBuffer> Material::MakeIndexBindable(Graphics& gfx, const aiMesh& mesh) const noxnd
{
	return Bind::IndexBuffer::Resolve(gfx, MakeMeshTag(mesh), ExtractIndices(mesh));
}
std::string Material::MakeMeshTag(const aiMesh& mesh) const noexcept
{
	return m_ModelPath + "%" + mesh.mName.C_Str();
}
std::vector<Technique> Material::GetTechniques() const noexcept
{
	return m_Techniques;
}