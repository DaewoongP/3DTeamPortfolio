#pragma once

#include "Enemy.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CMesh;
class CModel;
class CShader;
class CRenderer;
END

BEGIN(Client)
class CMagicSlot;
END

BEGIN(Client)

class CEnergyBall final : public CGameObject
{
public:
	typedef struct tagEnergyBallInitDesc
	{
		_float3 vPosition = _float3();
		_float fActionProtegoTime = { 0.f }; // �����װ� ���� �ð�(�� �� �ڿ� ������ ����)
		function<_bool(const _float&)> DeathFunction = { nullptr };
	}ENERGYBALLINITDESC;

private:
	explicit CEnergyBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEnergyBall(const CEnergyBall& rhs);
	virtual ~CEnergyBall() = default;

public:
	_bool isEnable() const {
		return m_isEnable;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	
public:
	void Reset(const ENERGYBALLINITDESC& tagResetDesc);
	void Set_Protego_Collision(CTransform* pTransform, CEnemy::ATTACKTYPE eType) const;

private:
	_float m_fActionProtegoTime = { 0.f };
	_float m_fTimeAcc = { 0.f };
	_bool m_isFirst = { true };
	_bool m_isEnable = { false };
	function<_bool(const _float&)> m_DeathFunction;

private:
	CMagicSlot* m_pMagicSlot = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CShader* m_pShader = { nullptr };
	CModel* m_pModel = { nullptr };

private:
	HRESULT Make_Magics();
	HRESULT Add_Components();
	HRESULT Add_Components_Level(const _uint& iLevel);
	HRESULT SetUp_ShaderResources();

public:
	static CEnergyBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
