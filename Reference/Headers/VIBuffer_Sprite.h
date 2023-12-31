#pragma once

/* =============================================== */
//	[CVIBuffer_Sprite]
//	- 스프라이트 버퍼 클래스
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Sprite final : public CVIBuffer
{
public:
	typedef struct tagSpriteDesc
	{
		_bool bRepeat;		// 반복재생 유무
		_float fFrameSpeed; // 초 단위로 넣으면 된다.
		_uint iNumWidth;	// 텍스처 내의 이미지 가로 개수
		_uint iNumHeight;	// 텍스처 내의 이미지 가로 개수
	}SPRITEDESC;

private:
	explicit CVIBuffer_Sprite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Sprite(const CVIBuffer_Sprite& rhs);
	virtual ~CVIBuffer_Sprite() = default;

public:
	_bool isPlay() const {
		return m_isPlay;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;

	/* Tick을 호출해야 스프라이트이미지가 변경됨. */
	void Tick(const _float & fTimeDelta);

private:
	_bool m_isRepeat = { true };
	_bool m_isPlay = { true };
	_float m_fFrameTick = { 0.f };
	_float m_fTimeAcc = { 0.f };
	_uint m_iNumWidth = { 0 };
	_uint m_iNumHeight = { 0 };
	_uint m_iCurKeyFrame = { 0 };

public:
	static CVIBuffer_Sprite* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CVIBuffer_Sprite* Clone(void* pArg) override;
	virtual void Free() override;
};

END