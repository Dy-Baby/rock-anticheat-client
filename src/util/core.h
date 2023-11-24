#pragma once

#include "../../includes.h"

class Core
{
public:
	typedef struct {
		LPWSTR lpszProgramName;
	} SPROG_PUBLISHERINFO, * PSPROG_PUBLISHERINFO;

	/// <summary>
	/// ������� ��� CURL, ��� ��������� NULL-������
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="(size_t size)">������ ������</param>
	/// <param name="(size_t nmemb)">������ ���������� ������</param>
	/// <returns>
	/// 	���������� NULL
	/// </returns>
	static size_t curlNullWrite(void* buffer, size_t size, size_t nmemb, void* userp);

	/// <summary>
	/// ������� ��� CURL, ��� ��������� ������ � ������ � ����
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="(void* ptr)">���������</param>
	/// <param name="(size_t size)">������ ������</param>
	/// <param name="(size_t nmemb)">������ ���������� ������</param>
	/// <param name="(FILE* stream)">����-����� ��� ������ (�������� ����� fopen)</param>
	/// <returns>
	/// 	���������� ���������� ����
	/// </returns>
	static size_t curlWriteToFile(void* ptr, size_t size, size_t nmemb, FILE* stream);

	/// <summary>
	/// ������� ��� CURL, ��� ��������� ������ � ���� String
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="(void* contents)">���������� ������</param>
	/// <param name="(size_t size)">������ ������</param>
	/// <param name="(size_t nmemb)">������ ���������� ������</param>
	/// <param name="(void* userp)">��������� �� ������</param>
	/// <returns>
	/// 	���������� NULL
	/// </returns>
	static size_t curlWriteToString(void* contents, size_t size, size_t nmemb, void* userp);

	/// <summary>
	/// ����������� �� wstring � string
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="(const std::wstring str)">wide-������</param>
	/// <returns>
	/// 	���������� ������
	/// </returns>
	static std::string UTF16ToUTF8(const std::wstring str);

	/// <summary>
	/// ����������� �� string � wstring
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="(const std::string str)">������</param>
	/// <returns>
	/// 	���������� wide-������
	/// </returns>
	static std::wstring UTF8ToUTF16(const std::string str);

	/// <summary>
	/// ��������� �������� ������� DLL
	/// </summary>
	/// <returns>
	/// 	���������� HMODULE
	/// </returns>
	static HMODULE GetCurrentModule();

	/// <summary>
	/// ��������� PID �������� � �������
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="(const std::wstring processName)">��� ��������</param>
	/// <returns>
	/// 	���������� pid � ���� DWORD
	/// </returns>
	static DWORD getProcessID(const std::wstring& processName);

	/// <summary>
	/// ��������� �������� ����������� �� ���������� � �����������
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="(PCMSG_SIGNER_INFO pSignerInfo)">SignerInfo</param>
	/// <param name="(PSPROG_PUBLISHERINFO Info)">Info</param>
	/// <returns>
	/// 	���������� �������� � ���� DWORD
	/// </returns>
	static DWORD GetProgAndPublisherInfo(PCMSG_SIGNER_INFO pSignerInfo, Core::PSPROG_PUBLISHERINFO Info);

	/// <summary>
	/// ����������� LPCWSTR � LPWSTR 
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="(LPCWSTR inputString)">LPCWSTR-������</param>
	/// <returns>
	/// 	���������� LPWSTR
	/// </returns>
	static LPWSTR AllocateAndCopyWideString(LPCWSTR inputString);

	static std::string getCurrentDirectory();

	static std::string getWinVersion();

	static std::string getFileMD5(std::string path);

	static HWND getHWND(DWORD pid);

	static std::string getLastWriteTime(std::filesystem::file_time_type tp);

	static std::string getFilePermissions(std::filesystem::perms p);

	static bool comp(const std::string& lhs, const std::string& rhs);

	static std::string getUsername();

	static BOOL CALLBACK EnumWindowsProcMy(HWND hwnd, LPARAM lParam);

	static std::string HWNDToString(HWND inputA);

	std::string encryptAES(std::string content);

	static void loadDefaultLibs();
};