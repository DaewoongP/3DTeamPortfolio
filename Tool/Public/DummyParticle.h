#pragma once

#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTexture;
class CVIBuffer_Point_Color_Instance;
class CVIBuffer_Color_Instance;
class CParticleSystem;
END

BEGIN(Tool)
typedef CVIBuffer_Color_Instance::COLORINSTANCE COL_INSTANCE;

class CDummyParticle : public CGameObject
{
private:
	explicit CDummyParticle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CDummyParticle(const CDummyParticle& _rhs);
	virtual ~CDummyParticle() = default;

public:
	CParticleSystem* Get_ParticleSystem() { return m_pParticle; }

public:
	// �������� ������Ÿ�Կ��� ��ƼŬ ��θ� �޾���.
	HRESULT Initialize_Prototype();
	// Ŭ�󿡼��� �̴ϼȶ������ ��� �޾��༭ �ʱ�ȭ �ϸ� ��
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	void SaveParticle(const _tchar* _pDirectoryPath) {
		m_pParticle->Save(_pDirectoryPath);
	}

	void LoadParticle(const _tchar* _pDirectoryPath) {
		m_pParticle->Load(_pDirectoryPath);
	}

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	CShader* m_pShader = { nullptr };
	CTexture* m_pTexture = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CVIBuffer_Point_Color_Instance* m_pBuffer = { nullptr };
	CParticleSystem* m_pParticle = { nullptr };

private:
	_uint				  m_iNumInstance;

public:
	// �������� �Ű����� �߰�. Ŭ�󿡴� �����ϱ�
	static CDummyParticle* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free(void) override;
};
END