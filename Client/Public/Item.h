#pragma once

#include "GameObject.h"
#include "CustomModel.h"
#include "Engine_Defines.h"
#include "Player_Information.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CCoolTime;
class CVIBuffer_Rect;
class CHealth;
END

BEGIN(Client)
class CPlayer;

class CItem abstract : public CGameObject
{
protected:
	typedef struct tagItemCloneDesc
	{
		
	}ITEM_CLONE_DESC;

protected:
	typedef struct tagItemCreateDesc
	{
		_uint			iCost = { 0 };
		_float			fDuration = { 0.f };
		wstring			wstrKoreanName = { TEXT("") };
		wstring			wstrUIPath = { TEXT("") };
		ITEM_ID			eItemId = { ITEM_ID_END };
		ITEMTYPE		eItemType = { ITEMTYPE_END };
	}ITEM_CREATE_DESC;

protected:
	explicit CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CItem(const CItem& rhs);
	virtual ~CItem() = default;

public:
	HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Change_Position(_float fX, _float fY, _float fZ, _uint iWinSizeX = 1280.f, _uint iWinSizeY = 720.f);
	HRESULT Change_Scale(_float fX, _float fY);

	void Set_Enable(_bool isEnable) { m_isEnable = isEnable; }
	void Set_Alpha(_float fAlpha) { m_fAlphaRatio = fAlpha; }

public:
	ITEM_ID Get_ItemID() { return m_ItemCreateDesc.eItemId; }
	CTexture* Get_UITexture() { return m_pUITexture; }
	ITEMTYPE Get_Type() { return m_ItemCreateDesc.eItemType; }
	const _tchar* Get_KoreanName() {
		return m_ItemCreateDesc.wstrKoreanName.data();
	}

	virtual _bool Buy();
	void ToLayer(_int iLevel = -1, const _tchar* ComTag = ::Generate_HashtagW().data(), const _tchar* LayerTag = TEXT("Layer_Item"));
	static CItem* SimpleFactory(ITEM_ID eItemID, _uint iLevel = LEVEL_STATIC, void* pArg = nullptr);

protected:
	_uint		m_iLevel = { 0 };
	_bool		m_isEnable = { false };
	_float		m_fAlphaRatio = { 1.f };

protected:
	ITEM_CREATE_DESC m_ItemCreateDesc;
	CPlayer* m_pPlayer = { nullptr };

protected:
	CRenderer*		m_pRenderer = { nullptr };
	CTexture*		m_pUITexture = { nullptr };
	CShader*		m_pUIShader = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer = { nullptr };

protected: // Player Components
	class CCustomModel* m_pPlayerModel = { nullptr };
	class CTransform* m_pPlayerTransform = { nullptr };
	class CPlayer_Information* m_pPlayerInformation = { nullptr };
	
	_float4x4	m_ViewMatrix = { _float4x4() };
	_float4x4	m_ProjMatrix = { _float4x4() };

protected:
	_float			m_fX = { 0.f };
	_float			m_fY = { 0.f };
	_float			m_fZ = { 0.f };
	_float			m_fSizeX = { 0.f };
	_float			m_fSizeY = { 0.f };

private:
	HRESULT Add_Components();
	HRESULT Set_ShaderResources();
public:
	virtual void Free(void) override;
};
END
