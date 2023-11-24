#pragma once

#include "../../includes.h"

class Network
{
private:
	/// <summary>
	/// ������������� ��������� ��������� CURL
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="CURL* curl">��������� �� ������������������ CURL</param>
	/// <returns>
	/// 	������ �� ����������
	/// </returns>
	static void setDefaultCURLOptions(CURL* curl);

public:
	static long lastError;
	/// <summary>
	/// ��������� ���� �� ��������� ��� ������ libCURL
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="const char* url">����� ��������.</param>
	/// <param name="const char* out">���� ������ �����.</param>
	/// <returns>
	/// 	<c>true</c> ���� http-��� 200; �����, <c>false</c>.
	/// </returns>
	static DWORD init();

	/// <summary>
	/// ��������� ���� �� ��������� ��� ������ libCURL
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="const char* url">����� ��������.</param>
	/// <param name="const char* out">���� ������ �����.</param>
	/// <returns>
	/// 	<c>true</c> ���� http-��� 200; �����, <c>false</c>.
	/// </returns>
	static bool file_sendRequest(const char* url, const char* out);

	/// <summary>
	/// ���������� HTTPS-������ ��� ������ libCURL
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="const char* url">URL-�����</param>
	/// <returns>
	/// 	<c>������ �� ����������</c>.
	/// </returns>
	static long void_sendRequest(const char* url);

	/// <summary>
	/// ���������� HTTPS-������ ��� ������ libCURL
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="const char* url">URL-�����</param>
	/// <returns>
	/// 	���������� ����� ������� � ���� <c>int</c>.
	/// </returns>
	static int int_sendRequest(const char* url);

	/// <summary>
	/// ���������� HTTPS-������ ��� ������ libCURL
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="const char* url">URL-�����</param>
	/// <returns>
	/// 	���������� ����� ������� � ���� <c>bool</c>.
	/// </returns>
	static bool bool_sendRequest(const char* url, bool out);

	/// <summary>
	/// ���������� HTTPS-������ ��� ������ libCURL
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="const char* url">URL-�����</param>
	/// <returns>
	/// 	���������� ����� ������� � ���� <c>string</c>.
	/// </returns>
	static std::string string_sendRequest(const char* url);

	/// <summary>
	/// ���������� HTTPS-������ ��� ������ libCURL
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="const char* url">URL-�����</param>
	/// <returns>
	/// 	���������� ����� ������� � ���� vector-�������, ���� <c>string</c>.
	/// </returns>
	static bool vectorstring_sendRequest(const char* url, std::vector<std::string> &out);
};