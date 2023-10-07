#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMain1_Loader final : public CBase
{
	typedef struct LoadObjectDesc
	{
		_float4x4 matTransform; // 상태 행렬
		_uint iTagLen; // 문자열 길이
		_tchar wszTag[MAX_PATH]; // 오브젝트 종류(모델 컴포넌트 이름)
	}LOADOBJECTDESC;

	typedef struct LoadObjectInsDesc
	{
		_uint iInstanceCnt = { 0 };			// 인스턴스 개수
		_float4x4* pMatTransform;			// 각각 상태 행렬들의 주소
		_float4x4 WorldMatrix;				// 상태 행렬
		_uint iTagLen = { 0 };				// 문자열 길이
		_tchar wszTag[MAX_PATH] = TEXT(""); // 오브젝트 종류(모델 컴포넌트 이름)
	}LOADOJBECTINSDESC;

private:
	explicit CMain1_Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMain1_Loader() = default;

public:
	const HANDLE Get_Thread() const { return m_hThread; }
	_bool Get_Finished() const { return m_isFinished; }

public:
	HRESULT Initialize(LEVELID eNextLevel, _bool isStaticLoaded);
	HRESULT Loading();
	
private:
	HANDLE					m_hThread = { 0 };
	CRITICAL_SECTION		m_Critical_Section;

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

private:
	LEVELID					m_eNextLevelID = { LEVELID::LEVEL_END };
	_bool					m_isFinished = { false };
	_bool					m_isStaticLoaded = { false };

private:
	vector<_float4x4*>		m_vecFreeMatrix; // 동적 할당 해제 용도

private:
	HRESULT Loading_For_Logo(LEVELID eLevelID);
	HRESULT Loading_For_Cliffside(LEVELID eLevelID);
	HRESULT Loading_For_Vault(LEVELID eLevelID);
	HRESULT Loading_For_GreatHall(LEVELID eLevelID);
	HRESULT Loading_For_Hogsmeade(LEVELID eLevelID);

private:
	HRESULT Loading_For_Static(LEVELID eLevelID);
	
private:
	HRESULT Loading_Map_Object(const _tchar* pMapObjectPath, LEVELID eID);
	HRESULT Loading_Map_Object_Ins(const _tchar* pMapObjectInsPath, LEVELID eID);

public:
	static CMain1_Loader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel, _bool isStaticLoaded);
	virtual void Free() override;
};

END