#include "..\Public\ThreadPool.h"

HRESULT CThreadPool::Initialize(_uint iNumThreads)
{
	/*for (size_t i = 0; i < threads; ++i)
		workers.emplace_back(
			[this]
			{
				for (;;)
				{
					std::function<void()> task;

					{
						std::unique_lock<std::mutex> lock(this->queue_mutex);
						this->condition.wait(lock,
							[this] { return this->stop || !this->tasks.empty(); });
						if (this->stop && this->tasks.empty())
							return;
						task = std::move(this->tasks.front());
						this->tasks.pop();
					}

					task();
				}
			}
			);*/
	return E_NOTIMPL;
}

template<class T, class ...Args>
inline auto CThreadPool::enqueue(T&& t, Args && ...args) -> std::future<typename std::invoke_result<T, Args ...>::type>
{
	return std::future<typename std::invoke_result<T, Args ...>::type>();
}

void CThreadPool::Free()
{
}
