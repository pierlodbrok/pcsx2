/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2010  PCSX2 Dev Team
 *
 *  PCSX2 is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  PCSX2 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with PCSX2.
 *  If not, see <http://www.gnu.org/licenses/>.
 */


#include "PrecompiledHeader.h"

#include "Common.h"
#include "R5900OpcodeTables.h"
#include "iR5900.h"

using namespace x86Emitter;

namespace R5900 {
namespace Dynarec {
namespace OpcodeImpl
{

/*********************************************************
* Shift arithmetic with constant shift                   *
* Format:  OP rd, rt, sa                                 *
*********************************************************/
#ifndef SHIFT_RECOMPILE

namespace Interp = R5900::Interpreter::OpcodeImpl;

REC_FUNC_DEL(SLL, _Rd_);
REC_FUNC_DEL(SRL, _Rd_);
REC_FUNC_DEL(SRA, _Rd_);
REC_FUNC_DEL(DSLL, _Rd_);
REC_FUNC_DEL(DSRL, _Rd_);
REC_FUNC_DEL(DSRA, _Rd_);
REC_FUNC_DEL(DSLL32, _Rd_);
REC_FUNC_DEL(DSRL32, _Rd_);
REC_FUNC_DEL(DSRA32, _Rd_);

REC_FUNC_DEL(SLLV, _Rd_);
REC_FUNC_DEL(SRLV, _Rd_);
REC_FUNC_DEL(SRAV, _Rd_);
REC_FUNC_DEL(DSLLV, _Rd_);
REC_FUNC_DEL(DSRLV, _Rd_);
REC_FUNC_DEL(DSRAV, _Rd_);

#else

//// SLL
void recSLL_const()
{
	g_cpuConstRegs[_Rd_].SD[0] = (s32)(g_cpuConstRegs[_Rt_].UL[0] << _Sa_);
}

void recSLLs_(int info, int sa)
{
	pxAssert( !(info & PROCESS_EE_XMM) );

	xMOV(eax, ptr[&cpuRegs.GPR.r[ _Rt_ ].UL[ 0 ] ]);
	if ( sa != 0 )
	{
		xSHL(eax, sa );
	}

	xCDQ( );
	xMOV(ptr[&cpuRegs.GPR.r[ _Rd_ ].UL[ 0 ]], eax);
	xMOV(ptr[&cpuRegs.GPR.r[ _Rd_ ].UL[ 1 ]], edx);
}

void recSLL_(int info)
{
	recSLLs_(info, _Sa_);
}

EERECOMPILE_CODEX(eeRecompileCode2, SLL);

//// SRL
void recSRL_const()
{
	g_cpuConstRegs[_Rd_].SD[0] = (s32)(g_cpuConstRegs[_Rt_].UL[0] >> _Sa_);
}

void recSRLs_(int info, int sa)
{
	pxAssert( !(info & PROCESS_EE_XMM) );

	xMOV(eax, ptr[&cpuRegs.GPR.r[ _Rt_ ].UL[ 0 ] ]);
	if ( sa != 0 ) xSHR(eax, sa);

	xCDQ( );
	xMOV(ptr[&cpuRegs.GPR.r[ _Rd_ ].UL[ 0 ]], eax);
	xMOV(ptr[&cpuRegs.GPR.r[ _Rd_ ].UL[ 1 ]], edx);
}

void recSRL_(int info)
{
	recSRLs_(info, _Sa_);
}

EERECOMPILE_CODEX(eeRecompileCode2, SRL);

//// SRA
void recSRA_const()
{
	g_cpuConstRegs[_Rd_].SD[0] = (s32)(g_cpuConstRegs[_Rt_].SL[0] >> _Sa_);
}

void recSRAs_(int info, int sa)
{
	pxAssert( !(info & PROCESS_EE_XMM) );

	xMOV(eax, ptr[&cpuRegs.GPR.r[ _Rt_ ].UL[ 0 ] ]);
	if ( sa != 0 ) xSAR(eax, sa);

	xCDQ();
	xMOV(ptr[&cpuRegs.GPR.r[ _Rd_ ].UL[ 0 ]], eax);
	xMOV(ptr[&cpuRegs.GPR.r[ _Rd_ ].UL[ 1 ]], edx);
}

void recSRA_(int info)
{
	recSRAs_(info, _Sa_);
}

EERECOMPILE_CODEX(eeRecompileCode2, SRA);

////////////////////////////////////////////////////
void recDSLL_const()
{
	g_cpuConstRegs[_Rd_].UD[0] = (u64)(g_cpuConstRegs[_Rt_].UD[0] << _Sa_);
}

void recDSLLs_(int info, int sa)
{
	int rtreg, rdreg;
	pxAssert( !(info & PROCESS_EE_XMM) );

	_addNeededMMXreg(MMX_GPR+_Rt_);
	_addNeededMMXreg(MMX_GPR+_Rd_);
	rtreg = _allocMMXreg(-1, MMX_GPR+_Rt_, MODE_READ);
	rdreg = _allocMMXreg(-1, MMX_GPR+_Rd_, MODE_WRITE);
	SetMMXstate();

	if( rtreg != rdreg ) xMOVQ(xRegisterMMX(rdreg), xRegisterMMX(rtreg));
	xPSLL.Q(xRegisterMMX(rdreg), sa);
}

void recDSLL_(int info)
{
	recDSLLs_(info, _Sa_);
}

EERECOMPILE_CODEX(eeRecompileCode2, DSLL);

////////////////////////////////////////////////////
void recDSRL_const()
{
	g_cpuConstRegs[_Rd_].UD[0] = (u64)(g_cpuConstRegs[_Rt_].UD[0] >> _Sa_);
}

void recDSRLs_(int info, int sa)
{
	int rtreg, rdreg;
	pxAssert( !(info & PROCESS_EE_XMM) );

	_addNeededMMXreg(MMX_GPR+_Rt_);
	_addNeededMMXreg(MMX_GPR+_Rd_);
	rtreg = _allocMMXreg(-1, MMX_GPR+_Rt_, MODE_READ);
	rdreg = _allocMMXreg(-1, MMX_GPR+_Rd_, MODE_WRITE);
	SetMMXstate();

	if( rtreg != rdreg ) xMOVQ(xRegisterMMX(rdreg), xRegisterMMX(rtreg));
	xPSRL.Q(xRegisterMMX(rdreg), sa);
}

void recDSRL_(int info)
{
	recDSRLs_(info, _Sa_);
}

EERECOMPILE_CODEX(eeRecompileCode2, DSRL);

//// DSRA
void recDSRA_const()
{
	g_cpuConstRegs[_Rd_].SD[0] = (u64)(g_cpuConstRegs[_Rt_].SD[0] >> _Sa_);
}

void recDSRAs_(int info, int sa)
{
	int rtreg, rdreg, t0reg;
	pxAssert( !(info & PROCESS_EE_XMM) );

	_addNeededMMXreg(MMX_GPR+_Rt_);
	_addNeededMMXreg(MMX_GPR+_Rd_);
	rtreg = _allocMMXreg(-1, MMX_GPR+_Rt_, MODE_READ);
	rdreg = _allocMMXreg(-1, MMX_GPR+_Rd_, MODE_WRITE);
	SetMMXstate();

	if( rtreg != rdreg ) xMOVQ(xRegisterMMX(rdreg), xRegisterMMX(rtreg));

    if ( sa != 0 ) {
		t0reg = _allocMMXreg(-1, MMX_TEMP, 0);
		xMOVQ(xRegisterMMX(t0reg), xRegisterMMX(rtreg));

		// it is a signed shift
		xPSRA.D(xRegisterMMX(t0reg), sa);
		xPSRL.Q(xRegisterMMX(rdreg), sa);

		xPUNPCK.HDQ(xRegisterMMX(t0reg), xRegisterMMX(t0reg)); // shift to lower
		// take lower dword of rdreg and lower dword of t0reg
		xPUNPCK.LDQ(xRegisterMMX(rdreg), xRegisterMMX(t0reg));

		_freeMMXreg(t0reg);
	}
}

void recDSRA_(int info)
{
	recDSRAs_(info, _Sa_);
}

EERECOMPILE_CODEX(eeRecompileCode2, DSRA);

///// DSLL32
void recDSLL32_const()
{
	g_cpuConstRegs[_Rd_].UD[0] = (u64)(g_cpuConstRegs[_Rt_].UD[0] << (_Sa_+32));
}

void recDSLL32s_(int info, int sa)
{
	pxAssert( !(info & PROCESS_EE_XMM) );

	xMOV(eax, ptr[&cpuRegs.GPR.r[ _Rt_ ].UL[ 0 ] ]);
	if ( sa != 0 )
	{
		xSHL(eax, sa );
	}
	xMOV(ptr32[&cpuRegs.GPR.r[ _Rd_ ].UL[ 0 ]], 0 );
	xMOV(ptr[&cpuRegs.GPR.r[ _Rd_ ].UL[ 1 ]], eax);

}

void recDSLL32_(int info)
{
	recDSLL32s_(info, _Sa_);
}

EERECOMPILE_CODEX(eeRecompileCode2, DSLL32);

//// DSRL32
void recDSRL32_const()
{
	g_cpuConstRegs[_Rd_].UD[0] = (u64)(g_cpuConstRegs[_Rt_].UD[0] >> (_Sa_+32));
}

void recDSRL32s_(int info, int sa)
{
	pxAssert( !(info & PROCESS_EE_XMM) );

	xMOV(eax, ptr[&cpuRegs.GPR.r[ _Rt_ ].UL[ 1 ] ]);
	if ( sa != 0 ) xSHR(eax, sa );

	xMOV(ptr[&cpuRegs.GPR.r[ _Rd_ ].UL[ 0 ]], eax);
	xMOV(ptr32[&cpuRegs.GPR.r[ _Rd_ ].UL[ 1 ]], 0 );
}

void recDSRL32_(int info)
{
	recDSRL32s_(info, _Sa_);
}

EERECOMPILE_CODEX(eeRecompileCode2, DSRL32);

//// DSRA32
void recDSRA32_const()
{
	g_cpuConstRegs[_Rd_].SD[0] = (u64)(g_cpuConstRegs[_Rt_].SD[0] >> (_Sa_+32));
}

void recDSRA32s_(int info, int sa)
{
	pxAssert( !(info & PROCESS_EE_XMM) );

	xMOV(eax, ptr[&cpuRegs.GPR.r[ _Rt_ ].UL[ 1 ] ]);
	xCDQ( );
	if ( sa != 0 ) xSAR(eax, sa );

	xMOV(ptr[&cpuRegs.GPR.r[ _Rd_ ].UL[ 0 ]], eax);
	xMOV(ptr[&cpuRegs.GPR.r[ _Rd_ ].UL[ 1 ]], edx);

}

void recDSRA32_(int info)
{
	recDSRA32s_(info, _Sa_);
}

EERECOMPILE_CODEX(eeRecompileCode2, DSRA32);

/*********************************************************
* Shift arithmetic with variant register shift           *
* Format:  OP rd, rt, rs                                 *
*********************************************************/

__aligned16 u32 s_sa[4] = {0x1f, 0, 0x3f, 0};

int recSetShiftV(int info, int* rsreg, int* rtreg, int* rdreg, int* rstemp, int shift64)
{
	pxAssert( !(info & PROCESS_EE_XMM) );

	_addNeededMMXreg(MMX_GPR+_Rt_);
	_addNeededMMXreg(MMX_GPR+_Rd_);
	*rtreg = _allocMMXreg(-1, MMX_GPR+_Rt_, MODE_READ);
	*rdreg = _allocMMXreg(-1, MMX_GPR+_Rd_, MODE_WRITE);
	SetMMXstate();

	*rstemp = _allocMMXreg(-1, MMX_TEMP, 0);
	xMOV(eax, ptr[&cpuRegs.GPR.r[_Rs_].UL[0]]);
	xAND(eax, shift64?0x3f:0x1f);
	xMOVDZX(xRegisterMMX(*rstemp), eax);
	*rsreg = *rstemp;

	if( *rtreg != *rdreg ) xMOVQ(xRegisterMMX(*rdreg), xRegisterMMX(*rtreg));
	return 1;
}

void recSetConstShiftV(int info, int* rsreg, int* rdreg, int* rstemp, int shift64)
{
	_addNeededMMXreg(MMX_GPR+_Rd_);
	*rdreg = _allocMMXreg(-1, MMX_GPR+_Rd_, MODE_WRITE);
	SetMMXstate();

	*rstemp = _allocMMXreg(-1, MMX_TEMP, 0);
	xMOV(eax, ptr[&cpuRegs.GPR.r[_Rs_].UL[0]]);
	xAND(eax, shift64?0x3f:0x1f);
	xMOVDZX(xRegisterMMX(*rstemp), eax);
	*rsreg = *rstemp;

	_flushConstReg(_Rt_);
}

//// SLLV
void recSLLV_const()
{
	g_cpuConstRegs[_Rd_].SD[0] = (s32)(g_cpuConstRegs[_Rt_].UL[0] << (g_cpuConstRegs[_Rs_].UL[0] &0x1f));
}

void recSLLV_consts(int info)
{
	recSLLs_(info, g_cpuConstRegs[_Rs_].UL[0]&0x1f);
}

void recSLLV_constt(int info)
{
	xMOV(ecx, ptr[&cpuRegs.GPR.r[ _Rs_ ].UL[ 0 ] ]);

	xMOV(eax, g_cpuConstRegs[_Rt_].UL[0] );
	xAND(ecx, 0x1f );
	xSHL(eax, cl);

	eeSignExtendTo(_Rd_);
}

void recSLLV_(int info)
{
	xMOV(eax, ptr[&cpuRegs.GPR.r[ _Rt_ ].UL[ 0 ] ]);
	if ( _Rs_ != 0 )
	{
		xMOV(ecx, ptr[&cpuRegs.GPR.r[ _Rs_ ].UL[ 0 ] ]);
		xAND(ecx, 0x1f );
		xSHL(eax, cl);
	}
	xCDQ();
	xMOV(ptr[&cpuRegs.GPR.r[ _Rd_ ].UL[ 0 ]], eax);
	xMOV(ptr[&cpuRegs.GPR.r[ _Rd_ ].UL[ 1 ]], edx);
}

EERECOMPILE_CODE0(SLLV, XMMINFO_READS|XMMINFO_READT|XMMINFO_WRITED);

//// SRLV
void recSRLV_const()
{
	g_cpuConstRegs[_Rd_].SD[0] = (s32)(g_cpuConstRegs[_Rt_].UL[0] >> (g_cpuConstRegs[_Rs_].UL[0] &0x1f));
}

void recSRLV_consts(int info)
{
	recSRLs_(info, g_cpuConstRegs[_Rs_].UL[0]&0x1f);
}

void recSRLV_constt(int info)
{
	xMOV(ecx, ptr[&cpuRegs.GPR.r[ _Rs_ ].UL[ 0 ] ]);

	xMOV(eax, g_cpuConstRegs[_Rt_].UL[0] );
	xAND(ecx, 0x1f );
	xSHR(eax, cl);

	eeSignExtendTo(_Rd_);
}

void recSRLV_(int info)
{
	xMOV(eax, ptr[&cpuRegs.GPR.r[ _Rt_ ].UL[ 0 ] ]);
	if ( _Rs_ != 0 )
	{
		xMOV(ecx, ptr[&cpuRegs.GPR.r[ _Rs_ ].UL[ 0 ] ]);
		xAND(ecx, 0x1f );
		xSHR(eax, cl);
	}
	xCDQ( );
	xMOV(ptr[&cpuRegs.GPR.r[ _Rd_ ].UL[ 0 ]], eax);
	xMOV(ptr[&cpuRegs.GPR.r[ _Rd_ ].UL[ 1 ]], edx);
}

EERECOMPILE_CODE0(SRLV, XMMINFO_READS|XMMINFO_READT|XMMINFO_WRITED);

//// SRAV
void recSRAV_const()
{
	g_cpuConstRegs[_Rd_].SD[0] = (s32)(g_cpuConstRegs[_Rt_].SL[0] >> (g_cpuConstRegs[_Rs_].UL[0] &0x1f));
}

void recSRAV_consts(int info)
{
	recSRAs_(info, g_cpuConstRegs[_Rs_].UL[0]&0x1f);
}

void recSRAV_constt(int info)
{
	xMOV(ecx, ptr[&cpuRegs.GPR.r[ _Rs_ ].UL[ 0 ] ]);

	xMOV(eax, g_cpuConstRegs[_Rt_].UL[0] );
	xAND(ecx, 0x1f );
	xSAR(eax, cl);

	eeSignExtendTo(_Rd_);
}

void recSRAV_(int info)
{
	xMOV(eax, ptr[&cpuRegs.GPR.r[ _Rt_ ].UL[ 0 ] ]);
	if ( _Rs_ != 0 )
	{
		xMOV(ecx, ptr[&cpuRegs.GPR.r[ _Rs_ ].UL[ 0 ] ]);
		xAND(ecx, 0x1f );
		xSAR(eax, cl);
	}
	xCDQ( );
	xMOV(ptr[&cpuRegs.GPR.r[ _Rd_ ].UL[ 0 ]], eax);
	xMOV(ptr[&cpuRegs.GPR.r[ _Rd_ ].UL[ 1 ]], edx);
}

EERECOMPILE_CODE0(SRAV, XMMINFO_READS|XMMINFO_READT|XMMINFO_WRITED);

//// DSLLV
void recDSLLV_const()
{
	g_cpuConstRegs[_Rd_].UD[0] = (u64)(g_cpuConstRegs[_Rt_].UD[0] << (g_cpuConstRegs[_Rs_].UL[0] &0x3f));
}

void recDSLLV_consts(int info)
{
	int sa = g_cpuConstRegs[_Rs_].UL[0]&0x3f;
	if( sa < 32 ) recDSLLs_(info, sa);
	else recDSLL32s_(info, sa-32);
}

void recDSLLV_constt(int info)
{
	int rsreg, rdreg, rstemp = -1;
	recSetConstShiftV(info, &rsreg, &rdreg, &rstemp, 1);

	xMOVQ(xRegisterMMX(rdreg), ptr[&cpuRegs.GPR.r[_Rt_]]);
	xPSLL.Q(xRegisterMMX(rdreg), xRegisterMMX(rsreg));
	if( rstemp != -1 ) _freeMMXreg(rstemp);
}

void recDSLLV_(int info)
{
	int rsreg, rtreg, rdreg, rstemp = -1;
	recSetShiftV(info, &rsreg, &rtreg, &rdreg, &rstemp, 1);

	xPSLL.Q(xRegisterMMX(rdreg), xRegisterMMX(rsreg));
	if( rstemp != -1 ) _freeMMXreg(rstemp);
}

EERECOMPILE_CODE0(DSLLV, XMMINFO_READS|XMMINFO_READT|XMMINFO_WRITED);

//// DSRLV
void recDSRLV_const()
{
	g_cpuConstRegs[_Rd_].UD[0] = (u64)(g_cpuConstRegs[_Rt_].UD[0] >> (g_cpuConstRegs[_Rs_].UL[0] &0x3f));
}

void recDSRLV_consts(int info)
{
	int sa = g_cpuConstRegs[_Rs_].UL[0]&0x3f;
	if( sa < 32 ) recDSRLs_(info, sa);
	else recDSRL32s_(info, sa-32);
}

void recDSRLV_constt(int info)
{
	int rsreg, rdreg, rstemp = -1;
	recSetConstShiftV(info, &rsreg, &rdreg, &rstemp, 1);

	xMOVQ(xRegisterMMX(rdreg), ptr[&cpuRegs.GPR.r[_Rt_]]);
	xPSRL.Q(xRegisterMMX(rdreg), xRegisterMMX(rsreg));
	if( rstemp != -1 ) _freeMMXreg(rstemp);
}

void recDSRLV_(int info)
{
	int rsreg, rtreg, rdreg, rstemp = -1;
	recSetShiftV(info, &rsreg, &rtreg, &rdreg, &rstemp, 1);

	xPSRL.Q(xRegisterMMX(rdreg), xRegisterMMX(rsreg));
	if( rstemp != -1 ) _freeMMXreg(rstemp);
}

EERECOMPILE_CODE0(DSRLV, XMMINFO_READS|XMMINFO_READT|XMMINFO_WRITED);

//// DSRAV
void recDSRAV_const()
{
	g_cpuConstRegs[_Rd_].SD[0] = (s64)(g_cpuConstRegs[_Rt_].SD[0] >> (g_cpuConstRegs[_Rs_].UL[0] &0x3f));
}

void recDSRAV_consts(int info)
{
	int sa = g_cpuConstRegs[_Rs_].UL[0]&0x3f;
	if( sa < 32 ) recDSRAs_(info, sa);
	else recDSRA32s_(info, sa-32);
}

void recDSRAV_constt(int info)
{
	int rsreg, rdreg, rstemp = -1, t0reg, t1reg;
	t0reg = _allocMMXreg(-1, MMX_TEMP, 0);
	t1reg = _allocMMXreg(-1, MMX_TEMP, 0);

	recSetConstShiftV(info, &rsreg, &rdreg, &rstemp, 1);

	xMOVQ(xRegisterMMX(rdreg), ptr[&cpuRegs.GPR.r[_Rt_]]);
	xPXOR(xRegisterMMX(t0reg), xRegisterMMX(t0reg));

	// calc high bit
	xMOVQ(xRegisterMMX(t1reg), xRegisterMMX(rdreg));
	xPCMP.GTD(xRegisterMMX(t0reg), xRegisterMMX(rdreg));
	xPUNPCK.HDQ(xRegisterMMX(t0reg), xRegisterMMX(t0reg)); // shift to lower

	// shift highest bit, 64 - eax
	xMOV(eax, 64);
	xMOVDZX(xRegisterMMX(t1reg), eax);
	xPSUB.D(xRegisterMMX(t1reg), xRegisterMMX(rsreg));

	// right logical shift
	xPSRL.Q(xRegisterMMX(rdreg), xRegisterMMX(rsreg));
	xPSLL.Q(xRegisterMMX(t0reg), xRegisterMMX(t1reg)); // highest bits

	xPOR(xRegisterMMX(rdreg), xRegisterMMX(t0reg));

	_freeMMXreg(t0reg);
	_freeMMXreg(t1reg);
	if( rstemp != -1 ) _freeMMXreg(rstemp);
}

void recDSRAV_(int info)
{
	int rsreg, rtreg, rdreg, rstemp = -1, t0reg, t1reg;
	t0reg = _allocMMXreg(-1, MMX_TEMP, 0);
	t1reg = _allocMMXreg(-1, MMX_TEMP, 0);
	recSetShiftV(info, &rsreg, &rtreg, &rdreg, &rstemp, 1);

	xPXOR(xRegisterMMX(t0reg), xRegisterMMX(t0reg));

	// calc high bit
	xMOVQ(xRegisterMMX(t1reg), xRegisterMMX(rdreg));
	xPCMP.GTD(xRegisterMMX(t0reg), xRegisterMMX(rdreg));
	xPUNPCK.HDQ(xRegisterMMX(t0reg), xRegisterMMX(t0reg)); // shift to lower

	// shift highest bit, 64 - eax
	xMOV(eax, 64);
	xMOVDZX(xRegisterMMX(t1reg), eax);
	xPSUB.D(xRegisterMMX(t1reg), xRegisterMMX(rsreg));

	// right logical shift
	xPSRL.Q(xRegisterMMX(rdreg), xRegisterMMX(rsreg));
	xPSLL.Q(xRegisterMMX(t0reg), xRegisterMMX(t1reg)); // highest bits

	xPOR(xRegisterMMX(rdreg), xRegisterMMX(t0reg));

	_freeMMXreg(t0reg);
	_freeMMXreg(t1reg);
	if( rstemp != -1 ) _freeMMXreg(rstemp);
}

EERECOMPILE_CODE0(DSRAV, XMMINFO_READS|XMMINFO_READT|XMMINFO_WRITED);

#endif

} } }
