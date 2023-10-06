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
		_float4x4 matTransform; // ���� ���
		_uint iTagLen; // ���ڿ� ����
		_tchar wszTag[MAX_PATH]; // ������Ʈ ����(�� ������Ʈ �̸�)
	}LOADOBJECTDESC;

	typedef struct LoadObjectInsDesc
	{
		_uint iInstanceCnt = { 0 };			// �ν��Ͻ� ����
		_float4x4* pMatTransform;			// ���� ���� ��ĵ��� �ּ�
		_float4x4 WorldMatrix;				// ���� ���
		_uint iTagLen = { 0 };				// ���ڿ� ����
		_tchar wszTag[MAX_PATH] = TEXT(""); // ������Ʈ ����(�� ������Ʈ �̸�)
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
	vector<_float4x4*>		m_vecFreeMatrix; // ���� �Ҵ� ���� �뵵

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