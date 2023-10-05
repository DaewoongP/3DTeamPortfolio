#pragma once
#include "Camera.h"
#include "Client_Defines.h"


BEGIN(Engine)

class CTransform;

END

BEGIN(Client)

class CAnimation_Camera_Model;

END

BEGIN(Client)

class CPlayer_Camera :
	public CCamera
{
public:
	enum CAMERAACTIONTYPE
	{
		CAMERAACTION_NONE,
		CAMERAACTION_LIGHTNING,
		CAMERAACTION_END
	};

public:   
	typedef struct tagPlayerCameraDesc
	{
		CAMERADESC CameraDesc{};
		CTransform* pPlayerTransform = { nullptr };
		LEVELID eLevelID = { LEVEL_END };
	}PLAYERCAMERADESC;

private:
	explicit CPlayer_Camera() = default;
	explicit CPlayer_Camera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CPlayer_Camera() = default;

	//for. Get, Set
public:
	_float3 Get_CamLookXZ();
	_float3 Get_CamRightXZ();
public:
	void Change_Animation(const wstring& _AnimattionIndex);

public:
	HRESULT Initialize(void* pArg);
	virtual void Tick(const _float& fTimeDelta);

#ifdef _DEBUG
	void Tick_ImGui();
#endif // _DEBUG


private:
	//CAnimation_Camera_Model* m_pAnimation_Camera_Model = { nullptr };

	CTransform* m_pTransform = { nullptr };
	CTransform* m_pPlayerTransform = { nullptr };
	LEVELID m_eLevelID = { LEVEL_END };

	_float m_fFollowLimit = { 0.0f };

	//ī�޶� ��ġ ���� ��
	_float3 m_vEyeStandard{};
	_float m_fEyeMaxDistance = { 0.0f };
	_float m_fEyeDistance = { 0.0f };
	_float m_fEyeMinDistance = { 0.0f };

	_float m_fEyeIntersectDistance = { 0.0f };

	//Ÿ�� ��ġ ���� ��
	_float3 m_vAtStandard{};
	_float m_fAtMaxDistance = { 0.0f };
	_float m_fAtDistance = { 0.0f };
	_float m_fAtMinDistance = { 0.0f };

	_float m_fAtIntersectDistance = { 0.0f };

	_float m_fTimeSpeed = { 0.0f };

	_float4x4 m_FollowTargetMatrix{};

	_float m_fCameraHeight{ 0.0f };

	_float m_fLerpTimeAcc = { 0.0f };

	_float m_fLerpSpeed = { 5.0f };

private:
	void	Mouse_Input(_float _fTimeDelta);
	//��� ����
	void	Update_FollowMatrix();

	//��ġ�� ����
	void	Follow_Transform();
	//Eye At �浹 �Ÿ��� ���� �ٲ��
	void	Eye_At_Distance();
	//Eye At ���ϱ�
	void	Update_Eye_At();

	_bool	IsValid_CameraPos(_float3 vEye, _float3 vUp);

	//HRESULT Ready_Animation_Camera();

public:
	static CPlayer_Camera* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, void* pArg);
	virtual void Free() override;



};

END