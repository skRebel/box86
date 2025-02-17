#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>
#include <errno.h>

#include "debug.h"
#include "box86context.h"
#include "dynarec.h"
#include "emu/x86emu_private.h"
#include "emu/x86run_private.h"
#include "x86run.h"
#include "x86emu.h"
#include "box86stack.h"
#include "callback.h"
#include "emu/x86run_private.h"
#include "emu/x87emu_private.h"
#include "x86trace.h"
#include "dynarec_arm.h"
#include "dynarec_arm_private.h"
#include "arm_printer.h"

#include "dynarec_arm_functions.h"
#include "dynarec_arm_helper.h"


uintptr_t dynarecDD(dynarec_arm_t* dyn, uintptr_t addr, uintptr_t ip, int ninst, int* ok, int* need_epilog)
{
    uint8_t nextop = F8;
    int fixedaddress;
    int parity;
    uint8_t ed;
    int v1, v2;
    int i1, i2, i3;

    MAYUSE(v2);
    MAYUSE(v1);

    switch(nextop) {
        case 0xC0:
        case 0xC1:
        case 0xC2:
        case 0xC3:
        case 0xC4:
        case 0xC5:
        case 0xC6:
        case 0xC7:
            INST_NAME("FFREE STx");
            x87_purgecache(dyn, ninst, 0, x1, x2, x3);
            MOVW(x1, nextop&7);
            CALL(fpu_do_free, -1, 0);
            break;
        case 0xD0:
        case 0xD1:
        case 0xD2:
        case 0xD3:
        case 0xD4:
        case 0xD5:
        case 0xD6:
        case 0xD7:
            INST_NAME("FST ST0, STx");
            v1 = x87_get_st(dyn, ninst, x1, x2, 0, X87_COMBINE(0, nextop&7));
            v2 = x87_get_st(dyn, ninst, x1, x2, nextop&7, X87_COMBINE(0, nextop&7));
            if(ST_IS_F(0)) {
                VMOV_32(v2, v1);
            } else {
                VMOV_64(v2, v1);
            }
            break;
        case 0xD8:
            INST_NAME("FSTP ST0, ST0");
            x87_do_pop(dyn, ninst, x3);
            break;
        case 0xD9:
        case 0xDA:
        case 0xDB:
        case 0xDC:
        case 0xDD:
        case 0xDE:
        case 0xDF:
            INST_NAME("FSTP ST0, STx");
            // copy the cache value for st0 to stx
            i1 = x87_get_cache(dyn, ninst, 0, x1, x2, nextop&7, X87_COMBINE(0, nextop&7));
            i2 = x87_get_cache(dyn, ninst, 1, x1, x2, 0, X87_COMBINE(0, nextop&7));
            i3 = dyn->x87cache[i1];
            dyn->x87cache[i1] = dyn->x87cache[i2];
            dyn->x87cache[i2] = i3;
            // swap those too
            i1 = x87_get_neoncache(dyn, ninst, x1, x2, nextop&7);
            i2 = x87_get_neoncache(dyn, ninst, x1, x2, 0);
            i3 = dyn->n.neoncache[i1].v;
            dyn->n.neoncache[i1].v = dyn->n.neoncache[i2].v;
            dyn->n.neoncache[i2].v = i3;
            x87_do_pop(dyn, ninst, x3);
            break;

        case 0xE0:
        case 0xE1:
        case 0xE2:
        case 0xE3:
        case 0xE4:
        case 0xE5:
        case 0xE6:
        case 0xE7:
            INST_NAME("FUCOM ST0, STx");
            v1 = x87_get_st(dyn, ninst, x1, x2, 0, X87_COMBINE(0, nextop&7));
            v2 = x87_get_st(dyn, ninst, x1, x2, nextop&7, X87_COMBINE(0, nextop&7));
            if(ST_IS_F(0)) {
                VCMP_F32(v1, v2);
            } else {
                VCMP_F64(v1, v2);
            }
            FCOM(x1, x2);
            break;
        case 0xE8:
        case 0xE9:
        case 0xEA:
        case 0xEB:
        case 0xEC:
        case 0xED:
        case 0xEE:
        case 0xEF:
            INST_NAME("FUCOMP ST0, STx");
            v1 = x87_get_st(dyn, ninst, x1, x2, 0, X87_COMBINE(0, nextop&7));
            v2 = x87_get_st(dyn, ninst, x1, x2, nextop&7, X87_COMBINE(0, nextop&7));
            if(ST_IS_F(0)) {
                VCMP_F32(v1, v2);
            } else {
                VCMP_F64(v1, v2);
            }
            FCOM(x1, x2);
            x87_do_pop(dyn, ninst, x3);
            break;

        case 0xC8:
        case 0xC9:
        case 0xCA:
        case 0xCB:
        case 0xCC:
        case 0xCD:
        case 0xCE:
        case 0xCF:
        case 0xF0:
        case 0xF1:
        case 0xF2:
        case 0xF3:
        case 0xF4:
        case 0xF5:
        case 0xF6:
        case 0xF7:
        case 0xF8:
        case 0xF9:
        case 0xFA:
        case 0xFB:
        case 0xFC:
        case 0xFD:
        case 0xFE:
        case 0xFF:
            DEFAULT;
            break;

        default:
            switch((nextop>>3)&7) {
                case 0:
                    INST_NAME("FLD double");
                    v1 = x87_do_push(dyn, ninst, x1, NEON_CACHE_ST_D);
		            parity = getedparity(dyn, ninst, addr, nextop, 3);
		            if (parity) {
                        addr = geted(dyn, addr, ninst, nextop, &ed, x1, &fixedaddress, 1023, 3, 0);
                        VLDR_64(v1, ed, fixedaddress);
                    } else {
                        addr = geted(dyn, addr, ninst, nextop, &ed, x1, &fixedaddress, 4095-4, 0, 0);
                        LDR_IMM9(x2, ed, fixedaddress);
                        LDR_IMM9(x3, ed, fixedaddress + 4);
                        VMOVtoV_D(v1, x2, x3);
        		    }
                    break;
                case 1:
                    INST_NAME("FISTTP i64, ST0");
                    x87_forget(dyn, ninst, x1, x2, 0);
                    addr = geted(dyn, addr, ninst, nextop, &ed, x1, &fixedaddress, 0, 0, 0);
                    if(ed!=x1) {MOV_REG(x1, ed);}
                    CALL(arm_fistt64, -1, 0);
                    x87_do_pop(dyn, ninst, x3);
                    break;
                case 2:
                    INST_NAME("FST double");
                    v1 = x87_get_st(dyn, ninst, x1, x2, 0, NEON_CACHE_ST_D);
                    parity = getedparity(dyn, ninst, addr, nextop, 3);
                    if(parity) {
                        addr = geted(dyn, addr, ninst, nextop, &ed, x1, &fixedaddress, 1023, 3, 0);
                        VSTR_64(v1, ed, fixedaddress);
                    } else {
                        addr = geted(dyn, addr, ninst, nextop, &ed, x1, &fixedaddress, 4095-4, 0, 0);
                        VMOVfrV_D(x2, x3, v1);
                        STR_IMM9(x2, ed, fixedaddress);
                        STR_IMM9(x3, ed, fixedaddress+4);
                    }
                    break;
                case 3:
                    INST_NAME("FSTP double");
                    v1 = x87_get_st(dyn, ninst, x1, x2, 0, NEON_CACHE_ST_D);
                    parity = getedparity(dyn, ninst, addr, nextop, 3);
                    if(parity) {
                        addr = geted(dyn, addr, ninst, nextop, &ed, x1, &fixedaddress, 1023, 3, 0);
                        VSTR_64(v1, ed, fixedaddress);
                    } else {
                        addr = geted(dyn, addr, ninst, nextop, &ed, x1, &fixedaddress, 4095-4, 0, 0);
                        VMOVfrV_D(x2, x3, v1);
                        STR_IMM9(x2, ed, fixedaddress);
                        STR_IMM9(x3, ed, fixedaddress+4);
                    }
                    x87_do_pop(dyn, ninst, x3);
                    break;
                case 4: 
                    INST_NAME("FRSTOR m108byte");
                    fpu_purgecache(dyn, ninst, 0, x1, x2, x3);
                    addr = geted(dyn, addr, ninst, nextop, &ed, x1, &fixedaddress, 0, 0, 0);
                    if(ed!=x1) {MOV_REG(x1, ed);}
                    CALL(arm_frstor, -1, 0);
                    break;
                case 6: 
                    INST_NAME("FSAVE m108byte");
                    fpu_purgecache(dyn, ninst, 0, x1, x2, x3);
                    addr = geted(dyn, addr, ninst, nextop, &ed, x1, &fixedaddress, 0, 0, 0);
                    if(ed!=x1) {MOV_REG(x1, ed);}
                    CALL(arm_fsave, -1, 0);
                    break;
                case 7:
                    INST_NAME("FNSTSW m2byte");
                    addr = geted(dyn, addr, ninst, nextop, &ed, x14, &fixedaddress, 0, 0, 0);
                    LDR_IMM9(x2, xEmu, offsetof(x86emu_t, top));
                    LDRH_IMM8(x1, xEmu, offsetof(x86emu_t, sw));
                    if(dyn->x87stack) {
                        // update top
                        if(dyn->x87stack>0) {
                            SUB_IMM8(x2, x2, dyn->x87stack);
                        } else {
                            ADD_IMM8(x2, x2, -dyn->x87stack);
                        }
                        AND_IMM8(x2, x2, 7);
                    }
                    BFI(x1, x2, 11, 3); // inject top
                    STRH_IMM8(x1, ed, 0);   // store whole sw flags
                    break;
                default:
                    DEFAULT;
            }
    }
    return addr;
}

