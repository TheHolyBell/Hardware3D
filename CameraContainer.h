#pragma once
#include <vector>
#include <memory>

class Camera;
class Graphics;

namespace RenderGraph
{
	class RenderGraph;
}

class CameraContainer
{
public:
	void SpawnWindow(Graphics& gfx);
	void Bind(Graphics& gfx);
	void AddCamera(std::shared_ptr<Camera> pCamera);
	Camera* operator->();
	~CameraContainer();
	void LinkTechniques(RenderGraph::RenderGraph& renderGraph);
	void Submit(size_t channels) const;
	Camera& GetActiveCamera();
private:
	Camera& GetControlledCamera();

private:
	std::vector<std::shared_ptr<Camera>> m_Cameras;
	int m_Active = 0;
	int m_Controlled = 0;
};