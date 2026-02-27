//本代码使用WindowsAPI
//其他还有使用第三方库，如libcurl
//使用Qt网络库
//使用POCO库
#include<iostream>
#include<string>
#include<Windows.h>
#include<wininet.h>
#pragma comment(lib,"wininet.lib")
bool CheckForUpdateWithWinInet(const std::wstring& server, const std::wstring& username, const std::wstring& password, const std::wstring remoteFilePath, const std::wstring& localFilePath)
{
	HINTERNET hInternet = NULL;
	HINTERNET hFtpSession = NULL;
	FILETIME remoteFt, localFt;
	bool bUpdateAvailable = false;
	//初始化WinInet
	hInternet = InternetOpen(L"SoftwareUpdateChecker", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (!hInternet)
	{
		std::cerr << "InternetOpen failed: " << GetLastError() << std::endl;
		return false;
	}
	//连接到FTP服务器
	hFtpSession = InternetConnect(hInternet, server.c_str(), INTERNET_DEFAULT_FTP_PORT, username.empty() ? NULL : username.c_str(), password.empty() ? NULL : password.c_str(), INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
	if (!hFtpSession)
	{
		std::cerr << "InternetConnet failed: " << GetLastError() << std::endl;
		InternetCloseHandle(hInternet);
		return false;
	}
	//获取远程文件的属性
	WIN32_FIND_DATA FindFileData;
	HINTERNET hFind = FtpFindFirstFile(hFtpSession, remoteFilePath.c_str(), &FindFileData, INTERNET_FLAG_RELOAD, 0);
	if (hFind != NULL)
	{
		HANDLE hLocalFile = CreateFile(localFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hLocalFile != INVALID_HANDLE_VALUE)
		{
			if (GetFileTime(hLocalFile, NULL, NULL, &localFt))
			{
				if (CompareFileTime(&FindFileData.ftLastWriteTime, &localFt) > 0)
				{
					std::cout << "update available" << std::endl;
					bUpdateAvailable = true;
				}
				else
				{
					std::cout << "local file is up to date" << std::endl;
				}
			}
			CloseHandle(hLocalFile);
		}
		InternetCloseHandle(hFind);
	}
	else
	{
		std::cerr << "Failed to get remote file info: " << GetLastError() << std::endl;
	}
	if (hFtpSession)
		InternetCloseHandle(hFtpSession);
	if (hInternet)
		InternetCloseHandle(hInternet);
	return bUpdateAvailable;
}
int main()
{
	std::wstring server = L"test.com";
	std::wstring username = L"username";
	std::wstring password = L"password";
	std::wstring remotePath = L"path.zip";
	std::wstring localPath = L"path.zip";

	std::cout << "Checking for update via FTP (WinInet)..." << std::endl;

	if (CheckForUpdateWithWinInet(server, username, password, remotePath, localPath)) {
		std::cout << "Proceed to download the update..." << std::endl;
	}

	return 0;
}