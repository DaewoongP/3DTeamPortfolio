#pragma once

/* =============================================== */
//	[CSequence_Levitate]
//	: ��ü�� ���ݰ��� �ൿ�� ����ϴ� Ŭ����
//	�� : �ּ�ȯ
//	�� : 
//
/* =============================================== */

#include "Sequence.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRandomChoose;
END

BEGIN(Client)

class CSequence_Levitate final : public CSequence
{
private:
	explicit CSequence_Levitate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSequence_Levitate(const CSequence_Levitate& rhs);
	virtual ~CSequence_Levitate() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;

public:
	// Random_Select Ŭ������ �߰��� Action�� ����ġ ����
	HRESULT Assemble_Random_Select_Behavior(const wstring& wstrActionTag, const _float& fWeight, const _float& fLoopTime);
	void Set_Option(const _float& fUpForce, const _float& fUpTime) {
		m_fUpForce = fUpForce;
		m_fUpTime = fUpTime;
	}

private:
	virtual HRESULT Assemble_Childs() override;

private:
	_float m_fUpForce = { 0.f };
	_float m_fUpTime = { 0.f };
	CRandomChoose* m_pRandom_Levitate_Loop = { nullptr };

private:
	virtual void Reset_Behavior(HRESULT result) override;

public:
	static CSequence_Levitate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSequence_Levitate* Clone(void* pArg) override;
	virtual void Free() override;
};

END