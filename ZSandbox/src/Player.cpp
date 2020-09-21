#include "Player.h"

Player::Player()
{	
}

Player::~Player()
{

}

void Player::Init(std::shared_ptr<XYZ::OrthoCamera>, std::shared_ptr<XYZ::Material> material, glm::vec3 start_pos, std::vector<Bomb>& bombs)
{
	m_Speed = 3.0f; 
	m_Material = material;
	m_Entity = XYZ::ECSManager::Get().CreateEntity();


	XYZ::ECSManager::Get().AddComponent(m_Entity, XYZ::Renderable2D(
		m_Material,
		glm::vec4(1),
		glm::vec4(0.0f, 0.0f, 1.0f / 8.0f, 1.0f / 3.0f),
		true,
		3
	));

	XYZ::ECSManager::Get().AddComponent(m_Entity, XYZ::Transform2D(start_pos));
	XYZ::ECSManager::Get().AddComponent(m_Entity, XYZ::SpriteAnimation(3, 8, 240, 90));
	XYZ::ECSManager::Get().AddComponent(m_Entity, XYZ::RigidBody2D(glm::vec2(0,0)));
	XYZ::ECSManager::Get().AddComponent(m_Entity, XYZ::GridBody((int)start_pos.y, (int)start_pos.x,1,1));
	XYZ::ECSManager::Get().AddComponent(m_Entity, XYZ::InterpolatedMovement(glm::vec2(0)));
	XYZ::ECSManager::Get().AddComponent(m_Entity, XYZ::CollisionComponent(1,1));
	XYZ::ECSManager::Get().AddComponent(m_Entity, XYZ::RealGridBody(start_pos.x,start_pos.x + 1,start_pos.y,start_pos.y + 1));


	auto spriteAnim = XYZ::ECSManager::Get().GetComponent<XYZ::SpriteAnimation>(m_Entity);
	spriteAnim->SetFrameInterval(0, 5, 0.5f);

	m_AnimController.AddAnimation("idle", XYZ::Animation{
		std::pair<int,int>(16, 17),
		0.3f
		});
	m_AnimController.AddAnimation("walkright", XYZ::Animation{
		std::pair<int,int>(8,9),
		0.3f
		});
	m_AnimController.AddAnimation("walkleft", XYZ::Animation{
		std::pair<int,int>(10,11),
		0.3f
		});
	m_AnimController.AddAnimation("walkup", XYZ::Animation{
		std::pair<int,int>(22,23),
		0.3f
		});
	m_AnimController.AddAnimation("walkdown", XYZ::Animation{
		std::pair<int,int>(20,21),
		0.3f
		});

	m_AnimController.StartAnimation("idle");
	m_AnimController.UpdateSpriteAnimation(spriteAnim);
	m_EventHandler = XYZ::EventManager::Get().AddHandler(XYZ::EventType::KeyPressed, std::bind(&Player::PutBombs, this, std::placeholders::_1, std::ref(bombs)));
}

void Player::UseControlsMode(int constrolsMode)
{
	m_ControlsMode = constrolsMode;
}

bool Player::MoveLeftControlIsPressed() const
{
	if (m_ControlsMode % 2 == 0)
		return XYZ::Input::IsKeyPressed(XYZ::KeyCode::XYZ_KEY_LEFT);
	else
		return XYZ::Input::IsKeyPressed(XYZ::KeyCode::XYZ_KEY_A);
}

bool Player::MoveRightControlIsPressed() const
{
	if (m_ControlsMode % 2 == 0)
		return XYZ::Input::IsKeyPressed(XYZ::KeyCode::XYZ_KEY_RIGHT);
	else
		return XYZ::Input::IsKeyPressed(XYZ::KeyCode::XYZ_KEY_D);
}

bool Player::MoveUpControlIsPressed() const
{
	if (m_ControlsMode % 2 == 0)
		return XYZ::Input::IsKeyPressed(XYZ::KeyCode::XYZ_KEY_UP);
	else
		return XYZ::Input::IsKeyPressed(XYZ::KeyCode::XYZ_KEY_W);
}

bool Player::MoveDownControlIsPressed() const
{
	if (m_ControlsMode % 2 == 0)
		return XYZ::Input::IsKeyPressed(XYZ::KeyCode::XYZ_KEY_DOWN);
	else
		return XYZ::Input::IsKeyPressed(XYZ::KeyCode::XYZ_KEY_S);
}

bool Player::PutBombControlIsPressed() const
{
	if (m_ControlsMode % 2 == 0)
		return XYZ::Input::IsKeyPressed(XYZ::KeyCode::XYZ_KEY_ENTER);
	else
		return XYZ::Input::IsKeyPressed(XYZ::KeyCode::XYZ_KEY_SPACE);
}

void Player::Update(std::vector<Bomb>& bombs, std::vector<std::pair<int, int>>& damagedCells)
{
	if (IsUnderExplotion(damagedCells))
	{
		//dying animation
		std::cout << "Player dead" << std::endl;
		m_IsAlive = false;
		Destroy();
		return;
	}

	auto spriteInter = XYZ::ECSManager::Get().GetComponent<XYZ::InterpolatedMovement>(m_Entity);
	auto gridBody = XYZ::ECSManager::Get().GetComponent<XYZ::GridBody>(m_Entity);
	gridBody->nextCol = 0;
	gridBody->nextRow = 0;
	auto rigid = XYZ::ECSManager::Get().GetComponent<XYZ::RigidBody2D>(m_Entity);

	rigid->velocity.x = 0;
	rigid->velocity.y = 0;

	if (!spriteInter->inProgress)
	{
		StayStill();

		if (MoveLeftControlIsPressed())
			MoveLeft();
		else if (MoveRightControlIsPressed())
			MoveRight();


		if (MoveUpControlIsPressed())
			MoveUp();
		else if (MoveDownControlIsPressed())
			MoveDown();
	}
}

void Player::PutBombs(XYZ::event_ptr event, std::vector<Bomb>& bombs)
{
	std::shared_ptr<XYZ::KeyPressedEvent> e = std::static_pointer_cast<XYZ::KeyPressedEvent>(event);
	
	if (m_IsAlive)
	{
		if (m_ControlsMode % 2 == 0)
		{
			if (e->GetKey() == XYZ::KeyCode::XYZ_KEY_ENTER)
			{
				bombs.push_back(PutBomb());
				e->SetHandled();
			}
		}
		else
		{
			if (e->GetKey() == XYZ::KeyCode::XYZ_KEY_SPACE)
			{
				bombs.push_back(PutBomb());
				e->SetHandled();
			}
		}
	}
}


void Player::MoveLeft()
{
	auto gridBody = XYZ::ECSManager::Get().GetComponent<XYZ::GridBody>(m_Entity);
	auto spriteAnim = XYZ::ECSManager::Get().GetComponent<XYZ::SpriteAnimation>(m_Entity);
	auto spriteInter = XYZ::ECSManager::Get().GetComponent<XYZ::InterpolatedMovement>(m_Entity);
	auto rigid = XYZ::ECSManager::Get().GetComponent<XYZ::RigidBody2D>(m_Entity);

	rigid->velocity.x = -1;
	gridBody->nextCol = -1;
	spriteInter->velocity.x = -m_Speed;

	m_AnimController.StartAnimation("walkleft");
	m_AnimController.UpdateSpriteAnimation(spriteAnim);
}

void Player::MoveRight()
{
	auto gridBody = XYZ::ECSManager::Get().GetComponent<XYZ::GridBody>(m_Entity);
	auto spriteAnim = XYZ::ECSManager::Get().GetComponent<XYZ::SpriteAnimation>(m_Entity);
	auto spriteInter = XYZ::ECSManager::Get().GetComponent<XYZ::InterpolatedMovement>(m_Entity);
	auto rigid = XYZ::ECSManager::Get().GetComponent<XYZ::RigidBody2D>(m_Entity);

	rigid->velocity.x = 1;
	gridBody->nextCol = 1;
	spriteInter->velocity.x = m_Speed;
	
	m_AnimController.StartAnimation("walkright");
	m_AnimController.UpdateSpriteAnimation(spriteAnim);
}

void Player::MoveUp()
{
	auto gridBody = XYZ::ECSManager::Get().GetComponent<XYZ::GridBody>(m_Entity);
	auto spriteAnim = XYZ::ECSManager::Get().GetComponent<XYZ::SpriteAnimation>(m_Entity);
	auto spriteInter = XYZ::ECSManager::Get().GetComponent<XYZ::InterpolatedMovement>(m_Entity);
	auto rigid = XYZ::ECSManager::Get().GetComponent<XYZ::RigidBody2D>(m_Entity);

	rigid->velocity.y = 1;
	gridBody->nextRow = 1;
	spriteInter->velocity.y = m_Speed;

	m_AnimController.StartAnimation("walkup");
	m_AnimController.UpdateSpriteAnimation(spriteAnim);
}

void Player::MoveDown()
{
	auto gridBody = XYZ::ECSManager::Get().GetComponent<XYZ::GridBody>(m_Entity);
	auto spriteAnim = XYZ::ECSManager::Get().GetComponent<XYZ::SpriteAnimation>(m_Entity);
	auto spriteInter = XYZ::ECSManager::Get().GetComponent<XYZ::InterpolatedMovement>(m_Entity);
	auto rigid = XYZ::ECSManager::Get().GetComponent<XYZ::RigidBody2D>(m_Entity);

	rigid->velocity.y = -1;
	gridBody->nextRow = -1;
	spriteInter->velocity.y = -m_Speed;

	m_AnimController.StartAnimation("walkdown");
	m_AnimController.UpdateSpriteAnimation(spriteAnim);
}

void Player::StayStill()
{
	auto gridBody = XYZ::ECSManager::Get().GetComponent<XYZ::GridBody>(m_Entity);
	auto spriteAnim = XYZ::ECSManager::Get().GetComponent<XYZ::SpriteAnimation>(m_Entity);

	m_AnimController.StartAnimation("idle");
	m_AnimController.UpdateSpriteAnimation(spriteAnim);
}

Bomb Player::PutBomb()
{
	Bomb bomb;
	auto gridPos = XYZ::ECSManager::Get().GetComponent<XYZ::GridBody>(m_Entity);
	bomb.Init(gridPos->row, gridPos->col, m_Material);
	return bomb;
}

void Player::Destroy()
{
	XYZ::ECSManager::Get().DestroyEntity(m_Entity);
	XYZ::EventManager::Get().RemoveHandler(XYZ::EventType::KeyPressed, m_EventHandler);
}

bool Player::IsUnderExplotion(std::vector<std::pair<int, int>>& damagedCells) const
{
	auto gridPos = XYZ::ECSManager::Get().GetComponent<XYZ::GridBody>(m_Entity);
	for (const auto& cell : damagedCells)
		if (cell.first == gridPos->row && cell.second == gridPos->col)
			return true;
	return false;
}

bool Player::IsDead() const
{
	return !m_IsAlive;
}