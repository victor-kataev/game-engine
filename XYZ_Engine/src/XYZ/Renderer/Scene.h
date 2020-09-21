#pragma once

#include "XYZ/ECS/ECSManager.h"
#include "XYZ/Renderer/OrthoCamera.h"
#include "XYZ/AssetManager/AssetManager.h"

#include <set>

namespace XYZ
{
    /*! @class Scene
    *	@brief Holds all data relevant to a Scene
    */
    class Scene
    {
    private:
        std::string m_Name;
        OrthoCamera m_Camera;
        AssetManager& m_AssetMgr;
        ECSManager& m_ECSMgr;
        std::set<Entity> m_Entities;
    public:
        Scene(std::string& name, std::vector<std::string>& filesTextures,
            std::vector<std::string>& filesShaders,
            std::vector<std::string>& filesAudio);
        ~Scene();

        inline const std::string& GetName() const { return m_Name; }
        inline const OrthoCamera& GetCamera() const { return m_Camera; }
        inline auto& GetEntities() { return m_Entities; };

        /** Adds an entity to the scene */
        void AddEntity(const Entity& ent);

        /** Triggers when the scene is being rendered */
        void OnRender();

        /** Triggers when the scene is attached */
        void OnAttach();

        /** Triggers when the scene is detached */
        void OnDetach();
    };
}