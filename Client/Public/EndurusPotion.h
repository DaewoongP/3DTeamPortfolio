//#pragma once
//#include "Item.h"
//#include "Engine_Defines.h"
//#include "Client_Defines.h"
//
//BEGIN(Engine)
//
//class CShader;
//class CTexture;
//class CRenderer;
//class CTransform;
//class CModel;
//class CCoolTime;
////class CDefence;
//END
//
//BEGIN(Client)
//
//class CEndurusPotion final : public CItem
//{
//private:
//	CEndurusPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	CEndurusPotion(const CEndurusPotion& rhs);
//	virtual ~CEndurusPotion() = default;
//
//public:
//	virtual HRESULT Initialize_Prototype(_uint iLevel, const _tchar* pUIImagePath);
//	virtual HRESULT Initialize(void* pArg) override;
////	void Tick(_float fTimeDelta) override;
//
//	void Use(_float3 vPlayPos,void* ExtraValue = nullptr) override;
//
//private:
//	HRESULT Add_Components();
//	HRESULT SetUp_ShaderResources();
//
//private:
//	_bool m_isDrug = { false };
//	_float m_fDrugTime = { 0.f };
//	_uint m_iLevel = { 0 };
//
//	CCoolTime* m_pCooltime = { nullptr };
//	//CDefence* m_pDefence = { nullptr };
//	_int m_iDefValue = { 0 };
//
//
//public:
//	static CEndurusPotion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel, const _tchar* pUIImagePath);
//	virtual CGameObject* Clone(void* pArg) override;
//	virtual void Free(void) override;
//
//
//};
//END
