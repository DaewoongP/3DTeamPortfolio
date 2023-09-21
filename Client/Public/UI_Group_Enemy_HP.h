#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

#include "UI.h"

BEGIN(Engine)
class CTexture;
END


BEGIN(Client)

class CUI_Group_Enemy_HP final : public CGameObject
{
public:
	enum ENEMYTYPE { MONSTER, BOSS, ENEMYTYPE_END };

public:
	struct ENEMYHPDESC
	{
		ENEMYTYPE eType;
		_tchar wszObjectName[MAX_PATH] = TEXT("");
		_tchar wszObjectLevel[MAX_PATH] = TEXT("");
	};

private:
	explicit CUI_Group_Enemy_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Group_Enemy_HP(const CUI_Group_Enemy_HP& rhs);
	virtual ~CUI_Group_Enemy_HP() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	class CUI_Effect_Back*	m_pUI_Effect_Back = { nullptr };
	class CUI_HP*			m_pUI_HP = { nullptr };

	vector<class CUI_Font*>			m_Fonts;

private:
	_tchar			m_wszObjectLevel[MAX_PATH] = TEXT("");
	_tchar			m_wszObjectName[MAX_PATH] = TEXT("");

	ENEMYTYPE		eEnemyType;


private:
	HRESULT Add_Prototype();
	HRESULT Add_Components();
	HRESULT Read_File(const _tchar* pFilePath);
	CUI::UIDESC Load_File(const HANDLE hFile);

	HRESULT Add_Fonts(void* pArg);

public:
	static CUI_Group_Enemy_HP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END