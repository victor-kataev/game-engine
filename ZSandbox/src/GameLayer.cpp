#include "GameLayer.h"

#include <random>

#include "imgui/imgui.h"
#include <imgui/examples/imgui_impl_glfw.h>
#include <imgui/examples/imgui_impl_opengl3.h>

GameLayer::GameLayer()
{
}

GameLayer::~GameLayer()
{
}

void GameLayer::OnAttach()
{
	XYZ::Audio::Init();
	XYZ::ECSManager::Get().RegisterComponent<XYZ::Relationship>();
	XYZ::ECSManager::Get().RegisterComponent<XYZ::Transform2D>();
	XYZ::ECSManager::Get().RegisterComponent<XYZ::RigidBody2D>();
	XYZ::ECSManager::Get().RegisterComponent<XYZ::GridBody>();
	XYZ::ECSManager::Get().RegisterComponent<XYZ::ParticleEffect2D>();
	XYZ::ECSManager::Get().RegisterComponent<XYZ::Renderable2D>();
	XYZ::ECSManager::Get().RegisterComponent<XYZ::SpriteAnimation>();
	XYZ::ECSManager::Get().RegisterComponent<XYZ::InterpolatedMovement>();
	XYZ::ECSManager::Get().RegisterComponent<XYZ::ActiveComponent>();
	XYZ::ECSManager::Get().RegisterComponent<XYZ::ParticleEmitter>();
	XYZ::ECSManager::Get().RegisterComponent<XYZ::CollisionComponent>();
	XYZ::ECSManager::Get().RegisterComponent<XYZ::RealGridBody>();

	m_PhysicsSystem = XYZ::ECSManager::Get().RegisterSystem<XYZ::PhysicsSystem>();
	m_InterSystem = XYZ::ECSManager::Get().RegisterSystem<XYZ::InterpolatedMovementSystem>();
	m_GridCollisionSystem = XYZ::ECSManager::Get().RegisterSystem<XYZ::GridCollisionSystem>();
	m_GridCollisionSystem->ResizeGrid(21, 21, 1, 0, 0);
	m_RealGridCollisionSystem = XYZ::ECSManager::Get().RegisterSystem<XYZ::RealGridCollisionSystem>();
	m_RealGridCollisionSystem->CreateGrid(21, 21, 1);

	m_ParticleSystem = XYZ::ECSManager::Get().RegisterSystem<XYZ::ParticleSystem2D>();
	m_SpriteSystem = XYZ::ECSManager::Get().RegisterSystem<XYZ::SpriteSystem>();

	XYZ::Renderer::Init();


	m_Camera = std::make_shared<XYZ::OrthoCamera>(0, m_PlayableArea, 0, m_PlayableArea);

	m_Material = XYZ::Material::Create(XYZ::Shader::Create("TextureShader", "Assets/Shaders/DefaultShader.glsl"));
	m_Material->Set("u_Texture", XYZ::Texture2D::Create(XYZ::TextureWrap::Clamp, "Assets/Textures/bomb.png"), 0);
	m_Material->Set("u_Texture", XYZ::Texture2D::Create(XYZ::TextureWrap::Clamp, "Assets/Textures/wall.png"), 1);
	m_Material->Set("u_Texture", XYZ::Texture2D::Create(XYZ::TextureWrap::Repeat, "Assets/Textures/background.png"), 2);


	m_Material->Set("u_Texture", XYZ::Texture2D::Create(XYZ::TextureWrap::Clamp, "Assets/Textures/player_sprite.png"), 3);
	m_Material->Set("u_ViewProjection", m_Camera->GetViewProjectionMatrix());
	m_Material->SetFlags(XYZ::RenderFlags::TransparentFlag);


	


	m_Graph = std::make_shared<XYZ::SceneGraph>();
	m_Test.resize(10);

	for (int i = 0; i < 10; ++i)
	{
		m_Test[i] = XYZ::ECSManager::Get().CreateEntity();
		XYZ::ECSManager::Get().AddComponent(m_Test[i], XYZ::Relationship());
		XYZ::ECSManager::Get().AddComponent(m_Test[i], XYZ::Transform2D(glm::vec3(1)));
	}

	m_Graph->Insert(m_Test[0]);
	m_Graph->Insert(m_Test[1]);
	m_Graph->AttachChild(m_Test[0], m_Test[3]);
	m_Graph->AttachChild(m_Test[0], m_Test[5]);
	m_Graph->AttachChild(m_Test[5], m_Test[6]);
	m_Graph->Insert(m_Test[4]);
	m_Graph->AttachChild(m_Test[0], m_Test[8]);

	m_Graph->Print();



	InitBackgroundParticles();

	m_Audio = XYZ::Audio::Create("Assets/Audio/sample.mp3");
	// Audio setup
	m_Audio->SetLoop(true);
	m_Audio->SetPosition(glm::vec2(0));
	m_Audio->SetGain(5.0f);
	// Play whole track on different thread i guess
	//m_Audio->Play();
}

void GameLayer::OnDetach()
{
	XYZ::Renderer2D::Shutdown();
}

void GameLayer::OnImGuiRender()
{
	m_Menu.Update();
}

void GameLayer::OnUpdate(float dt)
{
	XYZ::RenderCommand::Clear();
	XYZ::RenderCommand::SetClearColor(glm::vec4(0.2, 0.2, 0.5, 1));
	XYZ::Renderer2D::BeginScene(*m_Camera);
	XYZ::Renderer2D::Flush();
	XYZ::Renderer2D::EndScene();

	if (m_Menu.GameStart())
		RestartGame();


	if (m_Menu.GameRunning())
	{
		// Start game loop
		m_RealGridCollisionSystem->Update(dt);
		m_GridCollisionSystem->Update(dt);
		m_PhysicsSystem->Update(dt);
		m_InterSystem->Update(dt);



		m_SpriteSystem->Update(dt);

		m_Material->Set("u_ViewProjection", m_Camera->GetViewProjectionMatrix());
		for (size_t i = 0; i < m_Bombs.size(); ++i)
		{
			if (m_Bombs[i].Update(dt, *m_Camera, m_DamagedCells))
			{
				auto cellsToDamage = m_Bombs[i].InflictDamage();
				m_DamagedCells.insert(std::end(m_DamagedCells), std::begin(cellsToDamage), std::end(cellsToDamage));

				m_Bombs[i].Destroy();
				m_Bombs[i] = m_Bombs[m_Bombs.size() - 1];
				m_Bombs.pop_back();
			}
		}

		
		for (int i = 0; i < 2; i++)
		{
			m_Players[i].Update(m_Bombs, m_DamagedCells);
			if (m_Players[i].IsDead())
				m_Players.erase(m_Players.begin() + i);
		
		}
		if (m_Players.size() == 1)
			m_Menu.SetEndGame();

		m_DamagedCells.clear();

	}
	else
	{

		m_ParticleMaterial->Set("u_ViewProjection", m_Camera->GetViewProjectionMatrix());
		m_ParticleSystem->Update(dt);

	}
}

void GameLayer::RestartGame()
{
	for (size_t i = 0; i < m_Players.size(); ++i)
		m_Players[i].Destroy();
	m_Players.clear();

	for (size_t i = 0; i < m_Bombs.size(); ++i)
		m_Bombs[i].Destroy();
	m_Bombs.clear();

	//m_Map.Init(m_PlayableArea, m_Material);
	//m_Map.Generate(20);

	m_Players.resize(2);
	m_Players[0].Init(m_Camera, m_Material, glm::vec3(10, 10, -0.8f), m_Bombs);
	m_Players[1].Init(m_Camera, m_Material, glm::vec3(2, 2, 0.0f), m_Bombs);

	for (int i = 2; i < m_Players.size(); ++i)
		m_Players[i].Init(m_Camera, m_Material, glm::vec3(10, 10, -0.8f), m_Bombs);

	m_Players[0].UseControlsMode(0);
	m_Players[1].UseControlsMode(1);
}

void GameLayer::InitBackgroundParticles()
{
	int count = 1000;
	m_ParticleEntity = XYZ::ECSManager::Get().CreateEntity();
	m_ParticleMaterial = XYZ::Material::Create(XYZ::Shader::Create("ParticleShader", "Assets/Shaders/Particle/ParticleShader.glsl"));
	m_ParticleMaterial->Set("u_Texture", XYZ::Texture2D::Create(XYZ::TextureWrap::Clamp, "Assets/Textures/bubbles.png"));
	m_ParticleMaterial->SetFlags(XYZ::RenderFlags::InstancedFlag);


	XYZ::ECSManager::Get().AddComponent(m_ParticleEntity,XYZ::ParticleEmitter());
	auto material = XYZ::Material::Create(XYZ::Shader::Create("Assets/Shaders/Particle/ParticleComputeShader.glsl"));
	auto emitter = XYZ::ECSManager::Get().GetComponent<XYZ::ParticleEmitter>(m_ParticleEntity);

	emitter->material = XYZ::MaterialInstance::Create(material);


	emitter->material->Set("u_Emitter", glm::vec2(10, 0));
	emitter->position = glm::vec2(5, 5);
	
	emitter->numParticles = 1000;


	XYZ::ECSManager::Get().AddComponent(m_ParticleEntity, XYZ::ParticleEffect2D(count, material, m_ParticleMaterial));
	auto effect = XYZ::ECSManager::Get().GetComponent<XYZ::ParticleEffect2D>(m_ParticleEntity);


	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_real_distribution<> dist(-1, 1);

	XYZ::ParticleVertex* m_Vertices = new XYZ::ParticleVertex[1000];
	XYZ::ParticleInformation* m_Data = new XYZ::ParticleInformation[1000];

	for (int i = 0; i < 1000; ++i)
	{
		m_Vertices[i].position = glm::vec4(0.02 * m_NumParticles, 20, i * (1.0f / 1000.0f),1);
		m_Vertices[i].color = glm::vec4(0, 1, 1, 1);
		m_Vertices[i].rotation = 0.0f;
		m_Vertices[i].texCoordOffset = glm::vec2(0);

		m_Data[i].defaultPosition.x = m_Vertices[i].position.x;
		m_Data[i].defaultPosition.y = m_Vertices[i].position.y;
		m_Data[i].colorBegin = m_Vertices[i].color;
		m_Data[i].colorEnd = m_Vertices[i].color;
		m_Data[i].sizeBegin = 2.0f;
		m_Data[i].sizeEnd = 0.0f;
		m_Data[i].rotation = dist(rng) * 50;
		m_Data[i].lifeTime = fabs(dist(rng)) * 10;
		m_Data[i].startVelocity = glm::vec2(dist(rng), -0.2f);
		m_Data[i].endVelocity = m_Data[i].startVelocity;
	}
	effect->SetParticlesRange(m_Vertices, m_Data, 0, 1000);
}