#if !defined(GAMESYSTEMINFO_H)
#define GAMESYSTEMINFO_H

#if defined(_WIN32)
#include <intrin.h>
#include <psapi.h> 
#include <powerbase.h>
#elif defined(__linux__)
#include <cpuid.h>
#include <cstring>
#endif
#include <sstream>
#include <string>
#include <thread>


namespace game
{

	// Contains information about the host's system
	class SystemInfo
	{
	private:
		// Contains information about the host's cpu
		struct CPUInfo
		{
		public:
			std::string processorVendor;
			std::string processorName;
			uint32_t processorCount = 0;
		};
        // Contains information about the host's RAM
        struct RAMInfo
        {
        public:
            uint64_t totalVirtualMemory = 0;
            uint64_t totalVirtualMemoryUsed = 0;
            uint64_t totalVirtualMemoryUsedByGame = 0;
            uint64_t totalVirtualMemoryAvailable = 0;
            uint64_t totalPhysicalMemory = 0;
            uint64_t totalPhysicalMemoryUsed = 0;
            uint64_t totalPhysicalMemoryAvailable = 0;
            uint64_t totalPhysicalMemoryUsedByGame = 0;
        };
		// Contains information about the host's renderer
		struct GPUInfo
		{
			int32_t internalPixelFormat = 0;
			int32_t internalPixelType = 0;
			int32_t totalMemory = 0;
			int32_t freeMemory = 0;
			std::string version;
			int32_t versionMajor = 0;
			int32_t versionMinor = 0;
			std::string maxShaderLanguageVersion;
			int32_t maxAnisotropy = 1;
			int32_t maxMultisamples = 0;
			int32_t multisampleSamples = 0;
			int32_t frontBufferColorSize = 0;
			int32_t backBufferColorSize = 0;
			int32_t depthBufferSize = 0;
			std::string renderer;
			std::string vendor;
		};
	public:
		CPUInfo cpuInfo;
		GPUInfo gpuInfo;
        RAMInfo ramInfo;
		void GetCPUInfo();
	};

    inline void SystemInfo::GetCPUInfo()
    {
        const char* szFeatures[] =
        {
            "x87 FPU On Chip",
            "Virtual-8086 Mode Enhancement",
            "Debugging Extensions",
            "Page Size Extensions",
            "Time Stamp Counter",
            "RDMSR and WRMSR Support",
            "Physical Address Extensions",
            "Machine Check Exception",
            "CMPXCHG8B Instruction",
            "APIC On Chip",
            "Unknown1",
            "SYSENTER and SYSEXIT",
            "Memory Type Range Registers",
            "PTE Global Bit",
            "Machine Check Architecture",
            "Conditional Move/Compare Instruction",
            "Page Attribute Table",
            "36-bit Page Size Extension",
            "Processor Serial Number",
            "CFLUSH Extension",
            "Unknown2",
            "Debug Store",
            "Thermal Monitor and Clock Ctrl",
            "MMX Technology",
            "FXSAVE/FXRSTOR",
            "SSE Extensions",
            "SSE2 Extensions",
            "Self Snoop",
            "Multithreading Technology",
            "Thermal Monitor",
            "Unknown4",
            "Pending Break Enable"
        };

        char CPUString[0x20] = { 0 };
        char CPUBrandString[0x40] = { 0 };
        int CPUInfo[4] = { -1 };
        int nSteppingID = 0;
        int nModel = 0;
        int nFamily = 0;
        int nProcessorType = 0;
        int nExtendedmodel = 0;
        int nExtendedfamily = 0;
        int nBrandIndex = 0;
        int nCLFLUSHcachelinesize = 0;
        int nLogicalProcessors = 0;
        int nAPICPhysicalID = 0;
        int nFeatureInfo = 0;
        int nCacheLineSize = 0;
        int nL2Associativity = 0;
        int nCacheSizeK = 0;
        int nPhysicalAddress = 0;
        int nVirtualAddress = 0;
        int nRet = 0;

        int nCores = 0;
        int nCacheType = 0;
        int nCacheLevel = 0;
        int nMaxThread = 0;
        int nSysLineSize = 0;
        int nPhysicalLinePartitions = 0;
        int nWaysAssociativity = 0;
        int nNumberSets = 0;

        unsigned    nIds, nExIds, i;

        bool    bSSE3Instructions = false;
        bool    bMONITOR_MWAIT = false;
        bool    bCPLQualifiedDebugStore = false;
        bool    bVirtualMachineExtensions = false;
        bool    bEnhancedIntelSpeedStepTechnology = false;
        bool    bThermalMonitor2 = false;
        bool    bSupplementalSSE3 = false;
        bool    bL1ContextID = false;
        bool    bCMPXCHG16B = false;
        bool    bxTPRUpdateControl = false;
        bool    bPerfDebugCapabilityMSR = false;
        bool    bSSE41Extensions = false;
        bool    bSSE42Extensions = false;
        bool    bPOPCNT = false;

        bool    bMultithreading = false;

        bool    bLAHF_SAHFAvailable = false;
        bool    bCmpLegacy = false;
        bool    bSVM = false;
        bool    bExtApicSpace = false;
        bool    bAltMovCr8 = false;
        bool    bLZCNT = false;
        bool    bSSE4A = false;
        bool    bMisalignedSSE = false;
        bool    bPREFETCH = false;
        bool    bSKINITandDEV = false;
        bool    bSYSCALL_SYSRETAvailable = false;
        bool    bExecuteDisableBitAvailable = false;
        bool    bMMXExtensions = false;
        bool    bFFXSR = false;
        bool    b1GBSupport = false;
        bool    bRDTSCP = false;
        bool    b64Available = false;
        bool    b3DNowExt = false;
        bool    b3DNow = false;
        bool    bNestedPaging = false;
        bool    bLBRVisualization = false;
        bool    bFP128 = false;
        bool    bMOVOptimization = false;

        bool    bSelfInit = false;
        bool    bFullyAssociative = false;

        // __cpuid with an InfoType argument of 0 returns the number of
        // valid Ids in CPUInfo[0] and the CPU identification string in
        // the other three array elements. The CPU identification string is
        // not in linear order. The code below arranges the information 
        // in a human readable form.
#if defined(_WIN32)
        __cpuid(CPUInfo, 0);
#elif defined(__linux__)

#endif
        nIds = CPUInfo[0];
        *((int*)CPUString) = CPUInfo[1];
        *((int*)(CPUString + 4)) = CPUInfo[3];
        *((int*)(CPUString + 8)) = CPUInfo[2];

        // Get the information associated with each valid Id
        for (i = 0; i <= nIds; ++i)
        {
#if defined(_WIN32)
            __cpuid(CPUInfo, i);
#elif defined(__linux__)

#endif

            // Interpret CPU feature information.
            if (i == 1)
            {
                nSteppingID = CPUInfo[0] & 0xf;
                nModel = (CPUInfo[0] >> 4) & 0xf;
                nFamily = (CPUInfo[0] >> 8) & 0xf;
                nProcessorType = (CPUInfo[0] >> 12) & 0x3;
                nExtendedmodel = (CPUInfo[0] >> 16) & 0xf;
                nExtendedfamily = (CPUInfo[0] >> 20) & 0xff;
                nBrandIndex = CPUInfo[1] & 0xff;
                nCLFLUSHcachelinesize = ((CPUInfo[1] >> 8) & 0xff) * 8;
                nLogicalProcessors = ((CPUInfo[1] >> 16) & 0xff);
                nAPICPhysicalID = (CPUInfo[1] >> 24) & 0xff;
                bSSE3Instructions = (CPUInfo[2] & 0x1) || false;
                bMONITOR_MWAIT = (CPUInfo[2] & 0x8) || false;
                bCPLQualifiedDebugStore = (CPUInfo[2] & 0x10) || false;
                bVirtualMachineExtensions = (CPUInfo[2] & 0x20) || false;
                bEnhancedIntelSpeedStepTechnology = (CPUInfo[2] & 0x80) || false;
                bThermalMonitor2 = (CPUInfo[2] & 0x100) || false;
                bSupplementalSSE3 = (CPUInfo[2] & 0x200) || false;
                bL1ContextID = (CPUInfo[2] & 0x300) || false;
                bCMPXCHG16B = (CPUInfo[2] & 0x2000) || false;
                bxTPRUpdateControl = (CPUInfo[2] & 0x4000) || false;
                bPerfDebugCapabilityMSR = (CPUInfo[2] & 0x8000) || false;
                bSSE41Extensions = (CPUInfo[2] & 0x80000) || false;
                bSSE42Extensions = (CPUInfo[2] & 0x100000) || false;
                bPOPCNT = (CPUInfo[2] & 0x800000) || false;
                nFeatureInfo = CPUInfo[3];
                bMultithreading = (nFeatureInfo & (1 << 28)) || false;
            }
        }

        // Calling __cpuid with 0x80000000 as the InfoType argument
        // gets the number of valid extended IDs.
#if defined(_WIN32)
        __cpuid(CPUInfo, 0x80000000);
#elif defined(__linux__)

#endif
        nExIds = CPUInfo[0];

        // Get the information associated with each extended ID.
        for (i = 0x80000000; i <= nExIds; ++i)
        {
#if defined(_WIN32)
            __cpuid(CPUInfo, i);
#elif defined(__linux__)

#endif

            if (i == 0x80000001)
            {
                bLAHF_SAHFAvailable = (CPUInfo[2] & 0x1) || false;
                bCmpLegacy = (CPUInfo[2] & 0x2) || false;
                bSVM = (CPUInfo[2] & 0x4) || false;
                bExtApicSpace = (CPUInfo[2] & 0x8) || false;
                bAltMovCr8 = (CPUInfo[2] & 0x10) || false;
                bLZCNT = (CPUInfo[2] & 0x20) || false;
                bSSE4A = (CPUInfo[2] & 0x40) || false;
                bMisalignedSSE = (CPUInfo[2] & 0x80) || false;
                bPREFETCH = (CPUInfo[2] & 0x100) || false;
                bSKINITandDEV = (CPUInfo[2] & 0x1000) || false;
                bSYSCALL_SYSRETAvailable = (CPUInfo[3] & 0x800) || false;
                bExecuteDisableBitAvailable = (CPUInfo[3] & 0x10000) || false;
                bMMXExtensions = (CPUInfo[3] & 0x40000) || false;
                bFFXSR = (CPUInfo[3] & 0x200000) || false;
                b1GBSupport = (CPUInfo[3] & 0x400000) || false;
                bRDTSCP = (CPUInfo[3] & 0x8000000) || false;
                b64Available = (CPUInfo[3] & 0x20000000) || false;
                b3DNowExt = (CPUInfo[3] & 0x40000000) || false;
                b3DNow = (CPUInfo[3] & 0x80000000) || false;
            }

            // Interpret CPU brand string and cache information.
            if (i == 0x80000002)
                std::memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
            else if (i == 0x80000003)
                memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
            else if (i == 0x80000004)
                memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));

        }

        //// Display all the information in user-friendly format.

        //printf_s("\n\nCPU String: %s\n", CPUString);

        //if (nIds >= 1)
        //{
        //    if (nSteppingID)
        //        printf_s("Stepping ID = %d\n", nSteppingID);
        //    if (nModel)
        //        printf_s("Model = %d\n", nModel);
        //    if (nFamily)
        //        printf_s("Family = %d\n", nFamily);
        //    if (nProcessorType)
        //        printf_s("Processor Type = %d\n", nProcessorType);
        //    if (nExtendedmodel)
        //        printf_s("Extended model = %d\n", nExtendedmodel);
        //    if (nExtendedfamily)
        //        printf_s("Extended family = %d\n", nExtendedfamily);
        //    if (nBrandIndex)
        //        printf_s("Brand Index = %d\n", nBrandIndex);
        //    if (nCLFLUSHcachelinesize)
        //        printf_s("CLFLUSH cache line size = %d\n",
        //            nCLFLUSHcachelinesize);
        //    if (bMultithreading && (nLogicalProcessors > 0))
        //        printf_s("Logical Processor Count = %d\n", nLogicalProcessors);
        //    if (nAPICPhysicalID)
        //        printf_s("APIC Physical ID = %d\n", nAPICPhysicalID);

        //    if (nFeatureInfo || bSSE3Instructions ||
        //        bMONITOR_MWAIT || bCPLQualifiedDebugStore ||
        //        bVirtualMachineExtensions || bEnhancedIntelSpeedStepTechnology ||
        //        bThermalMonitor2 || bSupplementalSSE3 || bL1ContextID ||
        //        bCMPXCHG16B || bxTPRUpdateControl || bPerfDebugCapabilityMSR ||
        //        bSSE41Extensions || bSSE42Extensions || bPOPCNT ||
        //        bLAHF_SAHFAvailable || bCmpLegacy || bSVM ||
        //        bExtApicSpace || bAltMovCr8 ||
        //        bLZCNT || bSSE4A || bMisalignedSSE ||
        //        bPREFETCH || bSKINITandDEV || bSYSCALL_SYSRETAvailable ||
        //        bExecuteDisableBitAvailable || bMMXExtensions || bFFXSR || b1GBSupport ||
        //        bRDTSCP || b64Available || b3DNowExt || b3DNow || bNestedPaging ||
        //        bLBRVisualization || bFP128 || bMOVOptimization)
        //    {
        //        printf_s("\nThe following features are supported:\n");

        //        if (bSSE3Instructions)
        //            printf_s("\tSSE3\n");
        //        if (bMONITOR_MWAIT)
        //            printf_s("\tMONITOR/MWAIT\n");
        //        if (bCPLQualifiedDebugStore)
        //            printf_s("\tCPL Qualified Debug Store\n");
        //        if (bVirtualMachineExtensions)
        //            printf_s("\tVirtual Machine Extensions\n");
        //        if (bEnhancedIntelSpeedStepTechnology)
        //            printf_s("\tEnhanced Intel SpeedStep Technology\n");
        //        if (bThermalMonitor2)
        //            printf_s("\tThermal Monitor 2\n");
        //        if (bSupplementalSSE3)
        //            printf_s("\tSupplemental Streaming SIMD Extensions 3\n");
        //        if (bL1ContextID)
        //            printf_s("\tL1 Context ID\n");
        //        if (bCMPXCHG16B)
        //            printf_s("\tCMPXCHG16B Instruction\n");
        //        if (bxTPRUpdateControl)
        //            printf_s("\txTPR Update Control\n");
        //        if (bPerfDebugCapabilityMSR)
        //            printf_s("\tPerf\\Debug Capability MSR\n");
        //        if (bSSE41Extensions)
        //            printf_s("\tSSE4.1 Extensions\n");
        //        if (bSSE42Extensions)
        //            printf_s("\tSSE4.2 Extensions\n");
        //        if (bPOPCNT)
        //            printf_s("\tPPOPCNT Instruction\n");

        //        i = 0;
        //        nIds = 1;
        //        while (i < (sizeof(szFeatures) / sizeof(const char*)))
        //        {
        //            if (nFeatureInfo & nIds)
        //            {
        //                printf_s("\t");
        //                printf_s(szFeatures[i]);
        //                printf_s("\n");
        //            }

        //            nIds <<= 1;
        //            ++i;
        //        }
        //        if (bLAHF_SAHFAvailable)
        //            printf_s("\tLAHF/SAHF in 64-bit mode\n");
        //        if (bCmpLegacy)
        //            printf_s("\tCore multi-processing legacy mode\n");
        //        if (bSVM)
        //            printf_s("\tSecure Virtual Machine\n");
        //        if (bExtApicSpace)
        //            printf_s("\tExtended APIC Register Space\n");
        //        if (bAltMovCr8)
        //            printf_s("\tAltMovCr8\n");
        //        if (bLZCNT)
        //            printf_s("\tLZCNT instruction\n");
        //        if (bSSE4A)
        //            printf_s("\tSSE4A (EXTRQ, INSERTQ, MOVNTSD, MOVNTSS)\n");
        //        if (bMisalignedSSE)
        //            printf_s("\tMisaligned SSE mode\n");
        //        if (bPREFETCH)
        //            printf_s("\tPREFETCH and PREFETCHW Instructions\n");
        //        if (bSKINITandDEV)
        //            printf_s("\tSKINIT and DEV support\n");
        //        if (bSYSCALL_SYSRETAvailable)
        //            printf_s("\tSYSCALL/SYSRET in 64-bit mode\n");
        //        if (bExecuteDisableBitAvailable)
        //            printf_s("\tExecute Disable Bit\n");
        //        if (bMMXExtensions)
        //            printf_s("\tExtensions to MMX Instructions\n");
        //        if (bFFXSR)
        //            printf_s("\tFFXSR\n");
        //        if (b1GBSupport)
        //            printf_s("\t1GB page support\n");
        //        if (bRDTSCP)
        //            printf_s("\tRDTSCP instruction\n");
        //        if (b64Available)
        //            printf_s("\t64 bit Technology\n");
        //        if (b3DNowExt)
        //            printf_s("\t3Dnow Ext\n");
        //        if (b3DNow)
        //            printf_s("\t3Dnow! instructions\n");
        //        if (bNestedPaging)
        //            printf_s("\tNested Paging\n");
        //        if (bLBRVisualization)
        //            printf_s("\tLBR Visualization\n");
        //        if (bFP128)
        //            printf_s("\tFP128 optimization\n");
        //        if (bMOVOptimization)
        //            printf_s("\tMOVU Optimization\n");
        //    }
        //}

        //if (nExIds >= 0x80000004)
        //    printf_s("\nCPU Brand String: %s\n", CPUBrandString);

        //if (nExIds >= 0x80000006)
        //{
        //    printf_s("Cache Line Size = %d\n", nCacheLineSize);
        //    printf_s("L2 Associativity = %d\n", nL2Associativity);
        //    printf_s("Cache Size = %dK\n", nCacheSizeK);
        //}

        // Save cpu info into global struct
        cpuInfo.processorCount = nLogicalProcessors;
        CPUBrandString[63] = '\0';
        cpuInfo.processorName = CPUBrandString;
        cpuInfo.processorVendor = CPUString;

        // ram info
#if defined(_WIN32)
        MEMORYSTATUSEX memInfo = { 0 };
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        uint64_t totalVirtualMem = memInfo.ullTotalPageFile;
        uint64_t virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
        PROCESS_MEMORY_COUNTERS_EX pmc = {0};


        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
        uint64_t virtualMemUsedByThis = pmc.PrivateUsage;
        uint64_t totalPhysMem = memInfo.ullTotalPhys;
        uint64_t physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
        uint64_t physMemUsedByMe = pmc.WorkingSetSize;


		ramInfo.totalVirtualMemory = (uint64_t)(totalVirtualMem / 1024.0f / 1024.0f);
		ramInfo.totalVirtualMemoryUsed = (uint64_t)(virtualMemUsed / 1024.0f / 1024.0f);
		ramInfo.totalVirtualMemoryUsedByGame = (uint64_t)(virtualMemUsedByThis / 1024.0f / 1024.0f);
        ramInfo.totalVirtualMemoryAvailable = (uint64_t)(memInfo.ullAvailPageFile / 1024.0f / 1024.0f);


		ramInfo.totalPhysicalMemory = (uint64_t)(totalPhysMem / 1024.0f / 1024.0f);
		ramInfo.totalPhysicalMemoryUsed = (uint64_t)(physMemUsed / 1024.0f / 1024.0f);
		ramInfo.totalPhysicalMemoryUsedByGame = (uint64_t)(physMemUsedByMe / 1024.0f / 1024.0f);
        ramInfo.totalPhysicalMemoryAvailable = (uint64_t)(memInfo.ullAvailPhys / 1024.0f / 1024.0f);
#elif defined(__linux__)
#endif


        // cpu usage
       
    }

}

#endif
