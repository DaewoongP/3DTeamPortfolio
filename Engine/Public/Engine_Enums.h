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
}
