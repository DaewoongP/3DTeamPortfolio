#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

#include "UI_Progress1.h"
#include "UI.h"

BEGIN(Engine)
class CHealth;
END

BEGIN(Client)

class CUI_Group_Finisher final : public CGameObject
{
public:
	struct FINISHERDESC
	{
		_tchar	szFilePath[MAX_PATH] = TEXT("");
		CHealth* pHealth = { nullptr };
	};

private:
	enum FINISHER
	{
		FRONT,
		BACK,
		FINISHER_END
	};

private:
	explicit CUI_Group_Finisher(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Group_Finisher(const CUI_Group_Finisher& rhs);
	virtual ~CUI_Group_Finisher() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	_bool	Set_Gauge();

private:
	// 0 = Front, 1 = Back
	vector<class CUI_Back*>		m_pBacks;
	vector<class CUI_Finisher*>	m_pFinishers;

private:
	CHealth*		 m_pHealth = { nullptr };

private:
	HRESULT Add_Prototype();
	HRESULT Add_Components(wstring wszTag);

	HRESULT Read_File(const _tchar* pFilePath, _uint iIndex);
	CUI::UIDESC Load_File(const HANDLE hFile);

private:
	HRESULT Create_Front(void* pArg);
	HRESULT Create_Back(const _tchar* pFIlePath);

public:
	static CUI_Group_Finisher* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END