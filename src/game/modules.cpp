#include "modules.h"

std::vector<std::string> allowedPublisher;
std::vector<std::string> allowedHash;
std::vector<std::string> allowedModules;

std::string Modules::getPublisher(LPCSTR path)
{
	WCHAR szFileName[MAX_PATH];
	mbstowcs(szFileName, path, MAX_PATH);

	HCERTSTORE hStore = NULL;
	HCRYPTMSG hMsg = NULL;
	PCCERT_CONTEXT pCertContext = NULL;
	BOOL fResult;
	DWORD dwEncoding, dwContentType, dwFormatType;
	PCMSG_SIGNER_INFO pSignerInfo = NULL;
	PCMSG_SIGNER_INFO pCounterSignerInfo = NULL;
	DWORD dwSignerInfo;
	Core::SPROG_PUBLISHERINFO ProgPubInfo;

	fResult = LI_FN(CryptQueryObject).get()(CERT_QUERY_OBJECT_FILE,
		szFileName,
		CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
		CERT_QUERY_FORMAT_FLAG_BINARY,
		0,
		&dwEncoding,
		&dwContentType,
		&dwFormatType,
		&hStore,
		&hMsg,
		NULL);
	if (!fResult) return XorStr("NONE");

	// Get signer information size.
	fResult = LI_FN(CryptMsgGetParam).get()(hMsg, CMSG_SIGNER_INFO_PARAM, 0, NULL, &dwSignerInfo);
	if (!fResult) return XorStr("NONE");

	// Allocate memory for signer information.
	pSignerInfo = (PCMSG_SIGNER_INFO)LI_FN(LocalAlloc).get()(LPTR, dwSignerInfo);
	if (!pSignerInfo) return XorStr("NONE");

	// Get Signer Information.
	fResult = LI_FN(CryptMsgGetParam).get()(hMsg, CMSG_SIGNER_INFO_PARAM, 0, (PVOID)pSignerInfo, &dwSignerInfo);
	if (!fResult) return XorStr("NONE");

	if (Core::GetProgAndPublisherInfo(pSignerInfo, &ProgPubInfo) == ERROR_SUCCESS)
	{
		if (ProgPubInfo.lpszProgramName != NULL)
		{
			std::wstring wideprogramName(ProgPubInfo.lpszProgramName);
			std::string programName = std::string(wideprogramName.begin(), wideprogramName.end());
			delete& wideprogramName;

			return programName;
		}
	}

	return XorStr("NONE");
}

bool Modules::verifyPublisher(LPCSTR path)
{
	WCHAR szFileName[MAX_PATH];
	mbstowcs(szFileName, path, MAX_PATH);

	HCERTSTORE hStore = NULL;
	HCRYPTMSG hMsg = NULL;
	PCCERT_CONTEXT pCertContext = NULL;
	BOOL fResult;
	DWORD dwEncoding, dwContentType, dwFormatType;
	PCMSG_SIGNER_INFO pSignerInfo = NULL;
	PCMSG_SIGNER_INFO pCounterSignerInfo = NULL;
	DWORD dwSignerInfo;
	Core::SPROG_PUBLISHERINFO ProgPubInfo;

	fResult = LI_FN(CryptQueryObject).get()(CERT_QUERY_OBJECT_FILE,
		szFileName,
		CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
		CERT_QUERY_FORMAT_FLAG_BINARY,
		0,
		&dwEncoding,
		&dwContentType,
		&dwFormatType,
		&hStore,
		&hMsg,
		NULL);
	if (!fResult) return false;

	// Get signer information size.
	fResult = LI_FN(CryptMsgGetParam).get()(hMsg, CMSG_SIGNER_INFO_PARAM, 0, NULL, &dwSignerInfo);
	if (!fResult) return false;

	// Allocate memory for signer information.
	pSignerInfo = (PCMSG_SIGNER_INFO)LI_FN(LocalAlloc).get()(LPTR, dwSignerInfo);
	if (!pSignerInfo) return false;

	// Get Signer Information.
	fResult = LI_FN(CryptMsgGetParam).get()(hMsg, CMSG_SIGNER_INFO_PARAM, 0, (PVOID)pSignerInfo, &dwSignerInfo);
	if (!fResult) return false;

	if (Core::GetProgAndPublisherInfo(pSignerInfo, &ProgPubInfo) == ERROR_SUCCESS)
	{
		if (ProgPubInfo.lpszProgramName != NULL)
		{
			std::wstring wideprogramName(ProgPubInfo.lpszProgramName);
			std::string programName = std::string(wideprogramName.begin(), wideprogramName.end());
			delete &wideprogramName;

			// ������� ����� �������� � ������ �����������
			std::vector<std::string>::iterator it = std::find(allowedPublisher.begin(), allowedPublisher.end(), programName);
			if (it == allowedPublisher.end())
			{
				return false;
			}
		}
	}

	return true;
}

long Modules::verifyMicrosoftSignature(LPCSTR path)
{
	USES_CONVERSION;
	LPCWSTR pwszSourceFile = A2W(path); //We convert the char* to wchar*
	LONG lStatus;
	GUID WintrustVerifyGuid = WINTRUST_ACTION_GENERIC_VERIFY_V2;
	GUID DriverActionGuid = DRIVER_ACTION_VERIFY;
	HANDLE hFile;
	DWORD dwHash;
	BYTE bHash[100];
	HCATINFO hCatInfo;
	HCATADMIN hCatAdmin;

	WINTRUST_DATA wd = { 0 };
	WINTRUST_FILE_INFO wfi = { 0 };
	WINTRUST_CATALOG_INFO wci = { 0 };

	////set up structs to verify files with cert signatures
	wfi.cbStruct = sizeof(WINTRUST_FILE_INFO);
	wfi.pcwszFilePath = pwszSourceFile;
	wfi.hFile = NULL;
	wfi.pgKnownSubject = NULL;

	wd.cbStruct = sizeof(WINTRUST_DATA);
	wd.pPolicyCallbackData = NULL;
	wd.pSIPClientData = NULL;
	wd.dwUIChoice = WTD_UI_NONE;
	wd.fdwRevocationChecks = WTD_REVOKE_NONE;
	wd.dwUnionChoice = WTD_CHOICE_FILE;
	wd.pFile = &wfi;
	wd.dwStateAction = WTD_STATEACTION_VERIFY;
	wd.hWVTStateData = NULL;
	wd.pwszURLReference = NULL;
	wd.dwProvFlags |= WTD_CACHE_ONLY_URL_RETRIEVAL;
	wd.dwUIContext = 0;
	wd.pSignatureSettings = 0;


	lStatus = LI_FN(WinVerifyTrust).get()((HWND)INVALID_HANDLE_VALUE, &WintrustVerifyGuid, &wd);
	wd.dwStateAction = WTD_STATEACTION_CLOSE;
	LI_FN(WinVerifyTrust).get()((HWND)INVALID_HANDLE_VALUE, &WintrustVerifyGuid, &wd); //close hWVTStateData

	////if failed, try to verify using catalog files
	if (lStatus != ERROR_SUCCESS)
	{
		//open the file
		hFile = LI_FN(CreateFileW).get()(pwszSourceFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			Log::write(XorStr("[ERROR] failed to CreateFileW"));
			Game::TerminateGame(1, XorStr("0x1000017"));
			return false;
		}

		dwHash = sizeof(bHash);
		if (!LI_FN(CryptCATAdminCalcHashFromFileHandle).get()(hFile, &dwHash, bHash, 0))
		{
			return false;
		}

		//Create a string form of the hash (used later in pszMemberTag)
		LPWSTR pszMemberTag = new WCHAR[dwHash * 2 + 1];
		for (DWORD dw = 0; dw < dwHash; ++dw)
		{
			wsprintfW(&pszMemberTag[dw * 2], XorStrW(L"%02X"), bHash[dw]);
		}

		if (!LI_FN(CryptCATAdminAcquireContext).get()(&hCatAdmin, &DriverActionGuid, 0))
		{
			return false;
		}

		//find the catalog which contains the hash
		hCatInfo = LI_FN(CryptCATAdminEnumCatalogFromHash).get()(hCatAdmin, bHash, dwHash, 0, NULL);

		if (hCatInfo)
		{
			CATALOG_INFO ci = { 0 };
			LI_FN(CryptCATCatalogInfoFromContext).get()(hCatInfo, &ci, 0);

			memset(&wci, 0, sizeof(wci));
			wci.cbStruct = sizeof(WINTRUST_CATALOG_INFO);
			wci.pcwszCatalogFilePath = ci.wszCatalogFile;
			wci.pcwszMemberFilePath = pwszSourceFile;
			wci.hMemberFile = hFile;
			wci.pcwszMemberTag = pszMemberTag;
			wci.pbCalculatedFileHash = bHash;
			wci.cbCalculatedFileHash = dwHash;
			wci.hCatAdmin = hCatAdmin;

			memset(&wd, 0, sizeof(wd));
			wd.cbStruct = sizeof(WINTRUST_DATA);
			wd.pPolicyCallbackData = NULL;
			wd.pSIPClientData = NULL;
			wd.dwUIChoice = WTD_UI_NONE;
			wd.fdwRevocationChecks = WTD_REVOKE_NONE;
			wd.dwUnionChoice = WTD_CHOICE_CATALOG;
			wd.pCatalog = &wci;
			wd.dwStateAction = WTD_STATEACTION_VERIFY;
			wd.hWVTStateData = NULL;
			wd.pwszURLReference = NULL;
			wd.dwProvFlags |= WTD_CACHE_ONLY_URL_RETRIEVAL;
			wd.dwUIContext = 0;
			wd.pSignatureSettings = 0;

			lStatus = LI_FN(WinVerifyTrust).get()((HWND)INVALID_HANDLE_VALUE, &WintrustVerifyGuid, &wd);
			wd.dwStateAction = WTD_STATEACTION_CLOSE;
			LI_FN(WinVerifyTrust).get()((HWND)INVALID_HANDLE_VALUE, &WintrustVerifyGuid, &wd); //close hWVTStateData
			LI_FN(CryptCATAdminReleaseCatalogContext).get()(hCatAdmin, hCatInfo, 0);
		}


		LI_FN(CryptCATAdminReleaseContext).get()(hCatAdmin, 0);
		delete[] pszMemberTag;

		LI_FN(CloseHandle).get()(hFile);
	}

	return (lStatus == ERROR_SUCCESS);
}

bool Modules::inspectModules(DWORD pID)
{
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;
	unsigned int i;

	hProcess = LI_FN(OpenProcess).get()(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, pID);

	if (LI_FN(K32EnumProcessModules).get()(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];

			if (LI_FN(K32GetModuleBaseNameW).get()(hProcess, hMods[i], szModName,
				sizeof(szModName) / sizeof(TCHAR)))
			{
				TCHAR path[MAX_PATH];
				if (LI_FN(GetModuleFileNameW).safe()(hMods[i], path, MAX_PATH) != 0)
				{
					// convert to string
					std::wstring wszModName(szModName);
					std::string injectedmodule(wszModName.begin(), wszModName.end());

					//if (injectedmodule != XorStr("gta_sa.exe") && injectedmodule != XorStr("rockAC.dll"))
					//{
						// ��������� ����� � ������ ����������� �������
						std::vector<std::string>::iterator it = std::find(allowedModules.begin(), allowedModules.end(), injectedmodule);
						if (it == allowedModules.end())
						{
							// ������� ��������� ������� ����������� Windows
							if (!verifyMicrosoftSignature(Core::UTF16ToUTF8(path).c_str()))
							{
								// ������� ��������� ����� ���������� ���������
								if (!verifyPublisher(Core::UTF16ToUTF8(path).c_str()))
								{
									// get base address of module
									std::ostringstream baseAddress;
									baseAddress << (DWORD)LI_FN(GetModuleHandleW).safe()(szModName);

									std::string newpath = Core::UTF16ToUTF8(path);
									std::replace(newpath.begin(), newpath.end(), '\\', '/');

									Report::ModuleInfo info;
									info.name = injectedmodule;
									info.path = newpath;
									info.hash = Core::getFileMD5(Core::UTF16ToUTF8(path));
									info.lastChange = Core::getLastWriteTime(std::filesystem::last_write_time(Core::UTF16ToUTF8(path)));
									info.permission = Core::getFilePermissions(std::filesystem::status(Core::UTF16ToUTF8(path)).permissions());
									info.base_address = baseAddress.str();
									info.publisher = getPublisher(Core::UTF16ToUTF8(path).c_str());
									info.size = std::to_string(std::filesystem::file_size(Core::UTF16ToUTF8(path)));

									Report::sendModuleReport(&info);
								}
							}
						}
						else
						{
							// �������, ������ ��� �����
							MD5 M;
							std::ifstream mFile;
							std::istream* input = NULL;
							const size_t BufferSize = 65536;
							char* buffer = new char[BufferSize];

							mFile.open(Core::UTF16ToUTF8(path).c_str(), std::ios::in | std::ios::binary);
							input = &mFile;

							// process file
							while (*input)
							{
								input->read(buffer, BufferSize);
								std::size_t numBytesRead = size_t(input->gcount());
								M.add(buffer, numBytesRead);
							}

							std::vector<std::string>::iterator it = std::find(allowedHash.begin(), allowedHash.end(), M.getHash());
							if (it == allowedHash.end())
							{
								// get base address of module
								std::ostringstream baseAddress;
								baseAddress << (DWORD)LI_FN(GetModuleHandleW).get()(szModName);

								std::string newpath = Core::UTF16ToUTF8(path);
								std::replace(newpath.begin(), newpath.end(), '\\', '/');

								Report::ModuleInfo info;
								info.name = injectedmodule;
								info.path = newpath;
								info.hash = Core::getFileMD5(Core::UTF16ToUTF8(path));
								info.lastChange = Core::getLastWriteTime(std::filesystem::last_write_time(Core::UTF16ToUTF8(path)));
								info.permission = Core::getFilePermissions(std::filesystem::status(Core::UTF16ToUTF8(path)).permissions());
								info.base_address = baseAddress.str();
								info.publisher = getPublisher(Core::UTF16ToUTF8(path).c_str());
								info.size = std::to_string(std::filesystem::file_size(Core::UTF16ToUTF8(path)));

								Report::sendModuleReport(&info);
							}


							// clean up
							mFile.close();
							delete[] buffer;
						}
					//}
				}
			}
		}
	}

	LI_FN(CloseHandle).get()(hProcess);

	return false;
}

void Modules::thread()
{
	Log::write(XorStr("Modules::thread() started"));
	Network::vectorstring_sendRequest((Config::URL_REPOSITORY + XorStr("/additional/modules/whitelist_publisher.php")).c_str(), allowedPublisher);
	Network::vectorstring_sendRequest((Config::URL_REPOSITORY + XorStr("/additional/modules/whitelist.php")).c_str(), allowedModules);
	Network::vectorstring_sendRequest((Config::URL_REPOSITORY + XorStr("/additional/modules/whitelist_hash.php")).c_str(), allowedHash);
	
	if (allowedPublisher.empty())
	{
		Log::write(XorStr("[ERROR] Modules::allowedPublisher empty"));
		Game::TerminateGame(1, XorStr("0x1000018"));
	}
	if (allowedModules.empty())
	{
		Log::write(XorStr("[ERROR] Modules::allowedModules empty"));
		Game::TerminateGame(1, XorStr("0x1000019"));
	}
	if (allowedHash.empty())
	{
		Log::write(XorStr("[ERROR] Modules::allowedHash empty"));
		Game::TerminateGame(1, XorStr("0x1000020"));
	}

	DWORD pid = Core::getProcessID(XorStrW(L"gta_sa.exe"));
	do
	{
		Modules::inspectModules(pid);
		LI_FN(Sleep).get()(Config::MODULES_CHECK_SLEEP);
	} while (pid != 0);
}