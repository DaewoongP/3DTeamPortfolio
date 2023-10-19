#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CMesh;
class CModel;
class CShader;
class CTexture;
class CRenderer;
END

BEGIN(Client)

class CCat final : public CGameObject
{
	enum CAT_ANIMATION {
		CAT_IDLE1, CAT_IDLE2, CAT_IDLE3,
		CAT_TURN, CAT_WALK, CAT_END
	};

public:
	typedef struct tagCatDesc
	{
		_float4x4 WorldMatrix; // 상태 행렬
		_uint iTagLen = { 0 }; // 문자열 길이
		_tchar wszTag[MAX_PATH] = TEXT(""); // 오브젝트 종류(모델 컴포넌트 이름)
		_uint iAnimIndex = { 0 }; // 어떤 애니메이션으로 동작할건지
	}CATDESC;

private:
	explicit CCat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCat(const CCat& rhs);
	virtual ~CCat() = default;

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix) override;

private:
	CShader* m_pShader = { nullptr };
	CShader* m_pShadowShader = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CModel* m_pModel = { nullptr };
	
	CTexture* m_pCatTexture = { nullptr }; // 커스텀 고양이 디퓨즈

private:
	// 절두체 컬링을 위해 Bounding Box를 생성 하기위한 최소, 최대 정점
	_float3			m_vMinPoint, m_vMaxPoint, m_vCenterPoint;
	_float			m_fRadius = { 0.f };
	_float			m_fWalkTime = { 0.f }; // 걷는 시간

	_int			m_iRandCatTexture = { 0 }; // 랜덤한 고양이 텍스처

private:
	CATDESC	m_ObjectDesc;
	CAT_ANIMATION  m_eCatAnimIndex = { CAT_END };
	CAT_ANIMATION  m_ePreCatAnimIndex = m_eCatAnimIndex;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);

	void Cat_Move(_float fTimeDelta); // 고양이의 움직임

public:
	static CCat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END