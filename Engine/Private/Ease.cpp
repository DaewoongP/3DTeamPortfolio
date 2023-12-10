#include "Ease.h"

namespace Engine {

    const char* CEase::pEases[EASE_END] = {
		"InQuad", "OutQuad", "InOutQuad"
		, "InSine", "OutSine", "InOutSine"
		, "InQuint", "OutQuint", "InOutQuint"
		, "InQuart", "OutQuart", "InOutQuart"
		, "NoneLinear", "InLinear", "OutLinear", "InOut_Linear"
		, "InExpo", "OutExpo", "InOutExpo"
		, "InElastic", "OutElastic", "InOutElastic"
		, "InCubic", "OutCubic", "InOutCubic"
		, "InCirc", "OutCirc", "InOutCirc"
		, "InBounce", "OutBounce", "InOutBounce"
		, "InBack", "OutBack", "InOutBack"
    };

	const _tchar* CEase::pTEases[EASE_END] = {
	TEXT("InQuad"), TEXT("OutQuad"), TEXT("InOutQuad")
	, TEXT("InSine"),	TEXT("OutSine"),	TEXT("InOutSine")
	, TEXT("InQuint"),	TEXT("OutQuint"),	TEXT("InOutQuint")
	, TEXT("InQuart"),	TEXT("OutQuart"),	TEXT("InOutQuart")
	, TEXT("NoneLinear"), TEXT("InLinear"),	TEXT("OutLinear"), TEXT("InOut_Linear")
	, TEXT("InExpo"),	TEXT("OutExpo"),	TEXT("InOutExpo")
	, TEXT("InElastic"),TEXT("OutElastic"),	TEXT("InOutElastic")
	, TEXT("InCubic"),	TEXT("OutCubic"),	TEXT("InOutCubic")
	, TEXT("InCirc"),	TEXT("OutCirc"),	TEXT("InOutCirc")
	, TEXT("InBounce"),	TEXT("OutBounce"),	TEXT("InOutBounce")
	, TEXT("InBack"),	TEXT("OutBack"),	TEXT("InOutBack")
	};
}