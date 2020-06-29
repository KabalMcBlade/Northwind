#include "Node.h"

#include "../Core/Assertions.h"
#include "../Core/SettingsDefines.h"
#include "../Utilities/HashTools.h"

NW_NAMESPACE_BEGIN

#define NW_BASE_NODE_NAME "Node"

static uint32 s_NodeIndex = 0;


Node::NodeAllocator* Node::GetAllocator()
{
	static eos::HeapAreaR nodeMemoryArea(SettingsDefines::Objects::kNodeAllocatorSize);
	static NodeAllocator nodeAllocator(nodeMemoryArea, "NodeMemory");

	return &nodeAllocator;
}

Node::Node() : m_parent(GetAllocator())
{
	m_index = s_NodeIndex;
	++s_NodeIndex;

	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

	uint64 counter = ms.count();
	m_uuid = HashTools::MakeHash64((const void*)&counter, (uint32)sizeof(uint64), m_index);

	nwString name = NW_BASE_NODE_NAME;
	name.append("_");
	name.append(std::to_string(m_index).c_str());
	
	SetName(name);
}

Node::~Node()
{
// 	for (eos::Vector<eos::SmartPointer<Node, NodeAllocator>, NodeAllocator, GetAllocator>::size_type i = 0; i < m_children.size(); ++i)
// 	{
// 		eos::SmartPointer<Node, NodeAllocator>& node = m_children[i];
// 		node->DetachFromParent();
// 	}

	DetachFromParent();
}


Node::Node(const char* _name) : m_parent(GetAllocator())
{
	m_index = s_NodeIndex;
	++s_NodeIndex;

	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

	uint64 counter = ms.count();
	m_uuid = HashTools::MakeHash64((const void*)&counter, (uint32)sizeof(uint64), m_index);

	SetName(_name);
}

Node::Node(const nwString& _name) : m_parent(GetAllocator())
{
	m_index = s_NodeIndex;
	++s_NodeIndex;

	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

	uint64 counter = ms.count();
	m_uuid = HashTools::MakeHash64((const void*)&counter, (uint32)sizeof(uint64), m_index);

	SetName(_name);
}

void Node::AttachToParent(eos::SmartPointer<Node, Node::NodeAllocator> _parent)
{
	if (_parent.IsValid())
	{
		DetachFromParent();
	}

	m_parent = _parent;
	if (m_parent.IsValid())
	{
		eos::Vector<eos::SmartPointer<Node, NodeAllocator>, NodeAllocator, GetAllocator>& children = m_parent->GetChildren();
		children.push_back(eos::SmartPointer<Node, NodeAllocator>(GetAllocator(), this));
	}
}

void Node::DetachFromParent()
{
	if (m_parent.IsValid())
	{
		eos::Vector<eos::SmartPointer<Node, NodeAllocator>, NodeAllocator, GetAllocator>::iterator pendingRemove = std::remove_if(m_parent->GetChildren().begin(), m_parent->GetChildren().end(),
			[&](eos::SmartPointer<Node, NodeAllocator> _object)
		{
			return this == _object;
		}
		);

		if (pendingRemove != m_parent->GetChildren().end())
		{
			m_parent->GetChildren().erase(pendingRemove, m_parent->GetChildren().end());
		}
	}
}

void Node::Update(float _dt)
{
	OnUpdate(_dt);

	eos::Vector<eos::SmartPointer<Node, NodeAllocator>, NodeAllocator, GetAllocator>::const_iterator begin = m_children.cbegin(), end = m_children.cend(), it = begin;
	for (; it != end; ++it)
	{
		(*it)->Update(_dt);
	}

	OnPostpdate(_dt);
}

void Node::SetName(const char* _name)
{
	m_name = _name;
}

void Node::SetName(const nwString& _name)
{
	m_name = _name;
}

const nwString& Node::GetName() const
{
	return m_name;
}

eos::Vector<eos::SmartPointer<Node, Node::NodeAllocator>, Node::NodeAllocator, Node::GetAllocator>& Node::GetChildren()
{
	return m_children;
}

NW_NAMESPACE_END