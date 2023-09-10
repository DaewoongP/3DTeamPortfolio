#pragma once

#include "ParticleSystem.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTexture;
class CVIBuffer_Point_Color_Instance;
class CVIBuffer_Rect_Color_Instance;
class CModel;
class CMeshEffect;
END

BEGIN(Tool)
class CEffect_Window;

class CDummyParticle : public CParticleSystem
{
	friend class CEffect_Window;

private:
	explicit CDummyParticle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CDummyParticle(const CDummyParticle& _rhs);
	virtual ~CDummyParticle() = default;

public:
	// �������� ������Ÿ�Կ��� ��ƼŬ ��θ� �޾���.
	HRESULT Initialize_Prototype(const _tchar* _pDirectoryPath);
	// Ŭ�󿡼��� �̴ϼȶ������ ��� �޾��༭ �ʱ�ȭ �ϸ� ��
	virtual HRESULT Initialize(void* _pArg) override;

	HRESULT Render() override;
private:
// ��ƼŬ �ؽ�ó ����.
	void ChangeTexture(CTexture** _pTexture, wstring& _wstrOriginPath, const _tchar* _pDestPath);

	// �ν��Ͻ� ���� ����
	void RemakeBuffer(_uint iNumInstance);

public:
	void Tick_Imgui(_float _fTimeDelta);
	void MainMoudle_TreeNode(CEffect_Window* pEffectWindow);
	void EmissionModule_TreeNode(CEffect_Window* pEffectWindow);
	void ShapeModule_TreeNode(CEffect_Window* pEffectWindow);
	void RendererModule_TreeNode(CEffect_Window* pEffectWindow);
	void Save_FileDialog();
	void Load_FileDialog();
	virtual void Restart() override;

private:
	class CComboBox* m_pEmitterVelocity_ComboBox = { nullptr };
	class CComboBox* m_pShapeCombo = { nullptr };
	class CComboBox* m_pMeshModeCombo = { nullptr };
	class CComboBox* m_pSpriteTypeCombo = { nullptr };
	class CComboBox* m_pArcModeCombo = { nullptr };
	class CComboBox* m_pMeshCombo = { nullptr };
	class CComboBox* m_pConeEmitFromCombo = { nullptr };
	class CComboBox* m_pBoxEmitFromCombo = { nullptr };
	class CComboBox* m_pMeshTypeCombo = { nullptr };
	class CComboBox* m_pClipChannelCombo = { nullptr };

	class CImageFileDialog* m_pMaterialTextureIFD = { nullptr };
	class CImageFileDialog* m_pAlphaTextureIFD = { nullptr };
	class CImageFileDialog* m_pSpriteTextureIFD = { nullptr };
	_bool m_isPrevLooping = { false };
	list<_tchar*> m_pTags;

public:
	// �������� �Ű����� �߰�. Ŭ�󿡴� �����ϱ�
	static CDummyParticle* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pDirectoryPath);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free(void) override;
};
END