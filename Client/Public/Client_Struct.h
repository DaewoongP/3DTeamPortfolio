#pragma once
#include "Engine_Defines.h"

namespace Client
{
	typedef struct MagicBuffTickDesc {

	}MAIGBUFFTICKDESC;

	//������ ������ ����ü
	typedef struct Stick_TickDesc :MAIGBUFFTICKDESC {
		_float3* pPosition = { nullptr };
	}STICK_TICKDESC;

	//�ȴٸ� ������ ����ü ����ü
	typedef struct Stick_4_TickDesc :MAIGBUFFTICKDESC {
		_float3* pHand[2] = { nullptr };
		_float3* pFoot[2] = { nullptr };
	}STICK_4_TICKDESC;
}