#pragma once

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

class Thread final : public eos::NoCopyableMoveable
{
public:
	Thread() = default;

	template<typename FunctionCallback>
	Thread(FunctionCallback _function) noexcept : m_thread(_function)
	{
		m_threadId = m_thread.get_id();
	}

	template<typename FunctionCallback, typename... _Args>
	Thread(FunctionCallback _function, _Args&&... _args) noexcept : m_thread(_function, _args...)
	{
		m_threadId = m_thread.get_id();
	}

	Thread(Thread && _other) noexcept
	{
		std::swap(m_thread, _other.m_thread);
		std::swap(m_threadId, _other.m_threadId);
	}

	Thread & operator=(Thread && _other) noexcept
	{
		Join();

		std::swap(m_thread, _other.m_thread);
		std::swap(m_threadId, _other.m_threadId);

		return *this;
	}

	~Thread()
	{
		Join();
	}

	void Join()
	{
		if (m_thread.joinable())
		{
			m_thread.join();
		}
	}

	void Detach()
	{
		m_thread.detach();
	}

	const std::thread::id& GetID() const
	{
		return m_threadId;
	}

	void Stop()
	{
		Join();
	}

	Thread(const Thread&) = delete;
	Thread& operator=(const Thread&) = delete;

private:
	std::thread m_thread;
	std::thread::id m_threadId;
};

NW_NAMESPACE_END