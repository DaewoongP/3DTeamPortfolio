#pragma once
#include "Level.h"
#include "Client_Defines.h"

BEGIN(Client)

class CLevel_GreatHall final : public CLevel
{
private:
	explicit CLevel_GreatHall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GreatHall() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);

private:
	HRESULT Load_MapObject(const _tchar* pObjectFilePath);
	HRESULT Load_MapObject_Ins(const _tchar* pObjectFilePath);
	HRESULT Load_Monsters(const wstring& wstrMonsterFilePath);

public:
	static CLevel_GreatHall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END