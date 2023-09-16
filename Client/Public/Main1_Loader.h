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
private:
	explicit CMain1_Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMain1_Loader() = default;

public:
	const HANDLE Get_Thread() const { return m_hThread; }
	_bool Get_Finished() const { return m_isFinished; }

public:
	HRESULT Initialize(LEVELID eNextLevel);
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

private:
	HRESULT Loading_For_Logo();
	HRESULT Loading_For_MainGame();
	
private:
	HRESULT Loading_Map_Object(const _tchar* pMapObjectPath);

public:
	static CMain1_Loader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel);
	virtual void Free() override;
};

END