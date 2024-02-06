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

		// 콜리전플래그가 맞지 않을경우 기본적인 피직스 처리만 하고 리턴
		// 충돌을 하고싶지 않을경우도 리턴처리
		if (0 == (filterData0.word0 & filterData1.word1) ||
			0 == (filterData0.word1 & filterData1.word0) ||
			END_COL == filterData0.word2 ||
			END_COL == filterData1.word2)
		{
			pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;

			return physx::PxFilterFlag::eSUPPRESS;
		}

		// 둘중 하나가 트리거일경우 트리거 처리로 변경
		if ((attribute0 & physx::PxFilterObjectFlag::eTRIGGER) != 0 ||
			(attribute1 & physx::PxFilterObjectFlag::eTRIGGER) != 0)
		{
			pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
			return physx::PxFilterFlag::eDEFAULT;
		}

		// 충돌처리 Enter, Stay, Exit
		pairFlags = PxPairFlag::eCONTACT_DEFAULT
			| PxPairFlag::eNOTIFY_TOUCH_FOUND
			| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
			| PxPairFlag::eNOTIFY_TOUCH_LOST;
		
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

	//	값의 범위를 제한하는 함수
	//	제한이 되었는지 아닌지도 알수 있다.
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
	// 결과 : [-5.f, 10.f]의 랜덤한 실수가 들어감.
	template<typename T>
	T Random_Generator(T _lowBound, T _highBound)
	{
		if (_lowBound > _highBound)
			std::swap(_lowBound, _highBound);

		// static을 사용하여 같은 엔진이 반복적으로 초기화되지 않도록 합니다.
		static std::default_random_engine RandGenerator(std::chrono::system_clock::now().time_since_epoch().count());

		std::uniform_real_distribution<T> distribution(_lowBound, _highBound);

		return distribution(RandGenerator);
	}

	// 매개변수(_fPercent) 확률로 true를 발생한다.
	// ex) bool isResult = RandomBool(0.66f); // 66% 확률로 true발생
	bool RandomBool(float _fPercent)
	{
		float fRandomFloat = Random_Generator(0.f, 1.f);
		Clamp(_fPercent, 0.f, 1.f); // 인자를 [0, 1]로 값 제한
		return _fPercent > fRandomFloat;
	}

	// 길이 10짜리 문자열을 랜덤으로 만들어주는 함수
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
		// 상대 경로로 변경할 기준 문자열
		const wstring target = L"3DTeamPortfolio";

		// 상대 경로의 시작 부분
		const wstring relativeStart = L"..\\..\\";

		// "3DTeamPortfolio"의 위치를 찾습니다.
		size_t found = absolutePath.find(target);
		if (found != wstring::npos)
		{
			// 상대 경로로 변환
			return relativeStart + absolutePath.substr(found + target.size() + 1);
		}

		// 변환할 대상 문자열이 없는 경우 원래의 절대 경로를 반환합니다.
		return absolutePath;
	}

	std::string ToRelativePath(const char* pAbsolutePath)
	{
		string absolutePath = pAbsolutePath;
		// 상대 경로로 변경할 기준 문자열
		const string target = "3DTeamPortfolio";

		// 상대 경로의 시작 부분
		const string relativeStart = "..\\..\\";

		// "3DTeamPortfolio"의 위치 찾기.
		size_t found = absolutePath.find(target);
		if (found != string::npos)
		{
			// 상대 경로로 변환
			return relativeStart + absolutePath.substr(found + target.size() + 1);
		}

		// 변환할 대상 문자열이 없는 경우 원래의 경로를 반환.
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
	
	// string - > wstring 변환 함수
	std::wstring strToWStr(const std::string& str)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}
	// wstring - > string 변환 함수
	std::string wstrToStr(const std::wstring& wstr)
	{
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}
	/* lowBound와 highBound 사이의 랜덤 값 반환 함수 */
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