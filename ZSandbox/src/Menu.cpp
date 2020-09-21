#include "Menu.h"
#include "imgui/imgui.h"

bool Menu::GameRunning()
{
	return m_GameRunning;
}

bool Menu::GameStart()
{
	if (m_GameStarted)
	{
		m_GameStarted = false;
		return true;
	}
	return false;
}

void Menu::SetEndGame()
{
	m_GameRunning = false;
	m_GameOver = true;
}

void Menu::Update()
{
	if (!m_GameRunning)
	{
		ImGui::Begin("Menu",(bool*)0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		if (ImGui::Button("Start Game",ImVec2(500,100)) && !m_GameStarted)
		{
			m_GameStarted = true;
			m_GameRunning = true;
			m_GameOver = false;
		}
		else if (ImGui::Button("Exit Game", ImVec2(500, 100)))
		{
			XYZ::Application::Get().Stop();
		}

		if (m_GameOver)
		{
			ImGui::Text("GAME OVER","");
		}
		ImGui::End();
	}
}