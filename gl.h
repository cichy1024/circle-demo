#ifndef __GL_H__
#define __GL_H__
#include <circle/types.h>
#include <circle/bcmpropertytags.h>
#include <cstdint>

class GL
{
    public:
        enum MemFlag : uint32_t {
            Discardable     = 1 << 0,               /* can be resized to 0 at any time. Use for cached data */
            Normal          = 0 << 2,               /* normal allocating alias. Don't use from ARM */
            Direct          = 1 << 2,		        /* 0xC alias uncached */
            Coherent        = 2 << 2,               /* 0x8 alias. Non-allocating in L2 but coherent */
            L1NonAllocating = (Direct | Coherent),  /* Allocating in L2 */
            Zero            = 1 << 4,               /* initialise buffer to all zeros */
            NoInit          = 1 << 5,               /* don't initialise (default is initialise to all ones */
            HintPermalock   = 1 << 6,               /* Likely to be locked for long periods of time. */
        };

        /* These come from the blob information header .. they start at   KHRN_HW_INSTR_HALT */
        /* https://github.com/raspberrypi/userland/blob/a1b89e91f393c7134b4cdc36431f863bb3333163/middleware/khronos/common/2708/khrn_prod_4.h */
        /* GL pipe control commands */
        enum Op : uint32_t {
            Halt                        = 0,
            Nop                         = 1,
            Flush                       = 4,
            FlushAllState               = 5,
            StartTileBinning            = 6,
            IncrementSemaphore          = 7,
            WaitOnSemaphore             = 8,
            Branch                      = 16,
            BranchToSublist             = 17,
            ReturnFromSublist           = 18,
            StoreMultisample            = 24,
            StoreMultisampleEnd         = 25,
            StoreFullTileBuffer         = 26,
            ReloadFullTileBuffer        = 27,
            StoreTileBuffer             = 28,
            LoadTileBuffer              = 29,
            IndexedPrimitiveList        = 32,
            VertexArrayPrimitives       = 33,
            VgCoordinateArrayPrimitives = 41,
            CompressedPrimitiveList     = 48,
            ClipCompressedPrimitiveList = 49,
            PrimitiveListFormat         = 56,
            ShaderState                 = 64,
            NvShaderState               = 65,
            VgShaderState               = 66,
            VgInlineShaderRecord        = 67,
            ConfigState                 = 96,
            FlatShadeFlags              = 97,
            PointsSize                  = 98,
            LineWidth                   = 99,
            RhtXBoundary                = 100,
            DepthOffset                 = 101,
            ClipWindow                  = 102,
            ViewportOffset              = 103,
            ZClippingPlanes             = 104,
            ClipperXyScaling            = 105,
            ClipperZZscaleOffset        = 106, 
            TileBinningConfig           = 112,
            TileRenderConfig            = 113,
            ClearColors                 = 114,
            TileCoordinates             = 115
        };

        /* Registers shamelessly copied from Eric AnHolt */
        // Defines for v3d register offsets
        enum V3DReg : uint32_t {
            IDENT0  = 0x000 >> 2,   // V3D Identification 0 (V3D block identity)
            IDENT1  = 0x004 >> 2,   // V3D Identification 1 (V3D Configuration A)
            IDENT2  = 0x008 >> 2,   // V3D Identification 1 (V3D Configuration B)

            SCRATCH = 0x010 >> 2,   // Scratch Register

            L2CACTL = 0x020 >> 2,   // 2 Cache Control
            SLCACTL = 0x024 >> 2,   // Slices Cache Control

            INTCTL  = 0x030 >> 2,   // Interrupt Control
            INTENA  = 0x034 >> 2,   // Interrupt Enables
            INTDIS  = 0x038 >> 2,   // Interrupt Disables

            CT0CS   = 0x100 >> 2,   // Control List Executor Thread 0 Control and Status.
            CT1CS   = 0x104 >> 2,   // Control List Executor Thread 1 Control and Status.
            CT0EA   = 0x108 >> 2,   // Control List Executor Thread 0 End Address.
            CT1EA   = 0x10c >> 2,   // Control List Executor Thread 1 End Address.
            CT0CA   = 0x110 >> 2,   // Control List Executor Thread 0 Current Address.
            CT1CA   = 0x114 >> 2,   // Control List Executor Thread 1 Current Address.
            CT00RA0 = 0x118 >> 2,   // Control List Executor Thread 0 Return Address.
            CT01RA0 = 0x11c >> 2,   // Control List Executor Thread 1 Return Address.
            CT0LC   = 0x120 >> 2,   // Control List Executor Thread 0 List Counter
            CT1LC   = 0x124 >> 2,   // Control List Executor Thread 1 List Counter
            CT0PC   = 0x128 >> 2,   // Control List Executor Thread 0 Primitive List Counter
            CT1PC   = 0x12c >> 2,   // Control List Executor Thread 1 Primitive List Counter

            PCS     = 0x130 >> 2,   // V3D Pipeline Control and Status
            BFC     = 0x134 >> 2,   // Binning Mode Flush Count
            RFC     = 0x138 >> 2,   // Rendering Mode Frame Count

            BPCA    = 0x300 >> 2,   // Current Address of Binning Memory Pool
            BPCS    = 0x304 >> 2,   // Remaining Size of Binning Memory Pool
            BPOA    = 0x308 >> 2,   // Address of Overspill Binning Memory Block
            BPOS    = 0x30c >> 2,   // Size of Overspill Binning Memory Block
            BXCF    = 0x310 >> 2,   // Binner Debug

            SQRSV0  = 0x410 >> 2,   // Reserve QPUs 0-7
            SQRSV1  = 0x414 >> 2,   // Reserve QPUs 8-15
            SQCNTL  = 0x418 >> 2,   // QPU Scheduler Control

            SRQPC   = 0x430 >> 2,   // QPU User Program Request Program Address
            SRQUA   = 0x434 >> 2,   // QPU User Program Request Uniforms Address
            SRQUL   = 0x438 >> 2,   // QPU User Program Request Uniforms Length
            SRQCS   = 0x43c >> 2,   // QPU User Program Request Control and Status

            VPACNTL = 0x500 >> 2,   // VPM Allocator Control
            VPMBASE = 0x504 >> 2,   // VPM base (user) memory reservation

            PCTRC   = 0x670 >> 2,   // Performance Counter Clear
            PCTRE   = 0x674 >> 2,   // Performance Counter Enables

            PCTR0   = 0x680 >> 2,   // Performance Counter Count 0
            PCTRS0  = 0x684 >> 2,   // Performance Counter Mapping 0
            PCTR1   = 0x688 >> 2,   // Performance Counter Count 1
            PCTRS1  = 0x68c >> 2,   // Performance Counter Mapping 1
            PCTR2   = 0x690 >> 2,   // Performance Counter Count 2
            PCTRS2  = 0x694 >> 2,   // Performance Counter Mapping 2
            PCTR3   = 0x698 >> 2,   // Performance Counter Count 3
            PCTRS3  = 0x69c >> 2,   // Performance Counter Mapping 3
            PCTR4   = 0x6a0 >> 2,   // Performance Counter Count 4
            PCTRS4  = 0x6a4 >> 2,   // Performance Counter Mapping 4
            PCTR5   = 0x6a8 >> 2,   // Performance Counter Count 5
            PCTRS5  = 0x6ac >> 2,   // Performance Counter Mapping 5
            PCTR6   = 0x6b0 >> 2,   // Performance Counter Count 6
            PCTRS6  = 0x6b4 >> 2,   // Performance Counter Mapping 6
            PCTR7   = 0x6b8 >> 2,   // Performance Counter Count 7
            PCTRS7  = 0x6bc >> 2,   // Performance Counter Mapping 7 
            PCTR8   = 0x6c0 >> 2,   // Performance Counter Count 8
            PCTRS8  = 0x6c4 >> 2,   // Performance Counter Mapping 8
            PCTR9   = 0x6c8 >> 2,   // Performance Counter Count 9
            PCTRS9  = 0x6cc >> 2,   // Performance Counter Mapping 9
            PCTR10  = 0x6d0 >> 2,   // Performance Counter Count 10
            PCTRS10 = 0x6d4 >> 2,   // Performance Counter Mapping 10
            PCTR11  = 0x6d8 >> 2,   // Performance Counter Count 11
            PCTRS11 = 0x6dc >> 2,   // Performance Counter Mapping 11
            PCTR12  = 0x6e0 >> 2,   // Performance Counter Count 12
            PCTRS12 = 0x6e4 >> 2,   // Performance Counter Mapping 12
            PCTR13  = 0x6e8 >> 2,   // Performance Counter Count 13
            PCTRS13 = 0x6ec >> 2,   // Performance Counter Mapping 13
            PCTR14  = 0x6f0 >> 2,   // Performance Counter Count 14
            PCTRS14 = 0x6f4 >> 2,   // Performance Counter Mapping 14
            PCTR15  = 0x6f8 >> 2,   // Performance Counter Count 15
            PCTRS15 = 0x6fc >> 2,   // Performance Counter Mapping 15

            DBGE    = 0xf00 >> 2,   // PSE Error Signals
            FDBGO   = 0xf04 >> 2,   // FEP Overrun Error Signals
            FDBGB   = 0xf08 >> 2,   // FEP Interface Ready and Stall Signals, FEP Busy Signals
            FDBGR   = 0xf0c >> 2,   // FEP Internal Ready Signals
            FDBGS   = 0xf10 >> 2,   // FEP Internal Stall Input Signals

            ERRSTAT = 0xf20 >> 2,   // Miscellaneous Error Signals (VPM, VDW, VCD, VCM, L2C)
        };

    public:
        GL(void);
        virtual ~GL(void) = default;
        bool Init(void);
        uint32_t MemAlloc(uint32_t size, uint32_t align, MemFlag flags);
        bool MemFree(uint32_t handle);
        uint32_t MemLock(uint32_t handle);
        bool MemUnlock(uint32_t handle);

        static volatile __attribute__((aligned(4))) uint32_t* V3D;

        static constexpr void *ArmPtr(uint32_t handle) {
            return reinterpret_cast<void *>(static_cast<uintptr_t>(handle & 0x3fffffff));
        }

        static constexpr uint32_t GpuPtr(void *addr) {
            return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(addr)) | 0xc0000000;
        }

    private:
        CBcmPropertyTags mbx;
};

#endif /* __GL_H__ */
