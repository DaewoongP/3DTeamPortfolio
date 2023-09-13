
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
	virtual HRESULT Initialize_Prototype(const _tchar* _pDirectoryPath, _uint _iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Enable() { m_isEnable = true; }
	void Disable() { m_isEnable = false; }

protected: /* For. Component */
	_uint m_iLevel = { 0 };
	string	m_strPass = "Default";
	_bool m_isEnable = { true };

	_uint     m_iTrailNum = { 50 };
	_float4x4 m_PivotMatrix = _float4x4();
	_float4x4 m_HighLocalMatrix = _float4x4();
	_float4x4 m_LowLocalMatrix = _float4x4();
	list<TRAIL_POINT>	m_TrailGroup;
	// ����
	_float3 m_vHeadColor = { 1.f, 1.f, 1.f };
	_float3 m_vTailColor = { 1.f, 1.f, 1.f };

	// �ʺ�
	_float	m_fWidth = { 1.f };

	// ���� ���ӽð�
	_float m_fTailDuration = { 0.032f };
	_float m_fTimeAcc = { 0.f };
	// Ŀ��

protected:
	CRenderer* m_pRenderer = { nullptr };
	CVIBuffer_Rect_Trail* m_pBuffer = { nullptr };
	CShader* m_pShader = { nullptr };
	CTexture* m_pTexture = { nullptr };
	CTexture* m_pGradientTexture = { nullptr };
	wstring m_wstrPath = { TEXT("../../Resources/Effects/Textures/Default_Particle.png") };

protected:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* _pDirectoryPath, _uint _iLevel = 0);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END

