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
		_float4x4 WorldMatrix; // ���� ���
		_uint iTagLen = { 0 }; // ���ڿ� ����
		_tchar wszTag[MAX_PATH] = TEXT(""); // ������Ʈ ����(�� ������Ʈ �̸�)
		_uint iAnimIndex = { 0 }; // � �ִϸ��̼����� �����Ұ���
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
	
	CTexture* m_pCatTexture = { nullptr }; // Ŀ���� ����� ��ǻ��

private:
	// ����ü �ø��� ���� Bounding Box�� ���� �ϱ����� �ּ�, �ִ� ����
	_float3			m_vMinPoint, m_vMaxPoint, m_vCenterPoint;
	_float			m_fRadius = { 0.f };
	_float			m_fWalkTime = { 0.f }; // �ȴ� �ð�

	_int			m_iRandCatTexture = { 0 }; // ������ ����� �ؽ�ó

private:
	CATDESC	m_ObjectDesc;
	CAT_ANIMATION  m_eCatAnimIndex = { CAT_END };
	CAT_ANIMATION  m_ePreCatAnimIndex = m_eCatAnimIndex;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);

	void Cat_Move(_float fTimeDelta); // ������� ������

public:
	static CCat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END