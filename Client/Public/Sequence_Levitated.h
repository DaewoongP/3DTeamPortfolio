#pragma once

/* =============================================== */
//	[CSequence_Levitated]
//	: ��ü�� ���ݰ��� �ൿ�� ����ϴ� Ŭ����
//	�� : �ּ�ȯ
//	�� : 
//
/* =============================================== */

#include "Sequence.h"
#include "Client_Defines.h"

BEGIN(Client)
class CRandom_Select;
END

BEGIN(Client)

class CSequence_Levitated final : public CSequence
{
private:
	explicit CSequence_Levitated(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSequence_Levitated(const CSequence_Levitated& rhs);
	virtual ~CSequence_Levitated() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(const _float& fTimeDelta);

public:
	// Random_Select Ŭ������ �߰��� Action�� ����ġ ����
	HRESULT Assemble_Random_Select_Behavior(const wstring& wstrActionTag, const _float& fWeight, const _float& fLoopTime);

private:
	virtual HRESULT Assemble_Childs() override;

private:
	CRandom_Select* m_pRandom_Levitated_Loop = { nullptr };

private:
	virtual void Reset_Behavior(HRESULT result) override;

public:
	static CSequence_Levitated* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSequence_Levitated* Clone(void* pArg) override;
	virtual void Free() override;
};

END