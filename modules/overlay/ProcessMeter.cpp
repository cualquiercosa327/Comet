#include "ProcessMeter.hpp"

#include <libpokey/debug.h>
#include <libpokey/hooks.h>

namespace Overlay {

// Credit: Bean, Chadderz

PokeyInsertDefAsm(0x8000960C)
{
	lwz r12, 0(r21);
	mr r3, r21;
	lwz r12, 24(r12);
	mtctr r12;
	bctrl;
	lwz r12, 0(r3);
	lwz r12, 8(r12);
	mtctr r12;
	bctrl;
	lwz r12, 0(r21);
}
PokeyInsertDefAsm(0x8000962C)
{
	bctrl;
	lwz r12, 0(r21);
	mr r3, r21;
	lwz r12, 24(r12);
	mtctr r12;
	bctrl;
	lwz r12, 0(r3);
	lwz r12, 16(r12);
	mtctr r12;
	bctrl;
}
PokeyInsertDefAsm(0x8000968C)
{
	bctrl;
	lwz r12, 0(r21);
	mr r3, r21;
	lwz r12, 24(r12);
	mtctr r12;
	bctrl;
	lwz r12, 0(r3);
	lwz r12, 20(r12);
	mtctr r12;
	bctrl;
}
PokeyInsertDefAsm(0x800097A4)
{
	bctrl;
	lwz r12, 0(r21);
	mr r3, r21;
	lwz r12, 24(r12);
	mtctr r12;
	bctrl;
	li r12, 1;
	stb r12, 266(r3);
	lwz r12, 0(r3);
	lwz r12, 12(r12);
	mtctr r12;
	bctrl;
}
} // namespace Overlay