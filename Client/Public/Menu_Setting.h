//#pragma once
//#include "GameObject.h"
//#include "Client_Defines.h"
//#include "UI.h"
//
//BEGIN(Engine)
//class CShader;
//class CRenderer;
//class CVIBuffer_Rect;
//class CTexture;
//END
//
//BEGIN(Client)
//class CUI_Back;
//class CUI_Effect_Back;
//
//class CMenu_Setting final : public CGameObject
//{
//private:
//	struct OFFSETDEST
//	{
//		_float2 vSize = { 0.f, 0.f };
//		_float2 vPos = { 0.f, 0.f };
//	};
//
//private:
//	explicit CMenu_Setting(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	explicit CMenu_Setting(const CMenu_Setting& rhs);
//	virtual ~CMenu_Setting() = default;
//
//public:
//	virtual HRESULT Initialize_Prototype() override;
//	virtual HRESULT Initialize(void* pArg) override;
//	virtual void Tick(_float fTimeDelta) override;
//	virtual void Late_Tick(_float fTimeDelta) override;
//	virtual HRESULT Render() override;
//
//public:
//	void		Set_Open(_bool isOpen);
//
//private:
//	HRESULT Add_Prototype();
//	HRESULT Ready_DefaultTexture();
//	HRESULT Ready_Offset();
//
//private:
//	HRESULT Add_Components(wstring wszTag, QUESTLIST eType);
//
//private:
//	CShader* m_pShaderCom = { nullptr };
//	CRenderer* m_pRendererCom = { nullptr };
//	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
//
//private:
//	vector<CUI_Back*>			m_pTexts;
//	vector<OFFSETDEST>			m_fOffset;
//	vector<CUI*>				m_pUIs;
//	vector<CUI_Effect_Back*>	m_pExplainBack;
//
//	vector<CTexture*>			m_pTextures;
//
//private:
//	_bool		m_isOpen = { false };
//
//public:
//	static CMenu_Setting* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	virtual CGameObject* Clone(void* pArg) override;
//	virtual void Free() override;
//};
//
//END