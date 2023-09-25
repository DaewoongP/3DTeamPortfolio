#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "MagicBall.h"
#include "Wingardium_Effect.h"


// �� ������� ��������� Ÿ���� �����ϰ� �ߵ��ϴ� �����Դϴ�.
// ���� Ÿ���� ������ �ȳ����ϴ�.
// �� ������� ��������� �÷��̾ �����Ű�ų�, �ٸ� ������ ���߽�Ű�� �������� ���׽��ϴ�.

BEGIN(Client)
class CWingardiumleviosa final : public CMagicBall
{
private:
	explicit CWingardiumleviosa(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWingardiumleviosa(const CWingardiumleviosa& rhs);
	virtual ~CWingardiumleviosa() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Reset() { return S_OK; }

private:
	CWingardium_Effect* m_pEffect = { nullptr };
	CTransform* m_pEffectTrans = { nullptr };

private:
	_float3				m_vTargetPosition = {};
	_bool				m_bDeadTrigger = { false };

	_float				m_fSettingTimer = { 8.f };

private:
	HRESULT Add_Components();
	virtual HRESULT Add_Effect();

public:
	static CWingardiumleviosa* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END