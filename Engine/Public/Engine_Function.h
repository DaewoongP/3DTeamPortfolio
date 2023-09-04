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

	//Queue.Clear() ��� ����
	template <typename T>
	void Clear_Queue(queue<T>& _Queue)
	{
		//����ٸ� Ż��
		if (_Queue.empty())
		{
			return;
		}

		//��� ���������
		size_t QueueSize = _Queue.size();

		for (size_t i = 0; i < QueueSize; i++)
		{
			_Queue.pop();
		}
	}

	template <typename T>
	void Clear_Queue_Pointer(queue<T>& _Queue)
	{
		//����ٸ� Ż��
		if (_Queue.empty())
		{
			return;
		}

		//��� ���������
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
	
	// ���� ������ �����ϴ� �Լ�
	// ex)
	// int value = 11;
	// Clamp(value, 0, 10);
	// ��� : value = 10; 
	template<typename T>
	inline void Clamp(T& _value, T _min, T _max)
	{
		if (_min > _max)
			std::swap(_min, _max);

		_value = max(_value, _min);
		_value = min(_value, _max);
	}

	// ex) _float fResult = Random_Generator(-5.f, 10.f)
	// ��� : [-5.f, 10.f]�� ������ �Ǽ��� ��.
	template<typename T>
	T Random_Generator(T _lowBound, T _highBound)
	{
		// static�� ����Ͽ� ���� ������ �ݺ������� �ʱ�ȭ���� �ʵ��� �մϴ�.
		static std::default_random_engine RandGenerator(std::chrono::system_clock::now().time_since_epoch().count());

		std::uniform_real_distribution<T> distribution(_lowBound, _highBound);

		return distribution(RandGenerator);
	}

	/* ���鼱������ - output, ������, ����, ������ ������, �������� ������ ���� ������ �Ÿ� k, ���ϴ� ���� �� f*/
	//_float3 GetVectorSlerp(_float3 v1, _float3 v2, _float3 vUp, _float k, _float f)
	//{
	//	_float3 v3, OV1, OV2, vCenter, vNormal1, vNormal2, vOut;
	//	_float fRad;
	//	_float4x4    RotationMatrix = XMMatrixIdentity();

	//	//�� ���� �߰���
	//	v3 = (v1 + v2) / 2.f;
	//	//�߰����� ������ ������ ����, ���ص� ���� ����
	//	vCenter = XMVector3Cross(v1 - v3, vUp);
	//	//�븻������
	//	vCenter.Normalize();
	//	//������ �Ǵ� �����Ÿ�(ex/Radius)�� ���Ͽ� ���������� �Ÿ� ���ͷ� ����
	//	vCenter *= k;
	//	//�� ���� �߰��� + ���������� �Ÿ� ���� = ����
	//	vCenter = v3 + vCenter;

	//	//�������� �� ���� ���� �Ÿ�����
	//	OV1 = v1 - vCenter;
	//	OV2 = v2 - vCenter;

	//	//�� ���� ���� �Ÿ������� �븻������
	//	vNormal1 = XMVector3Normalize(OV1);
	//	vNormal2 = XMVector3Normalize(OV2);

	//	//�� ���� �Ÿ������� ���� ���ϰ�
	//	fRad = acosf(vNormal1.Dot(vNormal2));
	//	//���� ����
	//	fRad *= f;
	//	//������ ���� ���ص� ������ ������ ���� ���
	//	RotationMatrix = XMMatrixRotationAxis(vUp, fRad);

	//	//�������� �Ÿ� ���͸� ���� ��ķ� ������
	//	vOut = XMVector3TransformCoord(OV1, RotationMatrix);

	//	//���� + ������ �Ÿ����� = ���� ��ġ
	//	return vCenter + vOut;
	//}
}