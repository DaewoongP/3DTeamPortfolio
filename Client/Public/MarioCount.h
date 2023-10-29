#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CMarioCount final : public CGameObject
{
private:
	explicit CMarioCount(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMarioCount(const CMarioCount& rhs);
	virtual ~CMarioCount() = default;

public:
	void Start();
	_bool Is_Finished();

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*			m_pShader = { nullptr };
	CTexture*			m_pTexture = { nullptr };
	CRenderer*			m_pRenderer = { nullptr };
	CVIBuffer_Rect*		m_pBuffer = { nullptr };

private:
	_float4x4			m_ViewMatrix, m_ProjMatrix;

private:
	_bool				m_isStart = { false };
	_bool				m_isFinished = { false };
	_uint				m_iCountIndex = { 0 };
	_float				m_fTime = { 0.f };
	_float				m_fTimeAcc = { 0.f };

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	static CMarioCount* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END