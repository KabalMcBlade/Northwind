#pragma once

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"
#include "../Core/MemoryWrapper.h"


/*
This is the first class using the smart pointer (ref counter)
Ideally I will extent the usage of smart pointer to any of the classes which a user may use directly,
for instance AABB or sort of that.
*/

NW_NAMESPACE_BEGIN

using UUID = uint64;

class NW_DLL eos::SmartObject;

class NW_DLL Node : public eos::SmartObject
{
public:
	using NodeAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;
	static NodeAllocator* GetAllocator();

public:
	Node();
	explicit Node(const char* _name);
	Node(const nwString& _name);
	virtual ~Node();

	void AttachToParent(eos::SmartPointer<Node, NodeAllocator> _parent);
	void DetachFromParent();

	void SetName(const char* _name);
	void SetName(const nwString& _name);
	const nwString& GetName() const;

	eos::Vector<eos::SmartPointer<Node, NodeAllocator>, NodeAllocator, GetAllocator>& GetChildren();

	void Update(float _dt);

public:
	//////////////////////////////////////////////////////////////////////////
	// User calls
	virtual void OnUpdate(float _deltaTime) {}
	virtual void OnPostpdate(float _deltaTime) {}

private:
	eos::Vector<eos::SmartPointer<Node, NodeAllocator>, NodeAllocator, GetAllocator> m_children;
	eos::SmartPointer<Node, NodeAllocator> m_parent;
	nwString m_name;
	UUID m_uuid;
	uint32 m_index;
};

NW_NAMESPACE_END