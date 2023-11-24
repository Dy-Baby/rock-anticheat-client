#include "virtual_machine.h"

bool GameVirtual::isVmWare()
{
    bool res = true;

    __try
    {
        __asm
        {
            push   edx
            push   ecx
            push   ebx

            mov    eax, 'VMXh'
            mov    ebx, 0
            mov    ecx, 10 // get VMWare version
            mov    edx, 'VX' // port number

            in     eax, dx // read port
                           // on return EAX returns the VERSION

                           cmp    ebx, 'VMXh' // compare with target
                           setz[res] // set return value

                           pop    ebx
                           pop    ecx
                           pop    edx
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        res = false;
    }

    return res;
}

std::string GameVirtual::getVMname()
{
    int cpuInfo[4] = {};

    __cpuid(cpuInfo, 1);


    if (!(cpuInfo[2] & (1 << 31)))
        return XorStr("Unknown");

    //
    // A hypervisor is running on the machine. Query the vendor id.
    //
    const auto queryVendorIdMagic = 0x40000000;
    __cpuid(cpuInfo, queryVendorIdMagic);

    const int vendorIdLength = 13;
    using VendorIdStr = char[vendorIdLength];

    VendorIdStr hyperVendorId = {};

    memcpy(hyperVendorId + 0, &cpuInfo[1], 4);
    memcpy(hyperVendorId + 4, &cpuInfo[2], 4);
    memcpy(hyperVendorId + 8, &cpuInfo[3], 4);
    hyperVendorId[12] = '\0';

    static const VendorIdStr vendors[]{
    "KVMKVMKVM\0\0\0", // KVM 
    //"Microsoft Hv",    // Microsoft Hyper-V or Windows Virtual PC */
    "VMwareVMware",    // VMware 
    "XenVMMXenVMM",    // Xen 
    "prl hyperv  ",    // Parallels
    "VBoxVBoxVBox"     // VirtualBox 
    };

    for (const auto& vendor : vendors)
    {
        if (!memcmp(vendor, hyperVendorId, vendorIdLength))
            return vendor;
    }

    return XorStr("Unknown");
}

bool GameVirtual::checkVirtualMachine()
{
    if (isVmWare())
    {
        // report
        Report::VMInfo info;
        info.name = XorStr("VMWare");
        Report::sendVMReport(&info);

        return true;
    }

    std::string vendor = getVMname();
    if (vendor != XorStr("Unknown"))
    {
        // report
        Report::VMInfo info;
        info.name = vendor;
        Report::sendVMReport(&info);

        return true;
    }

    return false;
}