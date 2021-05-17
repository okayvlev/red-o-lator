//
// Created by Diana Kudaiberdieva
//

#include <stdexcept>
#include "alu.h"

void set_reladdr_and_run(const Instruction& instruction,
                         Wavefront* wf,
                         WfStateSOPP& state,
                         void (*exec)(WfStateSOPP&));

static inline void run_s_branch(WfStateSOPP& state) {
    state.PC->set_value(state.RELADDR);
}

static inline void run_s_cbranch_cdbgsys(WfStateSOPP& state) {
    if (state.STATUS->cond_dbg_sys()) {
        state.PC->set_value(state.RELADDR);
    }
}

static inline void run_s_cbranch_cdbgsys_and_user(WfStateSOPP& state) {
    if (state.STATUS->cond_dbg_sys() && state.STATUS->cond_dbg_user()) {
        state.PC->set_value(state.RELADDR);
    }
}

static inline void run_s_cbranch_cdbgsys_or_user(WfStateSOPP& state) {
    if (state.STATUS->cond_dbg_sys() || state.STATUS->cond_dbg_user()) {
        state.PC->set_value(state.RELADDR);
    }
}

static inline void run_s_cbranch_cdbguser(WfStateSOPP& state) {
    if (state.STATUS->cond_dbg_user()) {
        state.PC->set_value(state.RELADDR);
    }
}

static inline void run_s_cbranch_execnz(WfStateSOPP& state) {
    if (state.EXEC != 0) state.PC->set_value(state.RELADDR);
}

static inline void run_s_cbranch_execz(WfStateSOPP& state) {
    if (state.EXEC == 0) state.PC->set_value(state.RELADDR);
}

static inline void run_s_cbranch_scc0(WfStateSOPP& state) {
    if (state.SCC == 0) state.PC->set_value(state.RELADDR);
}

static inline void run_s_cbranch_scc1(WfStateSOPP& state) {
    if (state.SCC) state.PC->set_value(state.RELADDR);
}
static inline void run_s_cbranch_vccnz(WfStateSOPP& state) {
    if (state.VCC != 0) state.PC->set_value(state.RELADDR);
}
static inline void run_s_cbranch_vccz(WfStateSOPP& state) {
    if (state.VCC == 0) state.PC->set_value(state.RELADDR);
}

static inline void run_s_nop(WfStateSOPP& state) {
    //do nothing
}

static inline void run_s_set_gpr_idx_mode(WfStateSOPP& state) {
    state.M0 = (state.M0 & 0xffff0fff) | ((state.SIMM16 & 15) << 12);
}

static inline void run_s_set_gpr_idx_off(WfStateSOPP& state) {
    state.MODE->gpr_idx_en(0);
}

static inline void run_s_sethalt(WfStateSOPP& state) {
    state.STATUS->halt(state.SIMM16 & 1);
}

static inline void run_s_waitcnt(WfStateSOPP& state) {
    //do nothing
}

void run_sopp(const Instruction& instruction, Wavefront* wavefront) {
    auto instrKey = instruction.get_key();

    auto state = wavefront->get_common_sopp_state(instruction);

    switch (instrKey) {
        case S_BARRIER:
        case S_ENDPGM:
            assert("unexpected instruction in SOPP");
            break;

        case S_BRANCH:
            set_reladdr_and_run(instruction, wavefront, state, run_s_branch);
            break;
        case S_CBRANCH_CDBGSYS:
            set_reladdr_and_run(instruction, wavefront, state, run_s_cbranch_cdbgsys);
            break;
        case S_CBRANCH_CDBGSYS_AND_USER:
            set_reladdr_and_run(instruction, wavefront, state,
                                run_s_cbranch_cdbgsys_and_user);
            break;
        case S_CBRANCH_CDBGSYS_OR_USER:
            set_reladdr_and_run(instruction, wavefront, state,
                                run_s_cbranch_cdbgsys_or_user);
            break;
        case S_CBRANCH_CDBGUSER:
            set_reladdr_and_run(instruction, wavefront, state, run_s_cbranch_cdbguser);
            break;
        case S_CBRANCH_EXECNZ:
            set_reladdr_and_run(instruction, wavefront, state, run_s_cbranch_execnz);
            break;
        case S_CBRANCH_EXECZ:
            set_reladdr_and_run(instruction, wavefront, state, run_s_cbranch_execz);
            break;
        case S_CBRANCH_SCC0:
            set_reladdr_and_run(instruction, wavefront, state, run_s_cbranch_scc0);
            break;
        case S_CBRANCH_SCC1:
            set_reladdr_and_run(instruction, wavefront, state, run_s_cbranch_scc1);
            break;
        case S_CBRANCH_VCCNZ:
            set_reladdr_and_run(instruction, wavefront, state, run_s_cbranch_vccnz);
            break;
        case S_CBRANCH_VCCZ:
            set_reladdr_and_run(instruction, wavefront, state, run_s_cbranch_vccz);
            break;
        case S_NOP:
            run_s_nop(state);
            break;
        case S_SET_GPR_IDX_MODE:
            run_s_set_gpr_idx_mode(state);
            break;
        case S_SET_GPR_IDX_OFF:
            run_s_set_gpr_idx_off(state);
            break;
        case S_SETHALT:
            run_s_sethalt(state);
            break;
        case S_WAITCNT:
            run_s_waitcnt(state);
            break;
        default:
            assert(false && "Unknown instruction met!");
    }

    wavefront->update_with_common_sopp_state(instruction, state);
}

void set_reladdr_and_run(const Instruction& instruction,
                         Wavefront* wf,
                         WfStateSOPP& state,
                         void (*exec)(WfStateSOPP&)) {
    state.RELADDR = to_uin64_t(wf->read_operand(*instruction[0]));
    exec(state);
}
