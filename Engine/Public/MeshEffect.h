#pragma once

//#include "Composite.h"
#include "GameObject.h"
#include "Model.h"
#include "Ease.h"
BEGIN(Engine)
class CRenderer;
class CTexture;
class CVIBuffer;
class CShader;
class CTransform;
END

BEGIN(Engine)
class ENGINE_DLL CMeshEffect : public CGameObject
{
protected:
	enum PATH { TEXTURE_PATH, ALPHA_CLIP_TEXTURE_PATH, MODEL_PATH, PATH_END};

protected:
	explicit CMeshEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CMeshEffect(const CMeshEffect& _rhs);
	virtual ~CMeshEffect();

public:
	void Set_PassName(string strPassName);
	void Set_Path(wstring wstrPath, PATH ePath);

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath, _uint _iLevel, _float4x4 PivotMatrix = _float4x4());
	virtual HRESULT Initialize(void* _pArg) override;
	void Tick(_float _fTimeDelta) override;
	void Late_Tick(_float _fTimeDelta) override;
	HRESULT Render() override;
	
	
	void Play(_float3 vPos);
	void Stop();
private:
	void Reset();

protected:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

	HRESULT Save(const _tchar* pFilePath);
	HRESULT Load(const _tchar* pFilePath);

protected:
	_uint m_iLevel = { 0 };

	_bool m_isEnble = { false }; // Save
	_float2 m_vStartOffset = { 0.f, 0.f };
	_float2 m_vOffset = { 0.f, 0.f, }; // Save
	_float2 m_vDeltaOffset = { 0.f, 0.f }; // Save

	_float2 m_vStartTiling = { 1.f, 1.f };
	_float2 m_vTililing = { 1.f, 1.f }; // Save
	_float2 m_vDeltaTiling = { 0.f, 0.f }; // Save

	// 색상
	_float4 m_vStartColor = { 1.f, 1.f, 1.f, 1.f }; // Save
	_float4 m_vEndColor = { 1.f, 1.f, 1.f, 1.f }; // Save
	_float4 m_vColor = { 1.f, 1.f, 1.f, 1.f };
	CEase::EASE	m_eColorEase = { CEase::IN_QUAD }; // Save

	// 크기
	_float3 m_vStartSize = { 1.f, 1.f, 1.f }; // Save
	_float3 m_vEndSize = { 1.f, 1.f, 1.f }; // Save
	_float3 m_vSize = { 1.f, 1.f, 1.f }; 
	CEase::EASE	m_eSizeEase = { CEase::IN_QUAD }; // Save

	// 회전
	_float3 m_vStartRot = { 0.f, 0.f, 0.f }; // Save
	_float3 m_vEndRot = { 0.f, 0.f, 0.f }; // Save
	_float3 m_vRot = { 0.f, 0.f, 0.f };
	CEase::EASE	m_eRotEase = { CEase::IN_QUAD }; // Save

	// 이동
	_float3 m_vStartPos = { 0.f, 0.f, 0.f };// Save
	_float3 m_vEndPos = { 0.f, 0.f, 0.f };// Save
	_float3 m_vPos = { 0.f, 0.f, 0.f };
	CEase::EASE	m_ePosEase = { CEase::IN_QUAD };// Save

	_float4x4 m_TransfomationMatrix = { _float4x4() };

	// 나이
	_float m_fLifeTime = { 5.f }; // Save
	_float m_fAge = { 0.f };

	// 렌더관련
	_bool m_isAlphaBlend = { true }; // Save
	string m_strClipChannel = { "Red" }; // Save
	_float m_fClipThreshold = { 0.1f }; // Save
	_bool m_isGlow = { false };
	_bool m_isDistortion = { false };
protected:
	CModel::TYPE m_eAnimType; // Save
	string m_strPassName = { "Default" }; // Save
	wstring m_Path[PATH_END]; // Save
	_bool m_isClipTexture = { false };
	_float4x4 m_PivotMatrix = { _float4x4() }; // Save
	string m_strCurAnim; // Save
	
protected:
	CModel* m_pModel = { nullptr };
	CTexture* m_pTexture = { nullptr };
	CTexture* m_pClipTexture = { nullptr };
	CShader* m_pShader = { nullptr };
	CRenderer* m_pRenderer = { nullptr };

public:
	static CMeshEffect* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const _tchar* pFilePath, _uint _iLevel = 0, _float4x4 PivotMatrix = _float4x4());
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END
