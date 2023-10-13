#pragma once
#include "Level.h"
#include "Client_Defines.h"

BEGIN(Client)

class CLevel_Vault final : public CLevel
{
private:
	explicit CLevel_Vault(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Vault() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Events(const _tchar* pLayerTag);
	HRESULT Ready_Lights();

private:
	HRESULT Load_MapObject(const _tchar* pObjectFilePath);
	HRESULT Load_MapObject_Ins(const _tchar* pObjectFilePath);
	HRESULT Load_Monsters(const wstring& wstrMonsterFilePath);

public:
	static CLevel_Vault* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END