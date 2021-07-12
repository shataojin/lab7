#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"

// required for IMGUI
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"
#include "Util.h"

PlayScene::PlayScene()
{
	PlayScene::start();
}

PlayScene::~PlayScene()
= default;

void PlayScene::draw()
{
	drawDisplayList();
	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 255, 255, 255, 255);
}

//void PlayScene::moveStarShip() const
//{
//	if(m_bToggleGrid)
//	{
//		m_pStarShip->setDesiredVelocity(m_pTarget->getTransform()->position);
//		m_pStarShip->getRigidBody()->velocity = m_pStarShip->getDesiredVelocity();
//		m_pStarShip->getTransform()->position += m_pStarShip->getRigidBody()->velocity;
//	}
//}

void PlayScene::update()
{
	updateDisplayList();

	m_CheckShipLOS(m_pTarget);
}

void PlayScene::clean()
{
	removeAllChildren();
}

void PlayScene::handleEvents()
{
	EventManager::Instance().update();

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_ESCAPE))
	{
		TheGame::Instance().quit();
	}
}

void PlayScene::start()
{
	// Set GUI Title
	m_guiTitle = "Play Scene";

	// add the ship to the scene as a start point
	m_pship = new Ship();
	m_pship->getTransform() -> position = glm::vec2(200.f, 300.f);
	addChild(m_pship, 3);

	// add the Obstacle to the scene as a start point
	m_obstacle1 = new Obstacle();
	m_obstacle1->getTransform()->position = glm::vec2(400.f, 300.f);
	addChild(m_obstacle1);

	// add the Obstacle to the scene as a start point
	m_obstacle2 = new Obstacle();
	m_obstacle2->getTransform()->position = glm::vec2(400.f, 100.f);
	addChild(m_obstacle2);

	// add the Obstacle to the scene as a start point
	m_obstacle3 = new Obstacle();
	m_obstacle3->getTransform()->position = glm::vec2(400.f,500.f);
	addChild(m_obstacle3);

	// added the target to the scene a goal
	m_pTarget = new Target();
	m_pTarget->getTransform()->position = glm::vec2(600.f, 300.f);
	addChild(m_pTarget);

	ImGuiWindowFrame::Instance().setGUIFunction(std::bind(&PlayScene::GUI_Function, this));
}

void PlayScene::GUI_Function()
{
	// TODO:
	// Toggle Visibility for the StarShip and the Target
		
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);
	
	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();
	
	ImGui::Begin("GAME3001 - M2021 - Lab 7", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar);

	// allow ship rotation
	static int angle;
	if (ImGui::SliderInt("Ship Direction", &angle, -360, 360))
	{
		m_pship->setCurrentHeading(angle);
	}

	ImGui::Separator();

	static int shipPosition[] = { m_pship->getTransform()->position.x, m_pship->getTransform()->position.y };
	if (ImGui::SliderInt2("Ship Position", shipPosition, 0, 800))
	{
		m_pship->getTransform()->position.x = shipPosition[0];
		m_pship->getTransform()->position.y = shipPosition[1];
	}

	static int targetPosition[] = { m_pTarget->getTransform()->position.x, m_pTarget->getTransform()->position.y };
	if (ImGui::SliderInt2("Target Position", targetPosition, 0, 800))
	{
		m_pTarget->getTransform()->position.x = targetPosition[0];
		m_pTarget->getTransform()->position.y = targetPosition[1];
	}

	ImGui::Separator();
	
	ImGui::End();
}

void PlayScene::m_CheckShipLOS(DisplayObject* target_object)
{
	m_pship->setHasLOS(false);
	auto ShipToTargetDistance = Util::distance(m_pship->getTransform()->position, target_object->getTransform()->position);
	if(ShipToTargetDistance<=m_pship->getLOSDistance())
	{
		std::vector < DisplayObject* > contactList;
		for(auto object: getDisplayList())
		{
			auto ShipToObjectDistance = Util::distance(m_pship->getTransform()->position, object->getTransform()->position);
			if(ShipToObjectDistance <= ShipToTargetDistance)
			{
				if ((object->getType() != m_pship->getType()) && ( object->getType() != target_object->getType()))
				contactList.push_back(object);
			}
		}
		contactList.push_back(target_object);
		auto hasLOS = CollisionManager::LOSCheck(m_pship->getTransform()->position,
			m_pship->getTransform()->position + m_pship->getCurrentDirection() * m_pship->getLOSDistance(),
			contactList, target_object);
		m_pship->setHasLOS(hasLOS);
	}
	
}