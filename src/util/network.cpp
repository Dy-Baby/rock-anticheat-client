#include "network.h"

long Network::lastError = 0;

DWORD Network::init()
{
    HMODULE library = LI_FN(LoadLibraryA)(XorStr("libcurl.dll"));
    if (!library)
    {
        library = GetModuleHandleA(XorStr("libcurl.dll"));
        if (library == NULL)
        {
            return GetLastError();
        }
        else
        {
            LI_FN(curl_global_init).get()(CURL_GLOBAL_DEFAULT);
        }
    }
    return 0;
}

void Network::setDefaultCURLOptions(CURL* curl)
{
    LI_FN(curl_easy_setopt).safe()(curl, CURLOPT_FOLLOWLOCATION, 1L);
    LI_FN(curl_easy_setopt).safe()(curl, CURLOPT_TIMEOUT, 0L);
    LI_FN(curl_easy_setopt).safe()(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
    LI_FN(curl_easy_setopt).safe()(curl, CURLOPT_USERAGENT, XorStr("STALKER/1.0"));
    LI_FN(curl_easy_setopt).safe()(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    LI_FN(curl_easy_setopt).safe()(curl, CURLOPT_NOSIGNAL, 1);
}

bool Network::file_sendRequest(const char* url, const char* out)
{
    FILE* fp;
    long httpStatusCode = 0;

    CURL* curl;

    // ������������� CURL.
    LI_FN(curl_global_init).get()(CURL_GLOBAL_DEFAULT);
    curl = LI_FN(curl_easy_init).get()();

    // ���� CURL ����� � ������.
    if (curl)
    {
        setDefaultCURLOptions(curl);

        // ��������� ���� ������.
        fp = fopen(out, XorStr("wb"));

        LI_FN(curl_easy_setopt).get()(curl, CURLOPT_URL, url);
        LI_FN(curl_easy_setopt).get()(curl, CURLOPT_WRITEFUNCTION, Core::curlWriteToFile);
        LI_FN(curl_easy_setopt).get()(curl, CURLOPT_WRITEDATA, fp);

        CURLcode result = LI_FN(curl_easy_perform).get()(curl);
        LI_FN(curl_easy_getinfo).get()(curl, CURLINFO_RESPONSE_CODE, &httpStatusCode);

        if (result == CURLE_OK && httpStatusCode == 200)
        {
            // ��������� ������� � �������� �����.
            LI_FN(curl_easy_cleanup).get()(curl);
            fclose(fp);

            return true;
        }
        else
        {
            Network::lastError = httpStatusCode;

            Log::write(XorStr("[ERROR] Failed to perform request file_sendRequest() httpStatusCode: %ld"), httpStatusCode);

            LI_FN(curl_easy_cleanup).get()(curl);
            fclose(fp);
            return false;
        }
    }
    LI_FN(curl_easy_cleanup).get()(curl);
    return false;
}
long Network::void_sendRequest(const char* url)
{
    long httpStatusCode = 0;

    CURL* curl;
    LI_FN(curl_global_init).get()(CURL_GLOBAL_DEFAULT);
    curl = LI_FN(curl_easy_init).get()();
    
    if (curl)
    {
        setDefaultCURLOptions(curl);

        // encrypt request
        std::string surl(url);
        std::string params(url);
        size_t pos = surl.find("?");
        if (pos++ != surl.npos)
        {
            params = surl.substr(pos);

            surl.erase(pos-1, params.length()+1);
        }

        mine::AES aesManager;
        const auto p1 = std::chrono::system_clock::now();
        params = params + "&time=" + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count());
        params = aesManager.encrypt(params, Config::AES_KEY, Config::AES_IV, mine::MineCommon::Encoding::Raw, mine::MineCommon::Encoding::Base64);

        std::string sdata = ("data=" + params);

        LI_FN(curl_easy_setopt).get()(curl, CURLOPT_URL, surl.c_str());
        LI_FN(curl_easy_setopt).get()(curl, CURLOPT_WRITEFUNCTION, Core::curlNullWrite);
        LI_FN(curl_easy_setopt).get()(curl, CURLOPT_WRITEDATA, NULL);
        LI_FN(curl_easy_setopt).get()(curl, CURLOPT_POST, 1L);
        LI_FN(curl_easy_setopt).get()(curl, CURLOPT_POSTFIELDS, sdata);

        CURLcode result = LI_FN(curl_easy_perform).get()(curl);
        LI_FN(curl_easy_getinfo).get()(curl, CURLINFO_RESPONSE_CODE, &httpStatusCode);
        Network::lastError = httpStatusCode;

        if (result == CURLE_OK && httpStatusCode != 200)
        {
            Log::write(XorStr("[ERROR] Failed to perform request void_sendRequest(%s) httpStatusCode: %ld"), surl.c_str(), httpStatusCode);
        }
    }
    LI_FN(curl_easy_cleanup).get()(curl);

    return httpStatusCode;
}
int Network::int_sendRequest(const char* url)
{
    long httpStatusCode = 0;

    CURL* curl;
    LI_FN(curl_global_init).get()(CURL_GLOBAL_DEFAULT);
    curl = LI_FN(curl_easy_init).get()();

    if (curl)
    {
        std::string _resp;

        setDefaultCURLOptions(curl);

        LI_FN(curl_easy_setopt).get()(curl, CURLOPT_URL, url);
        LI_FN(curl_easy_setopt).get()(curl, CURLOPT_WRITEFUNCTION, Core::curlWriteToString);
        LI_FN(curl_easy_setopt).get()(curl, CURLOPT_WRITEDATA, &_resp);

        CURLcode result = LI_FN(curl_easy_perform).get()(curl);
        LI_FN(curl_easy_getinfo).get()(curl, CURLINFO_RESPONSE_CODE, &httpStatusCode);

        if (result == CURLE_OK && httpStatusCode == 200)
        {
            LI_FN(curl_easy_cleanup).get()(curl);
            return atoi(_resp.c_str());
        }
        else
        {
            Network::lastError = httpStatusCode;
            LI_FN(curl_easy_cleanup).get()(curl);

            Log::write(XorStr("[ERROR] Failed to perform request int_sendRequest(%s) httpStatusCode: %ld"), url, httpStatusCode);
            return 9999;
        }
    }
    LI_FN(curl_easy_cleanup).get()(curl);

	return 9999;
}
bool Network::bool_sendRequest(const char* url, bool out)
{
    long httpStatusCode = 0;

    CURL* curl;

    LI_FN(curl_global_init).get()(CURL_GLOBAL_DEFAULT);
    curl = LI_FN(curl_easy_init).get()();

    if (curl)
    {
        std::string _resp;

        setDefaultCURLOptions(curl);

        LI_FN(curl_easy_setopt).get()(curl, CURLOPT_URL, url);
        LI_FN(curl_easy_setopt).get()(curl, CURLOPT_WRITEFUNCTION, Core::curlWriteToString);
        LI_FN(curl_easy_setopt).get()(curl, CURLOPT_WRITEDATA, &_resp);

        CURLcode result = LI_FN(curl_easy_perform).get()(curl);
        LI_FN(curl_easy_getinfo).get()(curl, CURLINFO_RESPONSE_CODE, &httpStatusCode);

        if (result == CURLE_OK && httpStatusCode == 200)
        {
            // Success
            LI_FN(curl_easy_cleanup).get()(curl);
            out = _resp != XorStr("0");
            return true;
        }
        else
        {
            Network::lastError = httpStatusCode;

            Log::write(XorStr("[ERROR] Failed to perform request bool_sendRequest(%s) httpStatusCode: %ld"), url, httpStatusCode);
            LI_FN(curl_easy_cleanup).get()(curl);
            return false;
        }
    }
    LI_FN(curl_easy_cleanup).get()(curl);

	return false;
}
std::string Network::string_sendRequest(const char* url)
{
    long httpStatusCode = 0;

    CURL* curl;
    LI_FN(curl_global_init).get()(CURL_GLOBAL_DEFAULT);
    curl = LI_FN(curl_easy_init).get()();

    if (curl)
    {
        std::string out;

        setDefaultCURLOptions(curl);

        LI_FN(curl_easy_setopt).get()(curl, CURLOPT_URL, url);
        LI_FN(curl_easy_setopt).get()(curl, CURLOPT_WRITEFUNCTION, Core::curlWriteToString);
        LI_FN(curl_easy_setopt).get()(curl, CURLOPT_WRITEDATA, &out);

        CURLcode result = LI_FN(curl_easy_perform).get()(curl);
        LI_FN(curl_easy_getinfo).get()(curl, CURLINFO_RESPONSE_CODE, &httpStatusCode);

        Network::lastError = httpStatusCode;

        if (result == CURLE_OK && httpStatusCode == 200)
        {
            // Success
            LI_FN(curl_easy_cleanup).get()(curl);
            return out;
        }
        else if (result == CURLE_OK && httpStatusCode == 403)
        {
            LI_FN(curl_easy_cleanup).get()(curl);

            Log::write(XorStr("[ERROR] Failed to perform request string_sendRequest(%s) httpStatusCode: %ld"), url, httpStatusCode);
            return "SESNF";
        }
        else
        {
            LI_FN(curl_easy_cleanup).get()(curl);

            Log::write(XorStr("[ERROR] Failed to perform request string_sendRequest(%s) httpStatusCode: %ld"), url, httpStatusCode);
            return "FAILED";
        }
    }
    LI_FN(curl_easy_cleanup).get()(curl);

    return "FAILED";
}
bool Network::vectorstring_sendRequest(const char* url, std::vector<std::string> &out)
{
    long httpStatusCode = 0;

    CURL* curl;
    LI_FN(curl_global_init).get()(CURL_GLOBAL_DEFAULT);
    curl = LI_FN(curl_easy_init).get()();

    if (curl)
    {
        std::string respStr;

        setDefaultCURLOptions(curl);

        LI_FN(curl_easy_setopt).get()(curl, CURLOPT_URL, url);
        LI_FN(curl_easy_setopt).get()(curl, CURLOPT_WRITEFUNCTION, Core::curlWriteToString);
        LI_FN(curl_easy_setopt).get()(curl, CURLOPT_WRITEDATA, &respStr);
       
        CURLcode result = LI_FN(curl_easy_perform).get()(curl);
        LI_FN(curl_easy_getinfo).get()(curl, CURLINFO_RESPONSE_CODE, &httpStatusCode);

        if (result == CURLE_OK && httpStatusCode == 200)
        {
            // Success
            LI_FN(curl_easy_cleanup).get()(curl);

            std::string to;
            std::string params;

            mine::AES aesManager;
            params = aesManager.decrypt(respStr, Config::AES_KEY, Config::AES_IV, mine::MineCommon::Encoding::Base64, mine::MineCommon::Encoding::Raw);

            std::stringstream ss(params.c_str());

            while (getline(ss, to, '^'))
            {
                out.push_back(to);
            }

            return true;
        }
        else
        {
            Network::lastError = httpStatusCode;

            Log::write(XorStr("[ERROR] Failed to perform request vectorstring_sendRequest(%s) httpStatusCode: %ld"), url, httpStatusCode);
            LI_FN(curl_easy_cleanup).get()(curl);
            return false;
        }
    }
    LI_FN(curl_easy_cleanup).get()(curl);

	return false;
}