#pragma once

#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTexture;
class CVIBuffer_Point_Color_Instance;
class CVIBuffer_Rect_Color_Instance;
class CParticleSystem;
class CModel;
class CMeshEffect;
END

BEGIN(Tool)
class CEffect_Window;

class CDummyParticle : public CGameObject
{
	friend class CEffect_Window;

private:
	explicit CDummyParticle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CDummyParticle(const CDummyParticle& _rhs);
	virtual ~CDummyParticle() = default;

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
		m_pParticleSystem->Save(_pDirectoryPath);
	}

	void LoadParticle(const _tchar* _pDirectoryPath) {
		m_pParticleSystem->Load(_pDirectoryPath);
	}

	CModel* ChangeModel(const _tchar* pPrototag);

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	CParticleSystem* m_pParticleSystem = { nullptr };
	CMeshEffect* m_pMeshEffect = { nullptr };
	CRenderer* m_pRenderer = { nullptr };

private: // Prototype_Component_Texture_
	_uint				  m_iNumInstance;
	_bool				  m_isRenderPaticle = { true };
	_bool				  m_isMesh = { false };

public:
	// �������� �Ű����� �߰�. Ŭ�󿡴� �����ϱ�
	static CDummyParticle* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free(void) override;
};
END