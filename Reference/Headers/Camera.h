#pragma once
/* =============================================== */
// 
//	�� : ������
//	�� :
//
/* =============================================== */

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCamera : public CBase
{
public:
	enum TYPE { TYPE_MAIN, TYPE_CUTSCENE, TYPE_END };
	typedef struct tagCameraDesc
	{
		_float m_fFovY = { 0.f };
		_float m_fAspect = { 0.f };
		_float m_fNear = { 0.f };
		_float m_fFar = { 0.f };
	}CAMERADESC;

protected:
	explicit CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize(const CAMERADESC& CameraDesc);
	//Ʈ�������� ����Ͽ� ���������� ����� ����
	//���������� �� �ٲٰ���
	virtual void Tick(const _float& fTimeDelta);

protected:
	class CTransform*			m_pTransform = { nullptr };

protected:
	CAMERADESC					m_CameraDesc;

	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

public:
	static CCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CAMERADESC& CameraDesc);
	virtual void Free() override;
};

END