#include "isr_helper.h"


Status_t Run_Protected(CbInstance_t cb)
{   Status_t status;
    isr_state_t state = Isr_Disable();
    status = cb.callback(cb.context);
    Isr_Enable(state);
    return status;
}

Status_t Run_Privileged(isr_state_t isr_mask, CbInstance_t cb)
{   Status_t status;
    isr_mask = Isr_Mask(isr_mask);
    status = cb.callback(cb.context);
    Isr_Restore(isr_mask);
    return status;
}