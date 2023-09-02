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
	//����Ʈ���� ��� ���� �����̳� 2�� �ʿ�(Eye, At)
	list<CCamera_Point*> m_EyeList;
	list<CCamera_Point*> m_AtList;
	//¦�� �´� ��� ������ �����̳�
	list<_bool> m_Lerplist;
	list<_float> m_DurationList;


	//���� �ε���(������ ��ü�� �ε���)
	_uint m_iCurrentIndex = { 0 };

	//���� ���õ� ������Ʈ
	CCamera_Point* m_pCurrentPoint = { nullptr };
	
	//��Ʈ�� �ϼ��� ������ : ������ ���� �� ������
	_uint m_iSetSize = { 0 };
	//�ִ� ������ : eye�� at�� �� ���� list�� ������
	_uint m_iMaxSize = { 0 };

	//Eye,At ���п�
	_bool m_isEye{ true };

	//IMGUI ���� ��ư��
	_int m_iPointRadio = { 0 };

	//����Ʈ ���� �Ÿ�
	_float m_fDistance = { 0 };


	//����Ʈ�� ���� �ϱ� ����
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

private:
	//Ŭ���� ����, ���, ����
	void Point_Select_Create_Delete();
	
	//���� ��� : ���� �� ����Ʈ�� �ִٸ� true ��ȯ, �ε����� ���� ��
	_bool Select_Point();

	//��� ���
	void Create_Point(const _float4& _vPosition);

	//���� ���
	void Delete_Point();

	//���� ���
	HRESULT Save_CutScene(const _tchar* _wszFilePath);

	//�ҷ����� ���
	HRESULT Load_CutScene(const _tchar* _wszFilePath);

	//���콺�� ��ġ ���� ���
	void Fix_Point();

public:
	static CCutScene_Camera_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void * pArg = nullptr);
	virtual void Free() override;

};

#define LIST_INDEX_ACCESS(_list, _iIndex, _OutData)				\
		if (false == _list.empty())								\
		{														\
			auto Iter = _list.begin();							\
																\
			for(size_t Index = 0; Index < _iIndex; Index++)		\
			{													\
				++Iter;											\
			}													\
																\
			_OutData = &(*Iter);								\
		}														

#define LIST_INDEX_ACCESS_POINTER(_list, _iIndex, _OutDataPointer)	\
		if (false == _list.empty())									\
		{															\
			auto Iter = _list.begin();								\
																	\
			for(size_t Index = 0; Index < _iIndex; Index++)			\
			{														\
				++Iter;												\
			}														\
																	\
			_OutDataPointer = (*Iter);								\
		}														


END