#pragma once
/* =============================================== */
// 
//	Á¤ : ¹Ú´ë¿õ
//	ºÎ :
//
/* =============================================== */

#include "Engine_Defines.h"

class CPXErrorCallBack : public PxErrorCallback
{
public:
	virtual ~CPXErrorCallBack() = default;

public:
	virtual void reportError(PxErrorCode::Enum eCode, const _char* pMessage, const _char* pFile,
		int iLine)
	{
		const _char* pErrorCode = NULL;

		switch (eCode)
		{
		case PxErrorCode::eNO_ERROR:
			pErrorCode = "no error";
			break;
		case PxErrorCode::eINVALID_PARAMETER:
			pErrorCode = "invalid parameter";
			break;
		case PxErrorCode::eINVALID_OPERATION:
			pErrorCode = "invalid operation";
			break;
		case PxErrorCode::eOUT_OF_MEMORY:
			pErrorCode = "out of memory";
			break;
		case PxErrorCode::eDEBUG_INFO:
			pErrorCode = "info";
			break;
		case PxErrorCode::eDEBUG_WARNING:
			pErrorCode = "warning";
			break;
		case PxErrorCode::ePERF_WARNING:
			pErrorCode = "performance warning";
			break;
		case PxErrorCode::eABORT:
			pErrorCode = "abort";
			break;
		case PxErrorCode::eINTERNAL_ERROR:
			pErrorCode = "internal error";
			break;
		case PxErrorCode::eMASK_ALL:
			pErrorCode = "unknown error";
			break;
		}

		PX_ASSERT(pErrorCode);
		if (pErrorCode)
		{
			_char buffer[1024];
			sprintf_s(buffer, "%s (%d) : %s : %s\n", pFile, iLine, pErrorCode, pMessage);

			cout << buffer << endl;

			// in debug builds halt execution for abort codes
			PX_ASSERT(e != PxErrorCode::eABORT);

			// in release builds we also want to halt execution 
			// and make sure that the error message is flushed  
			while (eCode == PxErrorCode::eABORT)
			{
				cout << buffer << endl;
			}
		}
	}
};