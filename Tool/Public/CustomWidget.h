#pragma once

#include "Base.h"
#include "Tool_Defines.h"

BEGIN(Tool)

class CCustomWidget : public CBase
{
protected:
    explicit CCustomWidget() = default;
    virtual ~CCustomWidget() = default;

public:
    virtual void Free(void) override;
};

END