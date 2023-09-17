#include <iostream>
#include <windows.h>
#include <string>
#include <tchar.h>
#include <map>
#include	<vector>
#pragma comment(lib, "Advapi32.lib")



int main()
{
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Enum\\USB"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		DWORD i = 0;
		TCHAR subKeyName[1024];
		DWORD subKeyNameSize = sizeof(subKeyName) / sizeof(TCHAR);
		while (RegEnumKey(hKey, i++, subKeyName, subKeyNameSize) == ERROR_SUCCESS)
		{
			HKEY hSubKey;
			if (RegOpenKeyEx(hKey, subKeyName, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
			{
				DWORD j = 0;
				TCHAR subSubKeyName[1024];
				DWORD subSubKeyNameSize = sizeof(subSubKeyName) / sizeof(TCHAR);
				while (RegEnumKey(hSubKey, j++, subSubKeyName, subSubKeyNameSize) == ERROR_SUCCESS)
				{
					HKEY hSubSubKey;
					if (RegOpenKeyEx(hSubKey, subSubKeyName, 0, KEY_READ, &hSubSubKey) == ERROR_SUCCESS)
					{
						TCHAR subsubSubKeyName[1024];
						DWORD subsubSubKeyNameSize = sizeof(subsubSubKeyName) / sizeof(TCHAR);
						enum index { device_parameters, properties };
						if (RegEnumKey(hSubSubKey, device_parameters, subsubSubKeyName, subsubSubKeyNameSize) == ERROR_SUCCESS)
						{
							HKEY hSubSubSubKey;
							if (RegOpenKeyEx(hSubSubKey, subsubSubKeyName, 0, KEY_READ, &hSubSubSubKey) == ERROR_SUCCESS)
							{
								TCHAR portName[MAX_PATH];
								DWORD size = sizeof(portName);
								if (RegGetValueW(hSubSubSubKey, NULL, L"PortName", RRF_RT_REG_SZ, NULL, portName, &size) == ERROR_SUCCESS)
								{
									if (_tcsstr((wchar_t *)portName, L"COM") != NULL)
									{
										
										_tprintf(TEXT("%s\\%s\\%s\n"), subKeyName, subSubKeyName, portName);
										std::wstring deviceInstanceId(subKeyName);


										// VID와 PID 추출
										size_t vidPos = deviceInstanceId.find(L"VID_");
										size_t pidPos = deviceInstanceId.find(L"PID_");
										if (vidPos != std::wstring::npos && pidPos != std::wstring::npos) {
											DWORD vid = std::wcstoul(deviceInstanceId.c_str() + vidPos + 4, nullptr, 16);
											DWORD pid = std::wcstoul(deviceInstanceId.c_str() + pidPos + 4, nullptr, 16);

											std::wcout << L"VID: " << vid << L", PID: " << pid << std::endl;
										}
									}
								}

								RegCloseKey(hSubSubSubKey);
							}
						}					
						RegCloseKey(hSubSubKey);
					}
					subSubKeyNameSize = sizeof(subSubKeyName) / sizeof(TCHAR); // 크기를 재설정
				}
				RegCloseKey(hSubKey);
			}
			subKeyNameSize = sizeof(subKeyName) / sizeof(TCHAR); // 크기를 재설정
		}

		RegCloseKey(hKey);
		}
	else
	{
		std::cout << "Failed to open registry key." << std::endl;
	}

	return 0;
}


#if 0
int main()
{
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Enum\\USB"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
			
		// 하위 tree 정보를 출력
		DWORD i = 0;
		TCHAR subKeyName[1024];
		DWORD subKeyNameSize = sizeof(subKeyName) / sizeof(TCHAR);
		while (RegEnumKey(hKey, i++, subKeyName, subKeyNameSize) == ERROR_SUCCESS)
		{
			HKEY hSubKey;
			if (RegOpenKeyEx(hKey, subKeyName, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
			{
				DWORD j = 0;
				TCHAR subSubKeyName[1024];
				DWORD subSubKeyNameSize = sizeof(subSubKeyName) / sizeof(TCHAR);
				while (RegEnumKey(hSubKey, j++, subSubKeyName, subSubKeyNameSize) == ERROR_SUCCESS)
				{
					_tprintf(TEXT("%s\\%s\n"), subKeyName, subSubKeyName);
					subSubKeyNameSize = sizeof(subSubKeyName) / sizeof(TCHAR); // 크기를 재설정
				}
				RegCloseKey(hSubKey);
			}
		}

		RegCloseKey(hKey);
	}
	else
	{
		std::cout << "Failed to open registry key." << std::endl;
	}

	return 0;
}

#endif

#if 0

std::map<std::wstring, std::wstring> GetComPortInfo() 
{
	std::map<std::wstring, std::wstring> comPortInfo;
	HKEY hKey;
	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		DWORD index = 0;
		wchar_t valueName[1024];
		DWORD valueNameSize = sizeof(valueName);
		while (RegEnumValueW(hKey, index++, valueName, &valueNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
			DWORD dataSize = 0;
			RegQueryValueExW(hKey, valueName, NULL, NULL, NULL, &dataSize);
			std::vector<wchar_t> data(dataSize);
			if (RegQueryValueExW(hKey, valueName, NULL, NULL, reinterpret_cast<BYTE*>(&data[0]), &dataSize) == ERROR_SUCCESS) {
				std::wstring comPortName = &data[0];
				comPortInfo[valueName] = comPortName;
			}
			valueNameSize = sizeof(valueName);
		}
		RegCloseKey(hKey);
	}
	return comPortInfo;
}

// 함수를 사용하여 USB 포트 정보와 VID/PID를 가져오고 맵에 저장
std::map<std::wstring, std::pair<DWORD, DWORD>> GetUsbPortInfoWithID() {
	std::map<std::wstring, std::pair<DWORD, DWORD>> usbPortInfo;
	HKEY hKey;
	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Enum\\USB\\", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		DWORD index = 0;
		wchar_t valueName[1024];
		DWORD valueNameSize = sizeof(valueName);
		while (RegEnumValueW(hKey, index++, valueName, &valueNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
		{
			DWORD dataSize = 0;
			RegQueryValueExW(hKey, valueName, NULL, NULL, NULL, &dataSize);
			std::vector<wchar_t> data(dataSize);
#if 0
			DWORD bufferSize = MAX_PATH;
			HKEY hDevKey;
			if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, (L"SYSTEM\\CurrentControlSet\\Enum\\" + comPortName).c_str(), 0, KEY_READ, &hDevKey) == ERROR_SUCCESS)
			{
				DWORD bufferSize = MAX_PATH;
				wchar_t deviceInstanceId[MAX_PATH];
				if (RegQueryValueExW(hDevKey, L"DeviceInstanceID", NULL, NULL, reinterpret_cast<BYTE*>(deviceInstanceId), &bufferSize) == ERROR_SUCCESS)
				{
					// 디바이스 인스턴스 ID에서 VID와 PID 추출
					std::wstring instanceId(deviceInstanceId);

					/*size_t vidPos = instanceId.find(L"VID_");
					size_t pidPos = instanceId.find(L"PID_");
					if (vidPos != std::wstring::npos && pidPos != std::wstring::npos)
					{
						DWORD vid = std::wcstoul(instanceId.c_str() + vidPos + 4, nullptr, 16);
						DWORD pid = std::wcstoul(instanceId.c_str() + pidPos + 4, nullptr, 16);
						comPortInfo[comPortName] = std::make_pair(vid, pid);
					}*/
				}
			}
			RegCloseKey(hKey);
#endif
		}
		RegCloseKey(hKey);
	}
	return usbPortInfo;
}

// 함수를 사용하여 COM 포트 정보와 VID/PID를 가져오고 맵에 저장
std::map<std::wstring, std::pair<DWORD, DWORD>> GetComPortInfoWithID() {
	std::map<std::wstring, std::pair<DWORD, DWORD>> comPortInfo;
	HKEY hKey;
	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_READ, &hKey) == ERROR_SUCCESS) 
	{
		DWORD index = 0;
		wchar_t valueName[1024];
		DWORD valueNameSize = sizeof(valueName);
		while (RegEnumValueW(hKey, index++, valueName, &valueNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) 
		{
			DWORD dataSize = 0;
			RegQueryValueExW(hKey, valueName, NULL, NULL, NULL, &dataSize);
			std::vector<wchar_t> data(dataSize);

			if (RegQueryValueExW(hKey, valueName, NULL, NULL, reinterpret_cast<BYTE*>(&data[0]), &dataSize) == ERROR_SUCCESS) 
			{
				std::wstring comPortName = &data[0];

				// VID와 PID를 가져오기 위해 디바이스 인터페이스 정보를 열기
				/*

				*/
				HKEY hDevKey;
				
				if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Enum\\USB", 0, KEY_READ, &hDevKey) == ERROR_SUCCESS) 
				{
					DWORD bufferSize = MAX_PATH;

					wchar_t deviceInstanceId[MAX_PATH];
					if (RegQueryValueExW(hDevKey, L"DeviceInstanceID", NULL, NULL, reinterpret_cast<BYTE*>(deviceInstanceId), &bufferSize) == ERROR_SUCCESS) 
					{
						// 디바이스 인스턴스 ID에서 VID와 PID 추출
						/*
						std::wstring instanceId(deviceInstanceId);
						size_t vidPos = instanceId.find(L"VID_");
						size_t pidPos = instanceId.find(L"PID_");
						if (vidPos != std::wstring::npos && pidPos != std::wstring::npos) 
						{
							DWORD vid = std::wcstoul(instanceId.c_str() + vidPos + 4, nullptr, 16);
							DWORD pid = std::wcstoul(instanceId.c_str() + pidPos + 4, nullptr, 16);
							comPortInfo[comPortName] = std::make_pair(vid, pid);
						}
						*/
					}
					RegCloseKey(hDevKey);
				}
			}
			valueNameSize = sizeof(valueName);
		}
		RegCloseKey(hKey);
	}
	return comPortInfo;
}

int main() {

	std::map<std::wstring, std::wstring> comPortInfo = GetComPortInfo();
	std::cout << "comPortInfo count: " << comPortInfo.size() << std::endl;
	// 결과 출력
	for (const auto& pair : comPortInfo) {
		std::wcout << L"Port Name: " << pair.first << L", Port Description: " << pair.second << std::endl;
	}


	// COM 포트 정보와 VID/PID 가져오기
	std::map<std::wstring, std::pair<DWORD, DWORD>> comPortInfoID = GetComPortInfoWithID();

	// 검색 결과
	// Port Name: COM1, VID: 0, PID: 0
	std::cout << "comPortInfo count: " << comPortInfoID.size() << std::endl;

	// 결과 출력
	for (const auto& pair : comPortInfoID) {
		std::wcout << L"Port Name: " << pair.first << L", VID: " << pair.second.first << L", PID: " << pair.second.second << std::endl;
	}

	return 0;
}




#endif


#if 0

int main()
{
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Enum\\USB"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		DWORD i = 0;
		TCHAR subKeyName[1024];
		DWORD subKeyNameSize = sizeof(subKeyName) / sizeof(TCHAR);
		while (RegEnumKey(hKey, i++, subKeyName, subKeyNameSize) == ERROR_SUCCESS)
		{
			HKEY hSubKey;
			if (RegOpenKeyEx(hKey, subKeyName, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
			{
				DWORD j = 0;
				TCHAR subSubKeyName[1024];
				DWORD subSubKeyNameSize = sizeof(subSubKeyName) / sizeof(TCHAR);
				while (RegEnumKey(hSubKey, j++, subSubKeyName, subSubKeyNameSize) == ERROR_SUCCESS)
				{
					_tprintf(TEXT("%s\\%s\n"), subKeyName, subSubKeyName);
					subSubKeyNameSize = sizeof(subSubKeyName) / sizeof(TCHAR); // 크기를 재설정
				}
				RegCloseKey(hSubKey);
			}
		}
		RegCloseKey(hKey);
	}

	return 0;
}




#endif


#if 0


	int main()
	{
		HKEY hKey;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Enum\\USB"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			DWORD i = 0;
			TCHAR subKeyName[1024];
			while (RegEnumKey(hKey, i++, subKeyName, sizeof(subKeyName)) == ERROR_SUCCESS)
			{
				HKEY hSubKey;
				if (RegOpenKeyEx(hKey, subKeyName, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
				{
					DWORD j = 0;
					TCHAR subSubKeyName[1024];
					while (RegEnumKey(hSubKey, j++, subSubKeyName, sizeof(subSubKeyName)) == ERROR_SUCCESS)
					{
						_tprintf(TEXT("%s\\%s\n"), subKeyName, subSubKeyName);
					}
					RegCloseKey(hSubKey);
				}
			}
			RegCloseKey(hKey);
		}

		return 0;
	}

#endif

#if 0

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <windows.h>


#pragma comment(lib, "Advapi32.lib")
	/**
	 *
	 * cl /EHsc /std:c++20 findport.cpp /Fe: m.exe /link Advapi32.lib
	*/

	// 멀티바이트 문자열을 유니코드 문자열로 변환하는 함수
	std::wstring StringToWString(const std::string & str) {
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), NULL, 0);
		std::wstring wstr(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), &wstr[0], size_needed);
		return wstr;
	}

	std::map<std::wstring, std::wstring> GetComPortInfo() {
		std::map<std::wstring, std::wstring> comPortInfo;
		HKEY hKey;
		if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
			DWORD index = 0;
			wchar_t valueName[1024];
			DWORD valueNameSize = sizeof(valueName);
			while (RegEnumValueW(hKey, index++, valueName, &valueNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
				DWORD dataSize = 0;
				RegQueryValueExW(hKey, valueName, NULL, NULL, NULL, &dataSize);
				std::vector<wchar_t> data(dataSize);
				if (RegQueryValueExW(hKey, valueName, NULL, NULL, reinterpret_cast<BYTE*>(&data[0]), &dataSize) == ERROR_SUCCESS) {
					std::wstring comPortName = &data[0];
					comPortInfo[valueName] = comPortName;
				}
				valueNameSize = sizeof(valueName);
			}
			RegCloseKey(hKey);
		}
		return comPortInfo;
	}

	// 함수를 사용하여 USB 포트 정보와 VID/PID를 가져오고 맵에 저장
	std::map<std::wstring, std::pair<DWORD, DWORD>> GetUsbPortInfoWithID() {
		std::map<std::wstring, std::pair<DWORD, DWORD>> usbPortInfo;
		HKEY hKey;
		if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Enum\\USB\\", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			DWORD index = 0;
			wchar_t valueName[1024];
			DWORD valueNameSize = sizeof(valueName);
			while (RegEnumValueW(hKey, index++, valueName, &valueNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
			{
				DWORD dataSize = 0;
				RegQueryValueExW(hKey, valueName, NULL, NULL, NULL, &dataSize);
				std::vector<wchar_t> data(dataSize);
#if 0
				DWORD bufferSize = MAX_PATH;
				HKEY hDevKey;
				if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, (L"SYSTEM\\CurrentControlSet\\Enum\\" + comPortName).c_str(), 0, KEY_READ, &hDevKey) == ERROR_SUCCESS)
				{
					DWORD bufferSize = MAX_PATH;
					wchar_t deviceInstanceId[MAX_PATH];
					if (RegQueryValueExW(hDevKey, L"DeviceInstanceID", NULL, NULL, reinterpret_cast<BYTE*>(deviceInstanceId), &bufferSize) == ERROR_SUCCESS)
					{
						// 디바이스 인스턴스 ID에서 VID와 PID 추출
						std::wstring instanceId(deviceInstanceId);

						/*size_t vidPos = instanceId.find(L"VID_");
						size_t pidPos = instanceId.find(L"PID_");
						if (vidPos != std::wstring::npos && pidPos != std::wstring::npos)
						{
							DWORD vid = std::wcstoul(instanceId.c_str() + vidPos + 4, nullptr, 16);
							DWORD pid = std::wcstoul(instanceId.c_str() + pidPos + 4, nullptr, 16);
							comPortInfo[comPortName] = std::make_pair(vid, pid);
						}*/
					}
				}
				RegCloseKey(hKey);
#endif
			}
			RegCloseKey(hKey);
		}
		return usbPortInfo;
	}

	// 함수를 사용하여 COM 포트 정보와 VID/PID를 가져오고 맵에 저장
	std::map<std::wstring, std::pair<DWORD, DWORD>> GetComPortInfoWithID() {
		std::map<std::wstring, std::pair<DWORD, DWORD>> comPortInfo;
		HKEY hKey;
		if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
			DWORD index = 0;
			wchar_t valueName[1024];
			DWORD valueNameSize = sizeof(valueName);
			while (RegEnumValueW(hKey, index++, valueName, &valueNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
				DWORD dataSize = 0;
				RegQueryValueExW(hKey, valueName, NULL, NULL, NULL, &dataSize);
				std::vector<wchar_t> data(dataSize);

				if (RegQueryValueExW(hKey, valueName, NULL, NULL, reinterpret_cast<BYTE*>(&data[0]), &dataSize) == ERROR_SUCCESS) {
					std::wstring comPortName = &data[0];

					// VID와 PID를 가져오기 위해 디바이스 인터페이스 정보를 열기
					/*

					*/
					std::cout << "comPortName size: " << comPortName.size() << std::endl;
					HKEY hDevKey;
					std::wcout << L"RegOpenKeyExW " << (L"SYSTEM\\CurrentControlSet\\Enum\\" + comPortName).c_str()
						<< L" LSTATUS " << RegOpenKeyExW(HKEY_LOCAL_MACHINE, (L"SYSTEM\\CurrentControlSet\\Enum\\" + comPortName).c_str(), 0, KEY_READ, &hDevKey) << std::endl;

					if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, (L"SYSTEM\\CurrentControlSet\\Enum\\" + comPortName).c_str(), 0, KEY_READ, &hDevKey) == ERROR_SUCCESS) {
						DWORD bufferSize = MAX_PATH;

						wchar_t deviceInstanceId[MAX_PATH];
						if (RegQueryValueExW(hDevKey, L"DeviceInstanceID", NULL, NULL, reinterpret_cast<BYTE*>(deviceInstanceId), &bufferSize) == ERROR_SUCCESS) {
							// 디바이스 인스턴스 ID에서 VID와 PID 추출
							std::wstring instanceId(deviceInstanceId);
							size_t vidPos = instanceId.find(L"VID_");
							size_t pidPos = instanceId.find(L"PID_");
							if (vidPos != std::wstring::npos && pidPos != std::wstring::npos) {
								DWORD vid = std::wcstoul(instanceId.c_str() + vidPos + 4, nullptr, 16);
								DWORD pid = std::wcstoul(instanceId.c_str() + pidPos + 4, nullptr, 16);
								comPortInfo[comPortName] = std::make_pair(vid, pid);
							}
						}
						RegCloseKey(hDevKey);
					}
				}
				valueNameSize = sizeof(valueName);
			}
			RegCloseKey(hKey);
		}
		return comPortInfo;
	}

	int main() {

		std::map<std::wstring, std::wstring> comPortInfo = GetComPortInfo();
		std::cout << "comPortInfo count: " << comPortInfo.size() << std::endl;
		// 결과 출력
		for (const auto& pair : comPortInfo) {
			std::wcout << L"Port Name: " << pair.first << L", Port Description: " << pair.second << std::endl;
		}


		// COM 포트 정보와 VID/PID 가져오기
		std::map<std::wstring, std::pair<DWORD, DWORD>> comPortInfoID = GetComPortInfoWithID();
		std::map<std::wstring, std::pair<DWORD, DWORD>> usbnfoID = GetUsbPortInfoWithID();

		// 검색 결과
		// Port Name: COM1, VID: 0, PID: 0
		std::cout << "comPortInfo count: " << comPortInfoID.size() << std::endl;

		// 결과 출력
		for (const auto& pair : comPortInfoID) {
			std::wcout << L"Port Name: " << pair.first << L", VID: " << pair.second.first << L", PID: " << pair.second.second << std::endl;
		}

		return 0;
	}


#endif

#if 0

#include <iostream>
#include <Windows.h>

#include <SetupAPI.h>

#include <devguid.h>
#include <tchar.h>
#include <cfgmgr32.h>
#pragma comment(lib, "SetupAPI.lib")


	int main()
	{
		HDEVINFO hDevInfo;
		SP_DEVINFO_DATA DeviceInfoData;
		DWORD i;

		// List all connected USB devices
		hDevInfo = SetupDiGetClassDevs(NULL, TEXT("USB"), NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
		if (hDevInfo == INVALID_HANDLE_VALUE)
		{
			std::cout << "Error: " << GetLastError() << std::endl;
			return 1;
		}

		DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
		{
			TCHAR szDeviceInstanceID[MAX_DEVICE_ID_LEN];
			if (CM_Get_Device_ID(DeviceInfoData.DevInst, szDeviceInstanceID, MAX_DEVICE_ID_LEN, 0) != CR_SUCCESS)
				continue;

			TCHAR szDesc[1024];
			DWORD dwSize;
			if (!SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_DEVICEDESC, NULL, (BYTE*)szDesc, sizeof(szDesc), &dwSize))
				continue;

			_tprintf(TEXT("Device Desc: %s\n"), szDesc);
			_tprintf(TEXT("Device Instance ID: %s\n"), szDeviceInstanceID);
		}

		if (GetLastError() != NO_ERROR && GetLastError() != ERROR_NO_MORE_ITEMS)
		{
			std::cout << "Error: " << GetLastError() << std::endl;
			return 1;
		}

		SetupDiDestroyDeviceInfoList(hDevInfo);

		return 0;
	}

#endif
#if 0
	int main()
	{
		HDEVINFO hDevInfo;
		SP_DEVINFO_DATA DeviceInfoData;
		DWORD i;

		// List all connected USB devices
		hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_USB, NULL, NULL, DIGCF_PRESENT | DIGCF_PROFILE);
		if (hDevInfo == INVALID_HANDLE_VALUE)
			return 1;

		DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
		{
			DWORD DataT;
			LPTSTR buffer = NULL;
			DWORD buffersize = 0;

			while (!SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_DEVICEDESC, &DataT, (PBYTE)buffer, buffersize, &buffersize))
			{
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
				{
					if (buffer)
						LocalFree(buffer);
					buffer = (LPTSTR)LocalAlloc(LPTR, buffersize * 2);
				}
				else
					break;
			}

			if (buffer)
				LocalFree(buffer);
		}

		if (GetLastError() != NO_ERROR && GetLastError() != ERROR_NO_MORE_ITEMS)
			return 1;

		SetupDiDestroyDeviceInfoList(hDevInfo);

		return 0;
	}

#endif


#if 0
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <regstr.h>
#include <iostream>

#pragma comment(lib, "setupapi.lib")

	int main()
	{
		HDEVINFO hDevInfo;
		SP_DEVINFO_DATA DeviceInfoData;
		DWORD i;

		// Create a HDEVINFO with all present devices.
		hDevInfo = SetupDiGetClassDevs(NULL, 0, 0, DIGCF_PRESENT | DIGCF_ALLCLASSES);

		if (hDevInfo == INVALID_HANDLE_VALUE)
		{
			return 1;
		}

		// Enumerate through all devices in Set.
		DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
		{
			DWORD DataT;
			LPTSTR buffer = NULL;
			DWORD buffersize = 0;

			while (!SetupDiGetDeviceRegistryProperty(
				hDevInfo,
				&DeviceInfoData,
				SPDRP_HARDWAREID,
				&DataT,
				(PBYTE)buffer,
				buffersize,
				&buffersize))
			{
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
				{
					if (buffer)
					{
						LocalFree(buffer);
					}
					buffer = (LPTSTR)LocalAlloc(LPTR, buffersize * 2);
				}
				else
				{
					break;
				}
			}

			if (buffer)
			{
				std::wstring hardwareID(buffer);
				LocalFree(buffer);

				// Check if VID and PID are present in the hardware ID.
				size_t vidPos = hardwareID.find(L"VID_");
				size_t pidPos = hardwareID.find(L"PID_");

				if (vidPos != std::wstring::npos && pidPos != std::wstring::npos)
				{
					// Extract the VID and PID values.
					std::wstring vid = hardwareID.substr(vidPos + 4, 4);
					std::wstring pid = hardwareID.substr(pidPos + 4, 4);

					// Print the VID and PID values.
					std::wcout << L"VID:" << vid << L", PID:" << pid << std::endl;
				}
			}
		}

		// Destroy the HDEVINFO handle when we're finished with it.
		SetupDiDestroyDeviceInfoList(hDevInfo);

		return 0;
	}
#endif