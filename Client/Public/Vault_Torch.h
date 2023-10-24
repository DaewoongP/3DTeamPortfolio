#pragma once
#include "MapObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CMesh;
class CModel;
class CShader;
class CRenderer;
class CParticleSystem;
class CLight;
END

BEGIN(Client)

class CVault_Torch final : public CMapObject
{
private:
	explicit CVault_Torch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVault_Torch(const CVault_Torch& rhs);
	virtual ~CVault_Torch() = default;

public:
	_uint Get_TorchIndex() const { return m_iTorchIndex; }
	void  Set_TorchOn(_bool tf) { m_isTorchOn = tf; }
	void Switch_OnOff(_bool isValue);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;

private:
	CParticleSystem* m_pEffect = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };

	CLight* m_pLight = { nullptr };
	CLight* m_pLightbyTrigger = { nullptr }; // Ʈ���ſ� ���� ��

private:
	// Torch �ѹ���. �������� 1, 2, 3 �̷��� ��������.
	// �� �������� �� 12���� Torch�� �ִ�.
	_uint	m_iTorchIndex = { 0 }; 
	_bool	m_isSwitch = { false };

	_bool	m_isTorchOn = { false };  // Ʈ���ſ� ���� ȶ�� On
	_bool	m_isEffectOn = { false }; // Ʈ���ſ� ���� ȶ�� ����Ʈ On

	_float	m_fLightTime = { 0.f }; // Ʈ���ſ� ���� ȶ���� ���� �� ���� ������ Ÿ�̹�

private:
	void	Torch_On_By_Trigger(_float fTimeDelta); // Ʈ���ſ� ���� Torch ��ȭ

public:
	static CVault_Torch* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END