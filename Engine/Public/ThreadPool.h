#pragma once
#include "Base.h"

BEGIN(Engine)

class CThreadPool final : public CBase
{
	DECLARE_SINGLETON(CThreadPool)

private:
	explicit CThreadPool() = default;
	virtual ~CThreadPool() = default;

public:
	void Initialize(_uint iNumThreads);
	template<class T, class... Args>
	auto Thread_Enqueue(T&& t, Args&&... args)
		->std::future<typename std::invoke_result<T, Args...>::type>;

private:
	vector<thread>			m_Workers;
	queue<function<void()>> m_Tasks;

	std::mutex				m_Queue_mutex;
	condition_variable		m_Condition;
	_bool					m_isStop = { false };

public:
	virtual void Free() override;
};

END