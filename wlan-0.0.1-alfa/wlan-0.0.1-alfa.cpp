// wlan-0.0.1-alfa.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"


int main(int argc, char **argv)
{
	// Декларируем и инициализируем переменные
	HANDLE hClient = NULL;
	DWORD dwMaxClient = 2;
	DWORD dwCurVersion = 0;
	DWORD dwResult = 0;
	DWORD dwRetVal = 0;
	//int iRet = 0;

	//WCHAR GuidString[39] = { 0 };

	/* Переменные используемые для  WlanEnumInterfaces */
	PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
	PWLAN_INTERFACE_INFO pIfInfo = NULL;
	//PWLAN_AVAILABLE_NETWORK_LIST pAvalNetList = NULL;
	//PWLAN_AVAILABLE_NETWORK pAvalNetEntry = NULL;
	PWLAN_BSS_LIST pBssList = NULL;
	PWLAN_BSS_ENTRY pBssEntry = NULL;
	BOOL bSecurityEnabled = TRUE;

	//int iRSSI = 0;


	// WlanOpenHandle()
	dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
	if (dwResult != ERROR_SUCCESS) {
		wprintf(L"WlanOpenHandle failed with error: %u\n", dwResult);
		return 1;
	}
	// WlanEnumInterfaces()
	dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
	if (dwResult != ERROR_SUCCESS) {
		wprintf(L"WlanEnumInterfaces failed with error: %u\n", dwResult);
		return 1;
	}
	else {
		for (int i = 0; i < (int)pIfList->dwNumberOfItems; i++) {
			pIfInfo = (WLAN_INTERFACE_INFO *)&pIfList->InterfaceInfo[i];
			dwResult = WlanScan(hClient, &pIfInfo->InterfaceGuid, NULL, NULL, NULL);
			Sleep(2000);
			if (dwResult != ERROR_SUCCESS) {
				return 1;
			}
			dwResult = WlanGetNetworkBssList(hClient, &pIfInfo->InterfaceGuid, NULL, dot11_BSS_type_infrastructure, bSecurityEnabled, NULL, &pBssList);
			if (dwResult != ERROR_SUCCESS) {
				return 1;
			}
			wprintf(L"Ssid;Bssid;Rssi;LinkQuality;Frequency\n");
			for (int j = 0; j < (int)pBssList->dwNumberOfItems; j++) {
				pBssEntry = (WLAN_BSS_ENTRY*)&pBssList->wlanBssEntries[j];
				for (int k = 0; k < (int)pBssEntry->dot11Ssid.uSSIDLength; k++) {
					wprintf(L"%c", pBssEntry->dot11Ssid.ucSSID[k]);
				}
				wprintf(L";");
				for (int k = 0; k < 6; k++) {
					if (k % 2 == 0 && k > 0)wprintf(L".");

					wprintf(L"%0*x", 2,pBssEntry->dot11Bssid[k]);
				}
				float Q = 0;
				if (pBssEntry->ulChCenterFrequency / 1000000 == 2)Q = (float)2.4;
				else if (pBssEntry->ulChCenterFrequency / 1000000 == 5)Q = (float)5.0;
				else Q = 0;
				wprintf(L";%i dBm;%i;%.*f kGz\n", pBssEntry->lRssi, pBssEntry->uLinkQuality, 1, Q);
			}

		}
	}
	/*
	if (pAvalNetList != NULL) {
		WlanFreeMemory(pAvalNetList);
		pAvalNetList = NULL;
	}
	*/

	if (pBssList != NULL) {
		WlanFreeMemory(pBssList);
		pBssList = NULL;
	}

	if (pIfList != NULL) {
		WlanFreeMemory(pIfList);
		pIfList = NULL;
	}
	WlanCloseHandle(hClient, NULL);
	return 0;
}

