#pragma once

#include "GameObject.h"
#include "Engine_Defines.h"
#include "Trail.h"
#include "VIBuffer_Rect_Trail.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect_Trail;

END

BEGIN(Engine)

class ENGINE_DLL CTrail : public CGameObject
{
protected:
	typedef CVIBuffer_Rect_Trail::TRAILDESC TRAILDESC;

private:
	typedef list<VTXPOSTEX>::iterator TRAIL_IT;

public:
	typedef struct tagTrailPoint {
		_float3 vPosition;
		_float fLifetime;
	}TRAIL_POINT;

protected:
	CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail(const CTrail& rhs);
	virtual ~CTrail() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* _pFilePath, const _tchar* _pGradientTexturePath, _uint _iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Enable() { m_isEnable = true; }
	void Disable() { m_isEnable = false; }
	HRESULT Reset_Trail(_float3 vHighPos, _float3 vLowPos) { return m_pBuffer->Reset_Trail(vHighPos, vLowPos); }
	HRESULT Reset_Trail() {
		if (m_pBuffer != nullptr)
			return m_pBuffer->Reset_Trail();
		return E_FAIL;
	}
	void Set_Trail_HeadColor(_float3 vColor) { m_vHeadColor = vColor; }
	void Set_Trail_TailColor(_float3 vColor) { m_vTailColor = vColor; }
	void Set_LocalSpace(const _float4x4* matrix) { m_LocalSpace = matrix; }
	void Set_Pass(string str) { m_strPass = str; }
	void Set_Threshold(_float value) { m_fClipThreshold = value; }
public:
	/* For. Moving*/
	void	Stright_Move(_float3 vTargerPosition, _float3 vStartPosition, _float fLerpAcc);
	void	Spin_Move(_float3 vTargerPosition, _float3 vStartPosition, _float fLerpAcc);
	void	Spline_Move(_float3 vSpline01, _float3 vStartPosition, _float3 vTargerPosition, _float3 vSpline02, _float fLerpAcc);

	void	Spline_Spin_Move(_float3 vSpline01, _float3 vStartPosition, _float3 vTargerPosition, _float3 vSpline02, _float fLerpAcc);

public:
	/* For. Lightning*/
	void	Ready_LightningStrike(_float3 vStartPosition, _float3 vEndPosition, _float3 vWeight[2], _uint iCount);
	void	Tick_LightningStrike(_float fTimeDelta);
protected:
	HRESULT Save(const _tchar* pFilePath);
	HRESULT Load(const _tchar* pFilePath);

protected:
	// For. Default Trail Setting
	_uint m_iLevel = { 0 };
	string	m_strPass = "Default";
	wstring m_wstrPath = { TEXT("../../Resources/Effects/Textures/Trails/Winga.png") };
	wstring m_wstrGradientTextureName = {};
	_bool m_isEnable = { true };

	_uint     m_iTrailNum = { 50 };
	_float4x4 m_PivotMatrix = _float4x4();
	_float4x4 m_HighLocalMatrix = _float4x4();
	_float4x4 m_LowLocalMatrix = _float4x4();
	list<TRAIL_POINT>	m_TrailGroup;

	// 색상
	_float3 m_vHeadColor = { 1.f, 1.f, 1.f };
	_float3 m_vTailColor = { 0.91f, 0.169f, 0.859f };

	// 너비
	_float	m_fWidth = { 1.f };

	// 꼬리 지속시간
	_float m_fTailDuration = { 0.032f };
	_float m_fTimeAcc = { 0.f };

	//스레스 홀드
	_float m_fClipThreshold = { 0.2f };
	string m_strClipChannel = { "Red" };
	_bool m_isUseTextureColor = { false };

	//로컬스페이스용
	const _float4x4* m_LocalSpace = { nullptr };

	// For. Lightning Trail
	_bool m_isLightning = { false };
	vector<_float3> m_vSplineLerpPostion;
	vector<_float3> m_vSplineDir;

protected:
	CRenderer* m_pRenderer = { nullptr };
	CVIBuffer_Rect_Trail* m_pBuffer = { nullptr };
	CShader* m_pShader = { nullptr };
	CTexture* m_pTexture = { nullptr };
	CTexture* m_pGradientTexture = { nullptr };

protected:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* _pDirectoryPath, _uint _iLevel = 0,
		const _tchar* _pGradientTexturePath  = TEXT("../../Resources/Effects/Textures/Gradients/CustomLinearGradient.png"));
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END

