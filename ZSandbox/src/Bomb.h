#pragma once
#include <XYZ.h>


class Bomb
{
public:
	Bomb();
	~Bomb();

	void Init(int row,int col,std::shared_ptr<XYZ::Material> material);
	void Destroy();
	bool Update(float dt,XYZ::OrthoCamera& camera, std::vector<std::pair<int, int>> damagedCells);
	bool Explode(float dt);
	std::vector<std::pair<int, int>> Bomb::InflictDamage() const;


	bool operator ==(const Bomb& other) const
	{
		return m_Entity == other.m_Entity;
	}
	float m_Timer;
	XYZ::Entity m_Entity;
	std::shared_ptr<XYZ::Material> m_Material;
	XYZ::SpriteAnimationController m_AnimController;
	std::vector<std::pair<int, int>> m_CellsToDamage;


	float m_AnimLength;
	static constexpr float sc_AnimLength = 2.0f;
};