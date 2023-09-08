//#pragma once
//
//#include "Base.h"
//
//BEGIN(Engine)
//
//class ENGINE_DLL CShader_Node final : public CBase
//{
//private:
//	CShader_Node(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
//	virtual ~CShader_Node() = default;
//
//public:
//	virtual HRESULT Initialize();
//	
//
//private:
//	_bool m_isActivate = { false };
//
//private:
//	ID3D11Device* m_pDevice = { nullptr };
//	ID3D11DeviceContext* m_pContext = { nullptr };
//
//public:
//	static CShader_Node* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
//	virtual void Free() override;
//};
//
//END
