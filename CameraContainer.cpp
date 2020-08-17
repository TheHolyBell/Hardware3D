#include "CameraContainer.h"
#include "Camera.h"
#include "Graphics.h"
#include "RenderGraph.h"
#include "ImGui/imgui.h"

void CameraContainer::SpawnWindow(Graphics& gfx)
{
	if (ImGui::Begin("Cameras"))
	{
		if (ImGui::BeginCombo("Active Camera", (*this)->GetName().c_str()))
		{
			for (int i = 0; i < std::size(m_Cameras); i++)
			{
				const bool isSelected = i == m_Active;
				if (ImGui::Selectable(m_Cameras[i]->GetName().c_str(), isSelected))
				{
					m_Active = i;
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::BeginCombo("Controlled Camera", GetControlledCamera().GetName().c_str()))
		{
			for (int i = 0; i < std::size(m_Cameras); i++)
			{
				const bool isSelected = i == m_Controlled;
				if (ImGui::Selectable(m_Cameras[i]->GetName().c_str(), isSelected))
				{
					m_Controlled = i;
				}
			}
			ImGui::EndCombo();
		}

		GetControlledCamera().SpawnControlWidgets(gfx);
	}
	ImGui::End();
}

void CameraContainer::Bind(Graphics& gfx)
{
	gfx.SetCamera((*this)->GetMatrix());
}

void CameraContainer::AddCamera(std::shared_ptr<Camera> pCamera)
{
	m_Cameras.push_back(std::move(pCamera));
}

Camera* CameraContainer::operator->()
{
	return &GetActiveCamera();
}

CameraContainer::~CameraContainer()
{
}

void CameraContainer::LinkTechniques(RenderGraph::RenderGraph& renderGraph)
{
	for (auto& pcam : m_Cameras)
		pcam->LinkTechniques(renderGraph);
}

void CameraContainer::Submit(size_t channels) const
{
	for (size_t i = 0; i < m_Cameras.size(); i++)
	{
		if (i != m_Active)
		{
			m_Cameras[i]->Submit(channels);
		}
	}
}

Camera& CameraContainer::GetActiveCamera()
{
	return *m_Cameras[m_Active];
}

Camera& CameraContainer::GetControlledCamera()
{
	return *m_Cameras[m_Controlled];
}
