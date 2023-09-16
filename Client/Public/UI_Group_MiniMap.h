#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

#include "UI.h"

BEGIN(Engine)
class CTexture;

END


BEGIN(Client)

class CUI_Group_MiniMap final : public CGameObject
{
private:
	explicit CUI_Group_MiniMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Group_MiniMap(const CUI_Group_MiniMap& rhs);
	virtual ~CUI_Group_MiniMap() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;



private:
	class CUI_Effect_Back*		m_pUI_Effect_Back;
	class CUI_Back*				m_pUI_Back;

	vector<class CTexture*>			m_MapTextures;

public:
	HRESULT		Set_MapTexture(_uint iIndex);

private:
	HRESULT Add_Prototype();
	HRESULT Add_Components();
	HRESULT Read_File(const _tchar* pFilePath);
	CUI::UIDESC Load_File(const HANDLE hFile);

	HRESULT Add_MapTexture();

private:
	_float		m_fMiniMapRotation = { 0.f };
	_float		m_fPlayerRotation = { 0.f };

private:
	_float2		m_vPlayerPos = { 100.f, 100.f };
	_float2		m_vMapSize = { 1024.f, 1024.f };


public:
	static CUI_Group_MiniMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END