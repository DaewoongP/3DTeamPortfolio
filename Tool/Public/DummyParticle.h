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
	// 툴에서만 프로토타입에서 파티클 경로를 받아줌.
	HRESULT Initialize_Prototype(const _tchar* _pDirectoryPath);
	// 클라에서는 이니셜라이즈에서 경로 받아줘서 초기화 하면 됨
	virtual HRESULT Initialize(void* _pArg) override;

	HRESULT Render() override;
private:
// 파티클 텍스처 변경.
	void ChangeTexture(CTexture** _pTexture, wstring& _wstrOriginPath, const _tchar* _pDestPath);

	// 인스턴스 수를 변경
	void RemakeBuffer(_uint iNumInstance);

public:
	void Tick_Imgui(_float _fTimeDelta);
	void MainMoudle_TreeNode(CEffect_Window* pEffectWindow);
	void EmissionModule_TreeNode(CEffect_Window* pEffectWindow);
	void ShapeModule_TreeNode(CEffect_Window* pEffectWindow);
	void RendererModule_TreeNode(CEffect_Window* pEffectWindow);
	void ColorOverLifeTime_TreeNode(CEffect_Window* pEffectWindow);
	void SizeOverLifeTime_TreeNode(CEffect_Window* pEffectWindow);
	void RotationOverLifetimeModule_TreeNode(CEffect_Window* pEffectWindow);
	void TextureSheetAnimationModule_TreeNode(CEffect_Window* pEffectWindow);
	void Save_FileDialog();
	void Load_FileDialog();
	void Load_After();
	virtual void Restart() override;

	Vector3 QuaternionToEuler(const Quaternion& q) {
		Vector3 angles;

		// roll (x-axis rotation)
		float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
		float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
		angles.z = atan2(sinr_cosp, cosr_cosp);

		// pitch (y-axis rotation)
		float sinp = 2 * (q.w * q.y - q.z * q.x);
		if (fabs(sinp) >= 1)
			angles.x = copysign(XM_PI / 2, sinp); // use 90 degrees if out of range
		else
			angles.x = asin(sinp);

		// yaw (z-axis rotation)
		float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
		float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
		angles.y = atan2(siny_cosp, cosy_cosp);

		return angles;
	}


private:
	_float3 vShapePosition = { _float3() };
	_float3 vShapeRotation = {  };
	_float3 vShapeScale = { 2.f, 2.f, 2.f };
	Quaternion vShapeQuaternion = { Quaternion() };

private:
	class CComboBox* m_pEmitterVelocity_ComboBox = { nullptr };
	class CComboBox* m_pShapeCombo = { nullptr };
	class CComboBox* m_pMeshModeCombo = { nullptr };
	class CComboBox* m_pSpriteTypeCombo = { nullptr };
	class CComboBox* m_pThetaModeCombo = { nullptr };
	class CComboBox* m_pPhiModeCombo = { nullptr };
	class CComboBox* m_pMeshCombo = { nullptr };
	class CComboBox* m_pConeEmitFromCombo = { nullptr };
	class CComboBox* m_pBoxEmitFromCombo = { nullptr };
	class CComboBox* m_pMeshTypeCombo = { nullptr };
	class CComboBox* m_pStopActionCombo = { nullptr };
	class CComboBox* m_pClipChannelCombo = { nullptr };
	vector<CComboBox*> m_pEaseCombo;

	class CImageFileDialog* m_pMaterialTextureIFD = { nullptr };
	class CImageFileDialog* m_pAlphaTextureIFD = { nullptr };
	class CImageFileDialog* m_pSpriteTextureIFD = { nullptr };
	class CImageFileDialog* m_pNormalTextureIFD = { nullptr };
	_bool m_isPrevLooping = { false };
	list<_tchar*> m_pTags;

public:
	// 툴에서는 매개변수 추가. 클라에는 삭제하기
	static CDummyParticle* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pDirectoryPath);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free(void) override;
};
END