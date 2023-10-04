#pragma once

#include "GameObject.h"
#include "CustomModel.h"
#include "Engine_Defines.h"
#include "Client_Defines.h"
#include "Player_Information.h"

BEGIN(Engine)
class CTexture;
class CModel;
class CRenderer;
class CShader;
END

BEGIN(Client)
enum ITEMTYPE
{
	HAND,
	FACE,
	HEAD,
	NECK,
	BACK,
	OUTFIT,
	RESOURCE,
	POTION,
	ITEMTYPE_END
};

class CItem abstract : public CGameObject
{
protected:
	typedef struct tagItemCloneDesc
	{
		class CPlayer* pPlayer = { nullptr };
	}ITEM_CLONE_DESC;

protected:
	typedef struct tagItemCreateDesc
	{
		_uint			iCost = { 0 };
		_float			fDuration = { 0.f };
		wstring			wstrKoreanName = { TEXT("") };
		wstring			wstrUIPath = { TEXT("") };
		wstring			wstrModelPath = { TEXT("") };
		CModel::TYPE	eModelType = { CModel::TYPE_END };
		_float4x4		PivotMatrix = { _float4x4() };
		ITEMTYPE		eItemType = { ITEMTYPE_END };
	}ITEM_CREATE_DESC;

protected:
	explicit CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CItem(const CItem& rhs);
	virtual ~CItem() = default;

public:
	HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;


public:
	CTexture* Get_UITexture() { return m_pUITexture; }
	ITEMTYPE Get_Type() { return m_ItemCreateDesc.eItemType; }

protected:
	_uint		m_iLevel = { 0 };

protected:
	ITEM_CREATE_DESC m_ItemCreateDesc;
	CPlayer* m_pPlayer = { nullptr };

protected:
	CRenderer*	m_pRenderer = { nullptr };
	CTexture*	m_pUITexture = { nullptr };
	CModel*		m_pModel = { nullptr };
	CShader*	m_pShader = { nullptr };

protected: // Player Components
	class CCustomModel* m_pPlayerModel = { nullptr };
	class CTransform* m_pPlayerTransform = { nullptr };
	class CPlayer_Information* m_pPlayerInformation = { nullptr };

private:
	HRESULT Add_Components();

public:
	virtual void Free(void) override;
};
END
