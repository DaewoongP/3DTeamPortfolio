#pragma once

namespace Engine
{
	enum CELLFLAG
	{
		// 일반 이동가능한 셀
		CELL_MOVE = 1 << 0,
		// 특정 Y값보다 내려가면 사망처리되는 떨어지는셀.
		CELL_FALL = 1 << 1,
	};


	enum COLLISIONFLAG
	{
		COL_PLAYER = 1 << 0,
		COL_ENEMY = 1 << 1,
		// COL~~ = 1 << 2,
		// ... 계속 추가해주시면 됩니다!! 숫자만 계속 증가시켜주세요

		COL_TEST = 1 << 29, // 충돌 가능한 최대치입니다. 지우고 추가하셔도 됩니다.
							// 이 TEST를 지우고 추가하실때는 저한테 말씀해주세요

		COL_ALL = (1 << 30) - 1, // 다른 모든 객체와 충돌
	};

	enum ISCOLLISION
	{
		END_COL,
		USE_COL,
	};
}
