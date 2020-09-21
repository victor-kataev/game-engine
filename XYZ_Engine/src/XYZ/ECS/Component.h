#pragma once
#include "Types.h"

namespace XYZ {
	class ComponentManager;
	/* !@class IComponent
	* @brief interface of component
	*/
	class IComponent
	{
		friend class ComponentManager;
	public:
		virtual uint16_t GetComponentID() const = 0;

	protected:
		template <typename T>
		static uint16_t GetID()
		{
			static uint16_t compType = getNextID();
			return compType;
		}
	private:
		static uint16_t getNextID()
		{
			static uint16_t nextType = 0;
			return ++nextType;
		}
	};


	/* !@class Type
	* @brief type of component
	*/
	template <typename Derived, typename DeriveFrom = IComponent>
	class Type : public IComponent
	{
	public:
		uint16_t GetComponentID() const override
		{
			return IComponent::GetID<Derived>();
		}
	};

	struct ActiveComponent : public Type<ActiveComponent>
	{
		Signature activeComponents;
	};

	struct Relationship : public Type<Relationship>
	{
		size_t numChildren = 0;
		Entity parent = NULL_ENTITY;
		Entity previous = NULL_ENTITY;
		Entity next = NULL_ENTITY;
		Entity first = NULL_ENTITY;
	};
}