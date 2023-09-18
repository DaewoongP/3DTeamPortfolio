#pragma once
#include "Level.h"
#include "Client_Defines.h"

BEGIN(Client)

class CLevel_MainGame final : public CLevel
{
private:
	explicit CLevel_MainGame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_MainGame() = default;

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
	HRESULT Ready_Layer_Effect(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);

private:
	HRESULT Load_MapObject(const _tchar* pObjectFilePath);

#ifdef _DEBUG
private:
	HRESULT Ready_Layer_Debug(const _tchar* pLayerTag);
	HRESULT Ready_Layer_SceneTest(const _tchar* pLayerTag);
#endif // _DEBUG

public:
	static CLevel_MainGame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END