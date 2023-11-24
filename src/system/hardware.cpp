#include "hardware.h"

std::vector<std::string> Hardware::mac_address;
std::vector<std::string> Hardware::guid_drives;
std::string Hardware::cpu_hash;
std::string Hardware::pc_name;
std::string Hardware::user_guid;
std::string Hardware::motherboard_serial;

WmiQueryResult Hardware::getWmiQueryResult(std::wstring wmiQuery, std::wstring propNameOfResultObject, bool allowEmptyItems = false) {

    WmiQueryResult retVal;

    HRESULT hres;

    IWbemLocator* pLoc = NULL;
    IWbemServices* pSvc = NULL;
    IEnumWbemClassObject* pEnumerator = NULL;
    IWbemClassObject* pclsObj = NULL;
    VARIANT vtProp;


    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres))
    {
        retVal.Error = WmiQueryError::ComInitializationFailure;
        retVal.error_name = XorStr("ComInitializationFailure");
    }
    else
    {
          pLoc = NULL;

            hres = CoCreateInstance(
                CLSID_WbemLocator,
                0,
                CLSCTX_INPROC_SERVER,
                IID_IWbemLocator, (LPVOID*)&pLoc);

            if (FAILED(hres))
            {
                retVal.Error = WmiQueryError::IWbemLocatorFailure;
                retVal.error_name = XorStr("IWbemLocatorFailure");
            }
            else
            {
                pSvc = NULL;

                hres = pLoc->ConnectServer(
                    _bstr_t(XorStrW(L"ROOT\\CIMV2")), // Object path of WMI namespace
                    NULL,                    // User name. NULL = current user
                    NULL,                    // User password. NULL = current
                    0,                       // Locale. NULL indicates current
                    NULL,                    // Security flags.
                    0,                       // Authority (for example, Kerberos)
                    0,                       // Context object 
                    &pSvc                    // pointer to IWbemServices proxy
                );

                if (FAILED(hres))
                {
                    retVal.Error = WmiQueryError::IWbemServiceConnectionFailure;
                    retVal.error_name = XorStr("IWbemServiceConnectionFailure");
                }
                else
                {
                    hres = CoSetProxyBlanket(
                        pSvc,                        // Indicates the proxy to set
                        RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
                        RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
                        NULL,                        // Server principal name 
                        RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
                        RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
                        NULL,                        // client identity
                        EOAC_NONE                    // proxy capabilities 
                    );

                    if (FAILED(hres))
                    {
                        retVal.Error = WmiQueryError::BlanketProxySetFailure;
                        retVal.error_name = XorStr("BlanketProxySetFailure");
                    }
                    else
                    {
                        pEnumerator = NULL;
                        hres = pSvc->ExecQuery(
                            bstr_t(XorStr("WQL")),
                            bstr_t(wmiQuery.c_str()),
                            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                            NULL,
                            &pEnumerator);

                        if (FAILED(hres))
                        {
                            retVal.Error = WmiQueryError::BadQueryFailure;
                            retVal.error_name = XorStr("BadQueryFailure");
                        }
                        else
                        {
                            pclsObj = NULL;
                            ULONG uReturn = 0;

                            while (pEnumerator)
                            {
                                HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
                                    &pclsObj, &uReturn);

                                if (0 == uReturn)
                                {
                                    break;
                                }

                                hr = pclsObj->Get(propNameOfResultObject.c_str(), 0, &vtProp, 0, 0);
                                if (S_OK != hr) {
                                    retVal.Error = WmiQueryError::PropertyExtractionFailure;
                                    retVal.error_name = XorStr("PropertyExtractionFailure");
                                }
                                else {
                                    BSTR val = vtProp.bstrVal;

                                    if (NULL == val) {
                                        if (allowEmptyItems) {
                                            retVal.ResultList.push_back(std::wstring(L""));
                                        }
                                    }
                                    else {
                                        retVal.ResultList.push_back(std::wstring(val));
                                    }
                                }
                            }
                        }
                    }
                }
            }
    }

    VariantClear(&vtProp);
    if (pclsObj)
        pclsObj->Release();

    if (pSvc)
        pSvc->Release();

    if (pLoc)
        pLoc->Release();

    if (pEnumerator)
        pEnumerator->Release();

    CoUninitialize();

    return retVal;
}

bool Hardware::getMacAddress()
{
    IP_ADAPTER_INFO* info = NULL, * pos;
    DWORD size = 0;

    LI_FN(GetAdaptersInfo).safe()(info, &size);

    info = (IP_ADAPTER_INFO*)malloc(size);

    LI_FN(GetAdaptersInfo).safe()(info, &size);

    for (pos = info; pos != NULL; pos = pos->Next) {
        char* tmp = (char*)malloc(18);
        sprintf(tmp, XorStr("%2.2x"), pos->Address[0]);
        for (int i = 1; i < pos->AddressLength; i++)
            sprintf(tmp, XorStr("%s:%2.2x"), tmp, pos->Address[i]);

        Hardware::mac_address.push_back(tmp);
    }

    free(info);

    return (!mac_address.empty());
}

bool Hardware::getDriveGUID()
{
    WmiQueryResult res;
    
    res = Hardware::getWmiQueryResult(XorStrW(L"SELECT SerialNumber FROM Win32_PhysicalMedia"), XorStrW(L"SerialNumber"), false);

    if (res.Error == WmiQueryError::None) {
        for (const auto& item : res.ResultList) {
            std::string serial = Core::UTF16ToUTF8(item);
            std::string::iterator end_pos = std::remove(serial.begin(), serial.end(), ' ');
            serial.erase(end_pos, serial.end());
            
            Hardware::guid_drives.push_back(serial);
        }
    }
    else if (res.Error != WmiQueryError::None)
    {
        Report::ErrorInfo info;
        info.error = std::to_string(GetLastError());
        info.call = XorStr("getWmiQueryResult(L\"SELECT SerialNumber FROM Win32_PhysicalMedia\", L\"SerialNumber\", false)");
        info.ret = res.error_name;
        info.wh = XorStr("Hardware::getDriveGUID()");
        Report::sendErrorReport(&info);
    }

    // possible fix 0x7000002 error
    if (!guid_drives.empty())
    {
        res = Hardware::getWmiQueryResult(XorStrW(L"SELECT SerialNumber FROM Win32_DiskDrive"), XorStrW(L"SerialNumber"), false);

        if (res.Error == WmiQueryError::None) {
            for (const auto& item : res.ResultList) {
                std::string serial = Core::UTF16ToUTF8(item);
                std::string::iterator end_pos = std::remove(serial.begin(), serial.end(), ' ');
                serial.erase(end_pos, serial.end());

                Hardware::guid_drives.push_back(serial);
            }
        }
        else if (res.Error != WmiQueryError::None)
        {
            Report::ErrorInfo info;
            info.error = std::to_string(GetLastError());
            info.call = XorStr("getWmiQueryResult(L\"SELECT SerialNumber FROM Win32_DiskDrive\", L\"SerialNumber\", false)");
            info.ret = res.error_name;
            info.wh = XorStr("Hardware::getDriveGUID()");
            Report::sendErrorReport(&info);
        }
    }

    return (!guid_drives.empty());
}

bool Hardware::getCPUHash()
{
    int cpuinfo[4] = { 0, 0, 0, 0 };
    __cpuid(cpuinfo, 0);
    std::int16_t hash = 0;
    std::int16_t* ptr = (std::int16_t*)(&cpuinfo[0]);
    for (std::int32_t i = 0; i < 8; i++)
        hash += ptr[i];

    Hardware::cpu_hash = std::to_string(hash);

    return (!Hardware::cpu_hash.empty());
}

bool Hardware::getMotherboardSerial()
{
    WmiQueryResult res;

    res = Hardware::getWmiQueryResult(XorStrW(L"SELECT SerialNumber FROM Win32_BaseBoard"), XorStrW(L"SerialNumber"), false);
    //res = Hardware::getWmiQueryResult(L"SELECT SerialNumber FROM Win32_PhysicalMedia", L"SerialNumber", false);


    if (res.Error == WmiQueryError::None) {
        for (const auto& item : res.ResultList) {
            std::string serial = Core::UTF16ToUTF8(item);
            std::string::iterator end_pos = std::remove(serial.begin(), serial.end(), ' ');
            serial.erase(end_pos, serial.end());

            Hardware::motherboard_serial = serial;
        }
    }
    else if(res.Error != WmiQueryError::None)
    {
        // �� ������� ��������.
        Hardware::motherboard_serial = XorStr("00000000");
    }

    if (motherboard_serial.empty())
    {
        Hardware::motherboard_serial = XorStr("00000000");
    }

    return (!motherboard_serial.empty());
}

bool Hardware::getPCName()
{
    wchar_t computerName[1024];
    DWORD size = 1024;

    LI_FN(GetComputerNameW).safe()(computerName, &size);
    std::wstring wstr(computerName);
    std::string str(wstr.begin(), wstr.end());

    Hardware::pc_name = str;

    return (!Hardware::pc_name.empty());
}

bool Hardware::getUserGUID()
{
    /*char guidbuf[512];
    char path[512];
    char pathdir[512];
    sprintf(pathdir, XorStr("%s/.anti"), getenv(XorStr("APPDATA")));
    sprintf(path, XorStr("%s/.anti/f567765.dex"), getenv(XorStr("APPDATA")));

    wchar_t wpathdir[512];
    mbstowcs(wpathdir, pathdir, strlen(pathdir) + 1);//Plus null
    LPWSTR ptr = wpathdir;

    std::ifstream file;
    file.open(path);
    if (!file.is_open())
    {
        auto kernel_dll = LI_FN(LoadLibraryA)(XorStr("kernel32.dll"));
        LI_FN(CreateDirectoryW).in_safe(kernel_dll)(ptr, NULL);
        LI_FN(SetFileAttributesW).in_safe(kernel_dll)(ptr, FILE_ATTRIBUTE_HIDDEN);

        std::ofstream File;
        File.open(path);
        printf("%d\n", File.is_open());
        if (!File.is_open()) return 0;

        GUID guid;
        CoCreateGuid(&guid);
        sprintf(guidbuf, XorStr("%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX"),
            guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
            guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

        File << guidbuf;
        //File.write(guidbuf, sizeof(guidbuf));
        Hardware::user_guid = guidbuf;
        File.close();
    }
    else
    {
        std::string line;
        while (std::getline(file, line))

            Hardware::user_guid = line;
    }
    file.close();
    */

    std::string components;

    WmiQueryResult res;

    // ���������
    res = Hardware::getWmiQueryResult(XorStrW(L"SELECT Manufacturer FROM Win32_BaseBoard"), XorStrW(L"Manufacturer"), false);
    if (res.Error == WmiQueryError::None) {
        for (const auto& item : res.ResultList) {
            std::string serial = Core::UTF16ToUTF8(item);
            std::string::iterator end_pos = std::remove(serial.begin(), serial.end(), ' ');
            serial.erase(end_pos, serial.end());

            if (serial.length() > 0)
                components.append(serial);
        }
    }

    res = Hardware::getWmiQueryResult(XorStrW(L"SELECT Product FROM Win32_BaseBoard"), XorStrW(L"Product"), false);
    if (res.Error == WmiQueryError::None) {
        for (const auto& item : res.ResultList) {
            std::string serial = Core::UTF16ToUTF8(item);
            std::string::iterator end_pos = std::remove(serial.begin(), serial.end(), ' ');
            serial.erase(end_pos, serial.end());

            if (serial.length() > 0)
                components.append(serial);
        }
    }

    res = Hardware::getWmiQueryResult(XorStrW(L"SELECT DeviceID FROM Win32_IDEController"), XorStrW(L"DeviceID"), false);
    if (res.Error == WmiQueryError::None) {
        for (const auto& item : res.ResultList) {
            std::string serial = Core::UTF16ToUTF8(item);
            std::string::iterator end_pos = std::remove(serial.begin(), serial.end(), ' ');
            serial.erase(end_pos, serial.end());

            if (serial.length() > 0)
                components.append(serial);
        }
    }

    res = Hardware::getWmiQueryResult(XorStrW(L"SELECT Name FROM Win32_CPU"), XorStrW(L"Name"), false);
    if (res.Error == WmiQueryError::None) {
        for (const auto& item : res.ResultList) {
            std::string serial = Core::UTF16ToUTF8(item);
            std::string::iterator end_pos = std::remove(serial.begin(), serial.end(), ' ');
            serial.erase(end_pos, serial.end());

            if (serial.length() > 0)
                components.append(serial);
        }
    }

    res = Hardware::getWmiQueryResult(XorStrW(L"SELECT ProcessorId FROM Win32_Processor"), XorStrW(L"ProcessorId"), false);
    if (res.Error == WmiQueryError::None) {
        for (const auto& item : res.ResultList) {
            std::string serial = Core::UTF16ToUTF8(item);
            std::string::iterator end_pos = std::remove(serial.begin(), serial.end(), ' ');
            serial.erase(end_pos, serial.end());

            if (serial.length() > 0)
                components.append(serial);
        }
    }

    res = Hardware::getWmiQueryResult(XorStrW(L"SELECT TotalPhysicalMemory FROM Win32_ComputerSystem"), XorStrW(L"TotalPhysicalMemory"), false);
    if (res.Error == WmiQueryError::None) {
        for (const auto& item : res.ResultList) {
            std::string serial = Core::UTF16ToUTF8(item);
            std::string::iterator end_pos = std::remove(serial.begin(), serial.end(), ' ');
            serial.erase(end_pos, serial.end());

            if (serial.length() > 0)
                components.append(serial);
        }
    }

    MD5 M;
    M.add(components.c_str(), components.size());
    Hardware::user_guid = M.getHash();
    return (!Hardware::user_guid.empty());
}