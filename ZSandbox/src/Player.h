#pragma once
#include <XYZ.h>
#include "Bomb.h"

class Player
{
public:
	Player();
	~Player();
	void Init(std::shared_ptr<XYZ::OrthoCamera>, std::shared_ptr<XYZ::Material> material, glm::vec3 start_pos, std::vector<Bomb>& bombs);
	void UseControlsMode(int);
	void Update(std::vector<Bomb>&, std::vector<std::pair<int, int>>&);
	bool IsDead() const;
	void Destroy();

private:

	bool MoveLeftControlIsPressed() const;
	bool MoveRightControlIsPressed() const;
	bool MoveUpControlIsPressed() const;
	bool MoveDownControlIsPressed() const;
	bool PutBombControlIsPressed() const;


	void PutBombs(XYZ::event_ptr event, std::vector<Bomb>& bombs);


	void MoveLeft();
	void MoveRight();
	void MoveUp();
	void MoveDown();
	Bomb PutBomb();
	void StayStill();


	bool IsUnderExplotion(std::vector<std::pair<int, int>>&) const;

	XYZ::Entity m_Entity;
	std::shared_ptr<XYZ::Material> m_Material;
	XYZ::SpriteAnimationController m_AnimController;
	int m_ControlsMode = -1;
	float m_Speed;
	bool m_IsAlive = true;
	XYZ::HandlerID m_EventHandler;
};


