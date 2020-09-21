#pragma once

//-----Core-----//
#include "XYZ/Core/Application.h"
//#include "XYZ/Core/Layer.h"
#include "XYZ/Core/Input.h"
#include "XYZ/Core/KeyCodes.h"
#include "XYZ/Core/MouseCodes.h" 
#include "XYZ/Timer.h"

//-----Renderer-----//
#include "XYZ/Renderer/OrthoCamera.h"
#include "XYZ/Renderer/OrthoCameraController.h"
#include "XYZ/Renderer/Renderer2D.h"
#include "XYZ/Renderer/Renderer.h"
#include "XYZ/Renderer/RenderCommand.h"
#include "XYZ/Renderer/Material.h"
#include "XYZ/Renderer/SpriteAnimation.h"
#include "XYZ/Renderer/SpriteAnimationController.h"
#include "XYZ/Renderer/SpriteSystem.h"
#include "XYZ/Renderer/MaterialManager.h"
#include "XYZ/Renderer/SceneGraph.h"

//------ECS-----//
#include "XYZ/ECS/ECSManager.h"

//-----Systems-----//
#include "XYZ/Physics/PhysicsSystem.h"
#include "XYZ/Physics/GridCollisionSystem.h"
#include "XYZ/Physics/RealGridCollisionSystem.h"
#include "XYZ/Physics/InterpolatedMovementSystem.h"
#include "XYZ/Particle/ParticleSystem2D.h"

//-----ImGui-----//
#include "XYZ/ImGui/ImGuiLayer.h"

//-----Audio-----//
#include "XYZ/Audio/Audio.h"