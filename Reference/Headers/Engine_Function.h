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

	/* 구면선형보간 - output, 시작점, 끝점, 점간의 업벡터, 선형보간 중점과 원의 중점간 거리 k, 원하는 보간 값 f*/
	//_float3 GetVectorSlerp(_float3 v1, _float3 v2, _float3 vUp, _float k, _float f)
	//{
	//	_float3 v3, OV1, OV2, vCenter, vNormal1, vNormal2, vOut;
	//	_float fRad;
	//	_float4x4    RotationMatrix = XMMatrixIdentity();

	//	//두 점의 중간값
	//	v3 = (v1 + v2) / 2.f;
	//	//중간에서 시작점 까지의 벡터, 정해둔 축의 외적
	//	vCenter = XMVector3Cross(v1 - v3, vUp);
	//	//노말라이즈
	//	vCenter.Normalize();
	//	//기준이 되는 중점거리(ex/Radius)를 곱하여 중점까지의 거리 벡터로 만듬
	//	vCenter *= k;
	//	//두 점의 중간값 + 중점까지의 거리 벡터 = 중점
	//	vCenter = v3 + vCenter;

	//	//중점에서 두 점을 향한 거리백터
	//	OV1 = v1 - vCenter;
	//	OV2 = v2 - vCenter;

	//	//두 점을 향한 거리백터의 노말라이즈
	//	vNormal1 = XMVector3Normalize(OV1);
	//	vNormal2 = XMVector3Normalize(OV2);

	//	//두 점의 거리벡터의 각을 구하고
	//	fRad = acosf(vNormal1.Dot(vNormal2));
	//	//보간 해줌
	//	fRad *= f;
	//	//보간된 각을 정해둔 축으로 돌리는 보간 행렬
	//	RotationMatrix = XMMatrixRotationAxis(vUp, fRad);

	//	//시작점의 거리 벡터를 보간 행렬로 돌려줌
	//	vOut = XMVector3TransformCoord(OV1, RotationMatrix);

	//	//중점 + 보간된 거리백터 = 보간 위치
	//	return vCenter + vOut;
	//}
}