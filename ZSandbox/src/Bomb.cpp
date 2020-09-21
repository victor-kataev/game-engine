#include "Bomb.h"
#include <random>

Bomb::Bomb()
{
	
}

Bomb::~Bomb()
{

}

void Bomb::Init(int row,int col, std::shared_ptr<XYZ::Material> material)
{
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(3, 6);

	m_Timer = dist(rng);
	
	m_AnimLength = sc_AnimLength;
	m_Entity = XYZ::ECSManager::Get().CreateEntity();

	m_Material = material;


	XYZ::ECSManager::Get().AddComponent(m_Entity, XYZ::Renderable2D(
		m_Material,
		glm::vec4(1),
		glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
		true,
		0
	));

	XYZ::ECSManager::Get().AddComponent(m_Entity, XYZ::SpriteAnimation(1, 12, 720, 60));
	XYZ::ECSManager::Get().AddComponent(m_Entity, XYZ::RigidBody2D(glm::vec2(0)));
	XYZ::ECSManager::Get().AddComponent(m_Entity, XYZ::GridBody(row, col,1,1));
	XYZ::ECSManager::Get().AddComponent(m_Entity, XYZ::CollisionComponent(2, 2));

	auto spriteAnim = XYZ::ECSManager::Get().GetComponent<XYZ::SpriteAnimation>(m_Entity);
	spriteAnim->SetFrameInterval(0, 5, 0.5f);


	m_AnimController.AddAnimation("idle", XYZ::Animation{
		{0,1},
		0.1f
		});

	m_AnimController.AddAnimation("explode", XYZ::Animation{
		{0,10},
		0.2f
		});

	m_AnimController.AddAnimation("destroyed", XYZ::Animation{
		{11,12},
		0.1f
		});
	m_AnimController.StartAnimation("idle");
	m_AnimController.UpdateSpriteAnimation(spriteAnim);
}

bool Bomb::Update(float dt, XYZ::OrthoCamera& camera, std::vector<std::pair<int,int>> damagedCells)
{	
	if (m_Timer > 0.0f)
		m_Timer -= dt;
	else if (m_Timer <= 0.0f)
		return Explode(dt);
	
	return false;
}

bool Bomb::Explode(float dt)
{
	auto spriteAnim = XYZ::ECSManager::Get().GetComponent<XYZ::SpriteAnimation>(m_Entity);
	if (m_AnimLength > 0.0f)
	{
		m_AnimLength -= dt;
		m_AnimController.StartAnimation("explode");
		m_AnimController.UpdateSpriteAnimation(spriteAnim);
		return false;
	}
	else
	{
		auto gridBody = XYZ::ECSManager::Get().GetComponent<XYZ::GridBody>(m_Entity);
		m_AnimController.StartAnimation("destroyed");
		m_AnimController.UpdateSpriteAnimation(spriteAnim);

		m_CellsToDamage.emplace_back(gridBody->row - 1, gridBody->col);
		m_CellsToDamage.emplace_back(gridBody->row - 1, gridBody->col - 1);
		m_CellsToDamage.emplace_back(gridBody->row, gridBody->col - 1);
		m_CellsToDamage.emplace_back(gridBody->row + 1,gridBody->col - 1);
		m_CellsToDamage.emplace_back(gridBody->row + 1,gridBody->col);
		m_CellsToDamage.emplace_back(gridBody->row + 1,gridBody->col + 1);
		m_CellsToDamage.emplace_back(gridBody->row, gridBody->col + 1);
		m_CellsToDamage.emplace_back(gridBody->row - 1, gridBody->col + 1);
		m_CellsToDamage.emplace_back(gridBody->row, gridBody->col);
		// Life time of bomb is at the end, return true and remove bomb from stack
		return true;
	}
}
void Bomb::Destroy()
{
	XYZ::ECSManager::Get().DestroyEntity(m_Entity);
}

std::vector<std::pair<int, int>> Bomb::InflictDamage() const
{
	return m_CellsToDamage;
}