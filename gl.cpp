#include "gl.h"

volatile __attribute__((aligned(4))) uint32_t* GL::V3D = reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(ARM_IO_BASE + 0xc00000));

GL::GL(void)
{
}

bool GL::Init(void)
{
	const uint32_t CLOCK_ID_V3D = 4;
	const uint32_t TAG_ENABLE_QPU = 0x00030012;
    uint32_t tags[9] = { PROPTAG_GET_CLOCK_RATE, 8, 8, CLOCK_ID_V3D, 250000000, TAG_ENABLE_QPU, 4, 4, 1 };

    if (mbx.GetTags(tags, sizeof(tags)) == false)
        return false;

    if (GL::V3D[V3DReg::IDENT0] != 0x02443356) // magic number
        return false;

    return true;
}

uint32_t GL::MemAlloc(uint32_t size, uint32_t align, MemFlag flags)
{
    const uint32_t TAG_MEM_ALLOC = 0x0003000C;
    uint32_t tags[6] = { TAG_MEM_ALLOC, 12, 12, size, align, flags };

    if (mbx.GetTags(tags, sizeof(tags)) == false)
        return 0;

    return tags[3];
}

bool GL::MemFree(uint32_t handle)
{
    const uint32_t TAG_MEM_RELEASE = 0x0003000F;
    uint32_t tags[4] = { TAG_MEM_RELEASE, 4, 4, handle };

    return mbx.GetTags(tags, sizeof(tags));
}

uint32_t GL::MemLock(uint32_t handle)
{
    const uint32_t TAG_MEM_LOCK = 0x0003000D;
    uint32_t tags[4] = { TAG_MEM_LOCK, 4, 4, handle };

    if (mbx.GetTags(tags, sizeof(tags)) == false)
        return 0;

    return tags[3];
}

bool GL::MemUnlock(uint32_t handle)
{
    const uint32_t TAG_MEM_UNLOCK = 0x0003000E;
    uint32_t tags[4] = { TAG_MEM_UNLOCK, 4, 4, handle };

    return mbx.GetTags(tags, sizeof(tags));
}
