#include "stdafx.h"
#include "Scene.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "RenderCommand.h"

XYZ::Scene::Scene(std::string& name, std::vector<std::string>& filesTextures, std::vector<std::string>& filesShaders, std::vector<std::string>& filesAudio)
	: m_Name(name), m_Camera(OrthoCamera(1.0f, 1.0f, 1.0f, 1.0f)), m_AssetMgr(AssetManager::Get()), m_ECSMgr(ECSManager::Get())
{
	for (size_t i = 0; i < filesShaders.size(); ++i)
		m_AssetMgr.LoadShader(std::to_string(i), filesShaders[i]);
	for (size_t i = 0; i < filesTextures.size(); ++i)
		m_AssetMgr.LoadTexture(TextureWrap::Clamp, std::to_string(i), filesTextures[i]);
}

XYZ::Scene::~Scene() {}

void XYZ::Scene::AddEntity(const Entity& ent)
{
	m_Entities.insert(ent);
}

void XYZ::Scene::OnRender()
{
	RenderCommand::Clear();
	RenderCommand::SetClearColor(glm::vec4(0.2, 0.2, 0.5, 1));

	Renderer2D::BeginScene(m_Camera);

	Renderer2D::Flush();
	Renderer2D::EndScene();
}

void XYZ::Scene::OnAttach()
{
	Renderer::Init();
}

void XYZ::Scene::OnDetach()
{
	Renderer2D::Shutdown();
	for (const auto& x : m_Entities) m_ECSMgr.DestroyEntity(x);
}