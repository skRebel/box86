#ifndef __DYNAREC_ARM_FUNCTIONS_H__
#define __DYNAREC_ARM_FUNCTIONS_H__

typedef struct x86emu_s x86emu_t;

void arm_fstp(x86emu_t* emu, void* p);

void arm_print_armreg(x86emu_t* emu, uintptr_t reg, uintptr_t n);

void arm_f2xm1(x86emu_t* emu);
void arm_fxtract(x86emu_t* emu);
void arm_fprem(x86emu_t* emu);
void arm_fyl2xp1(x86emu_t* emu);
void arm_frndint(x86emu_t* emu);
void arm_fscale(x86emu_t* emu);
void arm_fbld(x86emu_t* emu, uint8_t* ed);
void arm_fild64(x86emu_t* emu, int64_t* ed);
void arm_fbst(x86emu_t* emu, uint8_t* ed);
void arm_fistp64(x86emu_t* emu, int64_t* ed);
void arm_fistt64(x86emu_t* emu, int64_t* ed);
void arm_fld(x86emu_t* emu, uint8_t* ed);
void arm_fsave(x86emu_t* emu, uint8_t* ed);
void arm_frstor(x86emu_t* emu, uint8_t* ed);
void arm_fprem1(x86emu_t* emu);

void arm_ud(x86emu_t* emu);

#define FPUFIRST    8

// Get an FPU single scratch reg
int fpu_get_scratch_single(dynarec_arm_t* dyn);
// Get an FPU double scratch reg
int fpu_get_scratch_double(dynarec_arm_t* dyn);
// Get an FPU quad scratch reg
int fpu_get_scratch_quad(dynarec_arm_t* dyn);
// Reset scratch regs counter
void fpu_reset_scratch(dynarec_arm_t* dyn);
// Get an FPU double reg, with NEON_CACHE_TYPE
int fpu_get_reg_double(dynarec_arm_t* dyn, unsigned int t, unsigned int n);
// Free a FPU double reg
void fpu_free_reg_double(dynarec_arm_t* dyn, int reg);
// Get an FPU quad reg
int fpu_get_reg_quad(dynarec_arm_t* dyn, unsigned int t, unsigned int n);
// Free a FPU quad reg
void fpu_free_reg_quad(dynarec_arm_t* dyn, int reg);
// Reset fpu regs counter
void fpu_reset_reg(dynarec_arm_t* dyn);

// ---- Neon cache functions
// Get type for STx
int neoncache_get_st(dynarec_arm_t* dyn, int ninst, int a);
// Get if STx is FLOAT or DOUBLE
int neoncache_get_st_f(dynarec_arm_t* dyn, int ninst, int a, int st);
// Get actual type for STx
int neoncache_get_current_st(dynarec_arm_t* dyn, int ninst, int a);
// Back-propagate a change float->double
void neoncache_promote_double(dynarec_arm_t* dyn, int ninst, int a, int st);
// Combine and propagate if needed (pass 1 only)
int neoncache_combine_st(dynarec_arm_t* dyn, int ninst, int a, int b);  // with stack current dyn->n_stack*
// Get if ED will have the correct parity. Not emiting anything. Parity is 2 for DWORD or 3 for QWORD
int getedparity(dynarec_arm_t* dyn, int ninst, uintptr_t addr, uint8_t nextop, int parity);
// Do the GETED, but don't emit anything...
uintptr_t fakeed(dynarec_arm_t* dyn, uintptr_t addr, int ninst, uint8_t nextop);

// Is what pointed at addr a native call? And if yes, to what function?
int isNativeCall(dynarec_arm_t* dyn, uintptr_t addr, uintptr_t* calladdress, int* retn);

#endif //__DYNAREC_ARM_FUNCTIONS_H__