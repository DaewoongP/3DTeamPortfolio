#include "..\Public\ThreadPool.h"

IMPLEMENT_SINGLETON(CThreadPool)

void CThreadPool::Initialize(_uint iNumThreads)
{
	for (size_t i = 0; i < iNumThreads; ++i)
	{
		m_Workers.emplace_back([&]() {
			while (true)
			{
				std::function<void()> task;

				{
					unique_lock<std::mutex> lock(m_Queue_mutex);
					m_Condition.wait(lock,
						[&] { return m_isStop || !m_Tasks.empty(); });
					if (m_isStop && m_Tasks.empty())
						return;
					task = std::move(m_Tasks.front());
					m_Tasks.pop();
				}

				task();
			}
			}
		);
	}

	return;
}

template<class T, class ...Args>
inline auto CThreadPool::Thread_Enqueue(T&& t, Args && ...args) -> std::future<typename std::invoke_result<T, Args ...>::type>
{
	using return_type = typename std::invoke_result <T, Args...>::type;

	auto task = std::make_shared< std::packaged_task<return_type()> >(
		std::bind(std::forward<T>(t), std::forward<Args>(args)...)
		);

	future<return_type> res = task->get_future();
	{
		unique_lock<std::mutex> lock(m_Queue_mutex);

		// don't allow enqueueing after stopping the pool
		if (m_isStop)
			throw runtime_error("enqueue on stopped ThreadPool");

		m_Tasks.emplace([task]() { (*task)(); });
	}

	m_Condition.notify_one();

	return res;
}

void CThreadPool::Free()
{
	{
		unique_lock<std::mutex> lock(m_Queue_mutex);
		m_isStop = true;
	}

	m_Condition.notify_all();

	for (std::thread& worker : m_Workers)
		worker.join();
}
