#pragma once
#include "Base.h"
#include "Tool_Defines.h"


BEGIN(Tool)

class CCamera_Point;

class CCutScene_Camera_Tool :
    public CBase
{
public:
	enum CUTSCENEPOINT
	{
		CUTSCENEPOINT_NONE,
		CUTSCENEPOINT_CREATE,
		CUTSCENEPOINT_DELETE,
		CUTSCENEPOINT_END
	};

private:
	explicit CCutScene_Camera_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CCutScene_Camera_Tool() = default;

public:
	HRESULT Initialize(void* pArg);
	void Tick(_float _fTimeDelta);
	HRESULT Render();




private:
	//포인트들을 담아 놓을 컨테이너 2개 필요(Eye, At)
	list<CCamera_Point*> m_EyeList;
	list<CCamera_Point*> m_AtList;
	//짝이 맞는 경우 생성될 컨테이너
	list<_bool> m_Lerplist;
	list<_float> m_DurationList;


	//현제 인덱스(수정할 객체의 인덱스)
	_uint m_iCurrentIndex = { 0 };

	//현제 선택된 오브젝트
	CCamera_Point* m_pCurrentPoint = { nullptr };
	
	//최대 인덱스
	_uint m_iMaxIndex = { 0 };

	//Eye,At 구분용
	_bool m_isEye;

	//IMGUI 라디오 버튼용
	_int m_iPointRadio = { 0 };

	//포인트 생성 거리
	_float m_fDistance = { 0 };


	//포인트를 생성 하기 위함
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

private:
	//담는 기능
	void Create_Point(const _float4& _vPosition);

	//빼는 기능
	void Delete_Point(const _uint& _iIndex);

	//저장 기능
	HRESULT Save_CutScene(const _tchar* _wszFileName);

	void Point_Create_Delete();


public:
	static CCutScene_Camera_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void * pArg = nullptr);
	virtual void Free() override;

};

#define LIST_INDEX_ACCESS(_list, _iIndex, _OutDataPointer)		\
		if (false == _list.empty())								\
		{														\
			auto Iter = _list.begin();							\
																\
			for(size_t Index = 0; Index < _iIndex; Index++)		\
			{													\
				++Iter;											\
			}													\
																\
			_OutDataPointer = &(*Iter);							\
		}														

END