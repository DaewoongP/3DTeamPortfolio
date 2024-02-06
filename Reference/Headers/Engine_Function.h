#pragma once

namespace Engine
{
	XMFLOAT4 ToColor(float R, float G, float B, float A)
	{
		return XMFLOAT4(R / 255.f, G / 255.f, B / 255.f, A / 255.f);
	}

	void Error_Message_Box(const char* pError)
	{
		wchar_t pReturnError[MAX_PATH] = TEXT("");
		MultiByteToWideChar(CP_ACP, 0, pError, (int)strlen(pError), pReturnError, MAX_PATH);
		MessageBox(nullptr, pReturnError, TEXT("System Message"), MB_OK);
	}

	void Error_Message_Box(const wchar_t* pError)
	{
		MessageBox(nullptr, pError, TEXT("System Message"), MB_OK);
	}

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
			if (0 == lstrcmp(m_pTargetTag, pair.first))
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
		PxPairFlags& pairFlags, const void* constantBlockSize, PxU32 constantBlock)
	{
		std::lock_guard<std::mutex> lock(mtx);

		PX_UNUSED(attribute0);
		PX_UNUSED(attribute1);
		PX_UNUSED(filterData0);
		PX_UNUSED(filterData1);
		PX_UNUSED(constantBlockSize);
		PX_UNUSED(constantBlock);

		// �ݸ����÷��װ� ���� ������� �⺻���� ������ ó���� �ϰ� ����
		// �浹�� �ϰ���� ������쵵 ����ó��
		if (0 == (filterData0.word0 & filterData1.word1) ||
			0 == (filterData0.word1 & filterData1.word0) ||
			END_COL == filterData0.word2 ||
			END_COL == filterData1.word2)
		{
			pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;

			return physx::PxFilterFlag::eSUPPRESS;
		}

		// ���� �ϳ��� Ʈ�����ϰ�� Ʈ���� ó���� ����
		if ((attribute0 & physx::PxFilterObjectFlag::eTRIGGER) != 0 ||
			(attribute1 & physx::PxFilterObjectFlag::eTRIGGER) != 0)
		{
			pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
			return physx::PxFilterFlag::eDEFAULT;
		}

		// �浹ó�� Enter, Stay, Exit
		pairFlags = PxPairFlag::eCONTACT_DEFAULT
			| PxPairFlag::eNOTIFY_TOUCH_FOUND
			| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
			| PxPairFlag::eNOTIFY_TOUCH_LOST;
		
		return PxFilterFlag::eDEFAULT;
	}

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

	//	���� ������ �����ϴ� �Լ�
	//	������ �Ǿ����� �ƴ����� �˼� �ִ�.
	template<typename T>
	inline int Saturate(T& _value, T _min, T _max)
	{
		if (_min > _max)
			std::swap(_min, _max);

		if (_value >= _max)
		{
			_value = _max;
			return 1;
		}
		else if (_value <= _min)
		{
			_value = _min;
			return 0;
		}

		return 0;
	}

	// ex) _float fResult = Random_Generator(-5.f, 10.f)
	// ��� : [-5.f, 10.f]�� ������ �Ǽ��� ��.
	template<typename T>
	T Random_Generator(T _lowBound, T _highBound)
	{
		if (_lowBound > _highBound)
			std::swap(_lowBound, _highBound);

		// static�� ����Ͽ� ���� ������ �ݺ������� �ʱ�ȭ���� �ʵ��� �մϴ�.
		static std::default_random_engine RandGenerator(std::chrono::system_clock::now().time_since_epoch().count());

		std::uniform_real_distribution<T> distribution(_lowBound, _highBound);

		return distribution(RandGenerator);
	}

	// �Ű�����(_fPercent) Ȯ���� true�� �߻��Ѵ�.
	// ex) bool isResult = RandomBool(0.66f); // 66% Ȯ���� true�߻�
	bool RandomBool(float _fPercent)
	{
		float fRandomFloat = Random_Generator(0.f, 1.f);
		Clamp(_fPercent, 0.f, 1.f); // ���ڸ� [0, 1]�� �� ����
		return _fPercent > fRandomFloat;
	}

	// ���� 10¥�� ���ڿ��� �������� ������ִ� �Լ�
	string Generate_Hashtag(bool _isSpSp = false, int _iLength = 15)
	{
		const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
		std::random_device rd;
		std::mt19937 generator(rd());

		std::string result = "";
		if (true == _isSpSp)
		{
			result = "##";
		}

		for (int i = 0; i < _iLength; ++i) {
			result += chars[generator() % chars.size()];
		}

		return result;
	}

	std::wstring Generate_HashtagW(bool _isSpSp = false, int _iLength = 15)
	{
		const std::wstring chars = TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
		std::random_device rd;
		std::mt19937 generator(rd());

		std::wstring result = TEXT("");
		if (true == _isSpSp)
		{
			result = TEXT("##");
		}

		for (int i = 0; i < _iLength; ++i) {
			result += chars[generator() % chars.size()];
		}

		return result;
	}

	std::wstring ToRelativePath(const wchar_t* pAbsolutePath)
	{
		wstring absolutePath = pAbsolutePath;
		// ��� ��η� ������ ���� ���ڿ�
		const wstring target = L"3DTeamPortfolio";

		// ��� ����� ���� �κ�
		const wstring relativeStart = L"..\\..\\";

		// "3DTeamPortfolio"�� ��ġ�� ã���ϴ�.
		size_t found = absolutePath.find(target);
		if (found != wstring::npos)
		{
			// ��� ��η� ��ȯ
			return relativeStart + absolutePath.substr(found + target.size() + 1);
		}

		// ��ȯ�� ��� ���ڿ��� ���� ��� ������ ���� ��θ� ��ȯ�մϴ�.
		return absolutePath;
	}

	std::string ToRelativePath(const char* pAbsolutePath)
	{
		string absolutePath = pAbsolutePath;
		// ��� ��η� ������ ���� ���ڿ�
		const string target = "3DTeamPortfolio";

		// ��� ����� ���� �κ�
		const string relativeStart = "..\\..\\";

		// "3DTeamPortfolio"�� ��ġ ã��.
		size_t found = absolutePath.find(target);
		if (found != string::npos)
		{
			// ��� ��η� ��ȯ
			return relativeStart + absolutePath.substr(found + target.size() + 1);
		}

		// ��ȯ�� ��� ���ڿ��� ���� ��� ������ ��θ� ��ȯ.
		return absolutePath;
	}

	// ex) ToPrototypeTag(TEXT("Prototype_Component_Texture"), "../../Resources/FileName.png");
	std::wstring ToPrototypeTag(const wchar_t* pPrototypeName, const wchar_t* pPath)
	{
		fs::path fsPath = pPath;
		
		wstring wstrPrototypeTag = pPrototypeName; // Prototype_Component_Texture
		if (wstrPrototypeTag.back() != TEXT('_'))
			wstrPrototypeTag += TEXT('_'); // Prototype_Component_Texture_
		wstrPrototypeTag += fsPath.stem().wstring(); // Prototype_Component_Texture_FileName

		return wstrPrototypeTag;
	}
	
	// string - > wstring ��ȯ �Լ�
	std::wstring strToWStr(const std::string& str)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}
	// wstring - > string ��ȯ �Լ�
	std::string wstrToStr(const std::wstring& wstr)
	{
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}
	/* lowBound�� highBound ������ ���� �� ��ȯ �Լ� */
	float GetRandomFloat(float lowBound, float highBound)
	{
		if (lowBound >= highBound)
			return lowBound;

		float f = (rand() % 10000) * 0.0001f;

		return (f * (highBound - lowBound)) + lowBound;
	}

	template<typename T>
	typename std::list<T>::iterator TransitionTo(typename std::list<T>::iterator& _iter, std::list<T>& _source, std::list<T>& _dest)
	{
		typename std::list<T>::iterator next_it = std::next(_iter);
		_dest.splice(_dest.end(), _source, _iter);
		return next_it;
	}

	template<typename T>
	typename std::vector<T>::iterator TransitionTo(typename std::vector<T>::iterator& _iter, std::vector<T>& _source, std::vector<T>& _dest)
	{
		typename std::vector<T>::iterator next_it = std::next(_iter);
		_dest.splice(_dest.end(), _source, _iter);
		return next_it;
	}
}