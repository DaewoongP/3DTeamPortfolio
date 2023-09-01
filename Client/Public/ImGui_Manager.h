#pragma once
// 릴리즈 빌드에서 제외된 파일입니다.
// 코드 작성시 유의 해주세요
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CImGui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)
private:
	explicit CImGui_Manager() = default;
	virtual ~CImGui_Manager() = default;

public:
	HRESULT Initialize(LEVELID iLevelID);
	void Tick(_float fTimeDelta);

private:
	void Debug_Camera();

private:
	_bool			m_is3rdCam = { false };
	_bool			m_isFix3rdCam = { false };
	_float3			m_v3rdCamOffset;

public:
	virtual void Free() override;
};

END