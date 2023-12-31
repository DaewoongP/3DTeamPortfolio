#pragma once
///////////////////////// My Macros /////////////////////////////
#define			PURE							= 0
#define			MAX_SHADERMATRIX				512
#define			MAX_SHADERVECTOR				512
#define			MAX_STR							256
#define			MAX_DIK							256
#define			ZEROMEM(_ptr)					ZeroMemory(_ptr, sizeof *_ptr)
#define			ZEROMEMSIZE(_ptr, _size)		ZeroMemory(_ptr, sizeof _size)
#define			CharToWChar(_pChar, _pWchar)	MultiByteToWideChar(CP_ACP, 0, _pChar, MAX_PATH, _pWchar, MAX_PATH);
#define         WCharToChar(_pWchar, _pChar)    WideCharToMultiByte(CP_ACP, 0, _pWchar, -1, _pChar, MAX_PATH, nullptr, nullptr);
#define			MAX_PARTICLE_NUM				300
#define			WHITEDEFAULT					_float4(1.f,1.f,1.f,1.f)
#define			BLACKDEFAULT					_float4(0.f,0.f,0.f,0.f)
///////////////////////// default Macros /////////////////////////////
#ifndef			MSG_BOX
#define			MSG_BOX(_message)			Error_Message_Box(_message)
#endif

#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			END						}

#define			USING(NAMESPACE)	using namespace NAMESPACE;

#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif

#define NULL_CHECK( _ptr)	\
	{if( _ptr == 0){__debugbreak();return;}}

#define NULL_CHECK_RETURN( _ptr, _return)	\
	{if( _ptr == 0){__debugbreak();return _return;}}

#define NULL_CHECK_MSG( _ptr, _message )		\
	{if( _ptr == 0){MessageBox(nullptr, _message, L"System Message",MB_OK);__debugbreak();}}

#define NULL_CHECK_RETURN_MSG( _ptr, _return, _message )	\
	{if( _ptr == 0){MessageBox(nullptr, _message, L"System Message",MB_OK);__debugbreak();return _return;}}


#define FAILED_CHECK(_hr)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(nullptr, L"Failed", L"System Error",MB_OK); __debugbreak(); return E_FAIL;}

#define FAILED_CHECK_RETURN(_hr, _return)	if( ((HRESULT)(_hr)) < 0 )		\
	{ MessageBoxW(nullptr, L"Failed", L"System Error",MB_OK); __debugbreak(); return _return;}

#define FAILED_CHECK_MSG( _hr, _message)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(nullptr, _message, L"System Message",MB_OK); __debugbreak();return E_FAIL;}

#define FAILED_CHECK_RETURN_MSG( _hr, _return, _message)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(nullptr, _message, L"System Message",MB_OK); __debugbreak();return _return;}



#define NO_COPY(CLASSNAME)										\
		private:												\
		CLASSNAME(const CLASSNAME&) = delete;					\
		CLASSNAME& operator = (const CLASSNAME&) = delete;		

#define DECLARE_SINGLETON(CLASSNAME)							\
		NO_COPY(CLASSNAME)										\
		private:												\
		static CLASSNAME*	m_pInstance;						\
		public:													\
		static CLASSNAME*	GetInstance( void );				\
		static unsigned long DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)							\
		CLASSNAME*	CLASSNAME::m_pInstance = nullptr;			\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
			if(nullptr == m_pInstance) {						\
				m_pInstance = New CLASSNAME;					\
			}													\
			return m_pInstance;									\
		}														\
		unsigned long CLASSNAME::DestroyInstance( void ) {		\
			unsigned long dwRefCnt = { 0 };						\
			if(nullptr != m_pInstance)							\
			{													\
				dwRefCnt = m_pInstance->Release();				\
				if(0 == dwRefCnt)								\
					m_pInstance = { nullptr };					\
			}													\
			return dwRefCnt;									\
		}