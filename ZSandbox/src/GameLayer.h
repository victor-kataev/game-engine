#pragma once

#include <XYZ.h>

#include "Bomb.h"
#include "Map.h"
#include "Player.h"
#include "Menu.h"

class GameLayer : public XYZ::Layer
{
public:
	GameLayer();
	virtual ~GameLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float ts) override;
	virtual void OnImGuiRender() override;

private:
	void RestartGame();
	void InitBackgroundParticles();
private:
	std::shared_ptr<XYZ::PhysicsSystem> m_PhysicsSystem;
	std::shared_ptr<XYZ::ParticleSystem2D> m_ParticleSystem;
	std::shared_ptr<XYZ::SpriteSystem> m_SpriteSystem;
	std::shared_ptr<XYZ::InterpolatedMovementSystem> m_InterSystem;
	std::shared_ptr<XYZ::GridCollisionSystem> m_GridCollisionSystem;
	std::shared_ptr<XYZ::RealGridCollisionSystem> m_RealGridCollisionSystem;

	std::shared_ptr<XYZ::Material> m_Material;
	std::shared_ptr<XYZ::Material> m_ParticleMaterial;
	std::shared_ptr<XYZ::OrthoCamera> m_Camera;


	// Testing environment
	std::vector<Player> m_Players;
	std::vector<Bomb> m_Bombs;

	XYZ::Entity m_ParticleEntity;


	int m_NumParticles = 0;
	int m_PlayableArea = 20;
	Map m_Map;
	Menu m_Menu;
	XYZ::HandlerID m_PutBomb;
	std::vector<std::pair<int, int>> m_DamagedCells;

	std::shared_ptr<XYZ::AudioSource> m_Audio;


	std::shared_ptr<XYZ::SceneGraph> m_Graph;
	std::vector<XYZ::Entity> m_Test;
};