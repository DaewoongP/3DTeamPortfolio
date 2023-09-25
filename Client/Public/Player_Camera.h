#pragma once
#include "Camera.h"
#include "Client_Defines.h"


BEGIN(Engine)

class CTransform;

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

public:
	HRESULT Initialize(void* pArg);
	virtual void Tick(const _float& fTimeDelta);

#ifdef _DEBUG
	void Tick_ImGui();
#endif // _DEBUG


private:
	CTransform* m_pTransform = { nullptr };
	CTransform* m_pPlayerTransform = { nullptr };
	_float m_fFollowLimit = { 0.0f };

	//카메라 위치 기준 축
	_float3 m_vEyeStandard{};
	_float m_fEyeMaxDistance = { 0.0f };
	_float m_fEyeDistance = { 0.0f };
	_float m_fEyeMinDistance = { 0.0f };

	_float m_fEyeIntersectDistance = { 0.0f };

	//타겟 위치 기준 축
	_float3 m_vAtStandard{};
	_float m_fAtMaxDistance = { 0.0f };
	_float m_fAtDistance = { 0.0f };
	_float m_fAtMinDistance = { 0.0f };

	_float m_fAtIntersectDistance = { 0.0f };

	_float m_fTimeSpeed = { 0.0f };

	_float4x4 m_FollowTargetMatrix{};

	_float m_fCameraHeight{ 0.0f };

private:
	void	Mouse_Input(_float _fTimeDelta);
	//행렬 갱신
	void	Update_FollowMatrix();

	//위치만 따라감
	void	Follow_Transform();
	//Eye At 충돌 거리에 따라 바뀌게
	void	Eye_At_Distance();
	//Eye At 구하기
	void	Update_Eye_At();

public:
	static CPlayer_Camera* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, void* pArg);
	virtual void Free() override;



};

END