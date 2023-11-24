#include "cleo_check.h"

template<class C, typename T>
bool contains(C&& c, T e) { return find(begin(c), end(c), e) != end(c); };

bool GameCleo::checkCleo()
{
	Log::write(XorStr("GameCleo::checkCleo() started"));
	char patht[MAX_PATH];
	char pathtt[MAX_PATH];
	std::string line;

	std::string url;

	std::ifstream gameListFile;
	int allGameFileCount = 0;
	std::string allGameFiles[1000] = {};

	std::ifstream hashListFile;
	int HashFilesCount = 0;
	std::string HashFilesList[1000] = {};

	// ��������� ������ � �������� �������.
	sprintf(patht, "%s", std::tmpnam(nullptr));

	url = Config::URL_REPOSITORY + XorStr("/additional/cleo/file.lists");
	if (!Network::file_sendRequest(url.c_str(), patht))
	{
		Game::TerminateGame(1, XorStr("0x1000001"));
		return false;
	}
	if (std::filesystem::is_empty(patht))
	{
		Game::TerminateGame(1, XorStr("0x1000002"));
		return false;
	}

	sprintf(pathtt, "%s", std::tmpnam(nullptr));
	url = Config::URL_REPOSITORY + XorStr("/additional/cleo/hash.lists");
	if (!Network::file_sendRequest(url.c_str(), pathtt))
	{
		Game::TerminateGame(1, XorStr("0x1000003"));
		return false;
	}
	if (std::filesystem::is_empty(pathtt))
	{
		Game::TerminateGame(1, XorStr("0x1000004"));
		return false;
	}

	/*
	* ������ ������ ������ ������ � ����������� ������.
	*/

	std::string newpathh = Core::getCurrentDirectory();
	std::replace(newpathh.begin(), newpathh.end(), '\\', '/');

	gameListFile.open(patht);
	if (gameListFile.is_open())
	{
		while (!gameListFile.eof())
		{
			while (std::getline(gameListFile, line))
			{
				// ���������� �������� ����� � ���� ������� ���� � ����������� ������.
				sprintf(patht, "%s/%s", newpathh.c_str(), line.c_str());

				allGameFiles[allGameFileCount] = patht;
				// �������������� ���������� � �������� ������������ �������.
				allGameFileCount++;
			}
		}
	}
	gameListFile.close();

	hashListFile.open(pathtt);
	if (hashListFile.is_open())
	{
		while (!hashListFile.eof())
		{
			while (std::getline(hashListFile, line))
			{
				// ���������� �������� ����� � ���� ������� ���� � ����������� ������.
				sprintf(pathtt, "%s", line.c_str());

				HashFilesList[HashFilesCount] = pathtt;
				// �������������� ���������� � �������� ������������ �������.
				HashFilesCount++;
			}
		}
	}
	hashListFile.close();

	std::vector<std::string> remainingFiles;

	/*
	* �������� ����� cleo
	*/
	char scriptsPath[MAX_PATH];
	sprintf(scriptsPath, "%s\\cleo", Core::getCurrentDirectory());

	for (auto& file : std::filesystem::recursive_directory_iterator(scriptsPath))
	{
		std::string newpath = file.path().string();
		std::replace(newpath.begin(), newpath.end(), '\\', '/');

		// ���� �������� �����������
		if (!std::filesystem::is_directory(file))
		{
			MD5 M;
			std::ifstream filez;
			std::istream* input = NULL;
			const size_t BufferSize = 65536;
			char* buffer = new char[BufferSize];

			filez.open(file.path().string().c_str(), std::ios::in | std::ios::binary);
			input = &filez;
			// process file
			while (*input)
			{
				input->read(buffer, BufferSize);
				std::size_t numBytesRead = size_t(input->gcount());
				M.add(buffer, numBytesRead);
			}
			// clean up
			filez.close();
			delete[] buffer;

			if (file.path().filename().string() == XorStr("CamHunt.cfg"))
			{
				int n = sizeof(allGameFiles) / sizeof(allGameFiles[0]);
				auto itr = find(allGameFiles, allGameFiles + n, newpath);

				if (itr != end(allGameFiles))
				{
					allGameFiles[distance(allGameFiles, itr)] = "";
					if (allGameFileCount > 0) allGameFileCount--;
				}
			}
			else if (!std::filesystem::is_empty(file) && !contains(allGameFiles, newpath))
			{
				// report
				Report::CleoInfo info;
				info.filename = file.path().filename().string();
				info.path = file.path().string();
				info.hash = M.getHash();
				info.lastChange = Core::getLastWriteTime(std::filesystem::last_write_time(file.path()));
				info.size = std::to_string(std::filesystem::file_size(file.path()));
				info.permissions = Core::getFilePermissions(std::filesystem::status(file.path()).permissions());
				Report::sendCleoReport(&info);

				remainingFiles.push_back(file.path().filename().string());
			}
			else
			{
				if (contains(allGameFiles, newpath))
				{
					int n = sizeof(allGameFiles) / sizeof(allGameFiles[0]);

					// ����� ����� � ����������� �������.
					auto itr = find(allGameFiles, allGameFiles + n, newpath);

					// ���� ������. 
					if (itr != end(allGameFiles))
					{
						int nn = sizeof(HashFilesList) / sizeof(HashFilesList[0]);
						auto itrr = find(HashFilesList, HashFilesList + nn, M.getHash());

						if (itrr != end(HashFilesList))
						{
							int n = sizeof(allGameFiles) / sizeof(allGameFiles[0]);
							auto itr = find(allGameFiles, allGameFiles + n, newpath);

							if (itr != end(allGameFiles))
							{
								allGameFiles[distance(allGameFiles, itr)] = "";
								if (allGameFileCount > 0) allGameFileCount--;
							}
							else
							{
								// report
								Report::CleoInfo info;
								info.filename = file.path().filename().string();
								info.path = file.path().string();
								info.hash = M.getHash();
								info.lastChange = Core::getLastWriteTime(std::filesystem::last_write_time(file.path()));
								info.size = std::to_string(std::filesystem::file_size(file.path()));
								info.permissions = Core::getFilePermissions(std::filesystem::status(file.path()).permissions());
								Report::sendCleoReport(&info);

								remainingFiles.push_back(file.path().filename().string());
							}
						}
						else
						{
							// report
							Report::CleoInfo info;
							info.filename = file.path().filename().string();
							info.path = file.path().string();
							info.hash = M.getHash();
							info.lastChange = Core::getLastWriteTime(std::filesystem::last_write_time(file.path()));
							info.size = std::to_string(std::filesystem::file_size(file.path()));
							info.permissions = Core::getFilePermissions(std::filesystem::status(file.path()).permissions());
							Report::sendCleoReport(&info);

							remainingFiles.push_back(file.path().filename().string());
						}
					}
				}
				else
				{
					// report
					Report::CleoInfo info;
					info.filename = file.path().filename().string();
					info.path = file.path().string();
					info.hash = M.getHash();
					info.lastChange = Core::getLastWriteTime(std::filesystem::last_write_time(file.path()));
					info.size = std::to_string(std::filesystem::file_size(file.path()));
					info.permissions = Core::getFilePermissions(std::filesystem::status(file.path()).permissions());
					Report::sendCleoReport(&info);

					remainingFiles.push_back(file.path().filename().string());
				}
			}
		}
	}

	if (remainingFiles.empty() && allGameFileCount == 0) {
		Log::write(XorStr("GameCleo::checkCleo() out 0 .cs files"));
		return true;
	}
	else
	{
		Log::write(XorStr("GameCleo::checkCleo() out %d .cs files"), allGameFileCount);
		if (allGameFileCount != 0 && !(remainingFiles.empty())) return false;
		else return true;
	}
	return false;
}