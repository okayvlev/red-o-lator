//
// Created by Diana Kudaiberdieva
//

#ifndef RED_O_LATOR_WF_STATE_H
#define RED_O_LATOR_WF_STATE_H

#include "reg/register.h"

struct WfStateSOP1 {
    uint64_t SDST;
    uint64_t SSRC0;
    uint64_t EXEC;
    uint32_t M0;
    uint64_t PC;
    bool SCC;

    WfStateSOP1(uint64_t SDST,
                uint64_t SSRC0,
                uint64_t EXEC,
                uint32_t M0,
                uint64_t PC,
                bool SCC)
        : SDST(SDST), SSRC0(SSRC0), EXEC(EXEC), M0(M0), PC(PC), SCC(SCC) {}
};

struct WfStateSOP2 {
    uint64_t SDST;
    uint64_t SSRC0;
    uint64_t SSRC1;
    bool SCC;

    WfStateSOP2(uint64_t SDST, uint64_t SSRC0, uint64_t SSRC1, bool SCC)
        : SDST(SDST), SSRC0(SSRC0), SCC(SCC) {}
};

struct WfStateSOPP {
    uint64_t RELADDR;
    uint64_t PC;
    uint64_t EXEC;
    ModeReg MODE;
    StatusReg STATUS;
    uint32_t M0;
    uint32_t SIMM16;
    uint64_t VCC;
    bool SCC;
};

struct WfStateSOPC {
    uint64_t SSRC0;
    uint64_t SSRC1;
    ModeReg MODE;
    uint32_t M0;
    uint32_t IMM8;
    bool SCC;
};

struct WfStateSOPK {
    uint64_t SDST;
    uint32_t IMM16;
    uint64_t RELADDR;
    uint64_t PC;
    bool SCC;
};

#endif  // RED_O_LATOR_WF_STATE_H
