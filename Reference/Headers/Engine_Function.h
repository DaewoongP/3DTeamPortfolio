#pragma once

namespace Engine
{
	template <typename T>
	unsigned long Safe_AddRef(T& pointer)
	{
		unsigned long	dwRefCnt = 0;

		if (nullptr != pointer)
			dwRefCnt = pointer->AddRef();

		return dwRefCnt;
	}


	template <typename T>
	unsigned long Safe_Release(T& pointer)
	{
		unsigned long	dwRefCnt = 0;

		if (nullptr != pointer)
		{
			dwRefCnt = pointer->Release();

			if (dwRefCnt == 0)
				pointer = nullptr;
		}
		return dwRefCnt;
	}



	template <typename T>
	void Safe_Delete(T& pointer)
	{
		if (nullptr != pointer)
		{
			delete pointer;
			pointer = nullptr;
		}
	}

	template <typename T>
	void Safe_Delete_Array(T& pointer)
	{
		if (nullptr != pointer)
		{
			delete[] pointer;
			pointer = nullptr;
		}
	}

	class CTag_Finder
	{
	public:
		explicit CTag_Finder(const wchar_t* pTag)
			: m_pTargetTag(pTag)
		{
		}
		~CTag_Finder() = default;
	public:
		template<typename T>
		bool operator()(const T& pair)
		{
			if (0 == lstrcmpW(m_pTargetTag, pair.first))
			{
				return true;
			}

			return false;
		}

	private:
		const wchar_t* m_pTargetTag = nullptr;
	};

	// Physx Filter Shader
	inline PxFilterFlags CollisionFilterShader(
		PxFilterObjectAttributes attribute0, PxFilterData filterData0,
		PxFilterObjectAttributes attribute1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void*, PxU32)
	{

		if (PxFilterObjectIsTrigger(attribute0) || PxFilterObjectIsTrigger(attribute1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlag::eDEFAULT;
		}

		pairFlags = PxPairFlag::eCONTACT_DEFAULT;

		// 비트플래그가 And 연산을 통해 값이 남으면 충돌
		if (0 != (filterData0.word0 & filterData1.word0))
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

		return PxFilterFlag::eDEFAULT;
	}

	//Queue.Clear() 없어서 만듬
	template <typename T>
	void Clear_Queue(queue<T>& _Queue)
	{
		//비었다면 탈출
		if (_Queue.empty())
		{
			return;
		}

		//모든 원소지우기
		size_t QueueSize = _Queue.size();

		for (size_t i = 0; i < QueueSize; i++)
		{
			_Queue.pop();
		}
	}

	template <typename T>
	void Clear_Queue_Pointer(queue<T>& _Queue)
	{
		//비었다면 탈출
		if (_Queue.empty())
		{
			return;
		}

		//모든 원소지우기
		size_t QueueSize = _Queue.size();

		for (size_t i = 0; i < QueueSize; i++)
		{
			Safe_Delete(_Queue.front());
			_Queue.pop();
		}
	}

	float Lerp(float fFrom, float fTo, float Alpha)
	{
		return fFrom * (1 - Alpha) + fTo * Alpha;
	}
	
	// 값의 범위를 제한하는 함수
	// ex)
	// int value = 11;
	// Clamp(value, 0, 10);
	// 결과 : value = 10; 
	template<typename T>
	inline void Clamp(T& _value, T _min, T _max)
	{
		if (_min > _max)
			std::swap(_min, _max);

		_value = max(_value, _min);
		_value = min(_value, _max);
	}

	// ex) _float fResult = Random_Generator(-5.f, 10.f)
	// 결과 : [-5.f, 10.f]의 랜덤한 실수가 들어감.
	template<typename T>
	T Random_Generator(T _lowBound, T _highBound)
	{
		// static을 사용하여 같은 엔진이 반복적으로 초기화되지 않도록 합니다.
		static std::default_random_engine RandGenerator(std::chrono::system_clock::now().time_since_epoch().count());

		std::uniform_real_distribution<T> distribution(_lowBound, _highBound);

		return distribution(RandGenerator);
	}
}