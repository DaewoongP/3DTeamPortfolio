#pragma once
#include "Level.h"
#include "Client_Defines.h"

BEGIN(Client)

class CLevel_Cliffside final : public CLevel
{
private:
	explicit CLevel_Cliffside(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Cliffside() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_NPC(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_FieldGuide_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Menu_UI(const _tchar* pLayerTag);
	
private:
	HRESULT Load_MapObject(const _tchar* pObjectFilePath);
	HRESULT Load_MapObject_Ins(const _tchar* pObjectFilePath);
	HRESULT Load_Monsters(const wstring& wstrMonsterFilePath);
	
#ifdef _DEBUG
private:
	HRESULT Ready_Layer_Debug(const _tchar* pLayerTag);
#endif // _DEBUG

public:
	static CLevel_Cliffside* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END