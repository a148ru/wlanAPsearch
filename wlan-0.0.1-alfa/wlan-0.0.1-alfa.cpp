// wlan-0.0.1-alfa.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
void makefile(PFILE stream, char* filename, PWLAN_BSS_LIST pBssList);
void printconsole(PWLAN_BSS_LIST pBssList);

int main(int argc, char **argv)
{
	// Декларируем и инициализируем переменные
	char filename[100] = { '\0' };
	FILE *stream = NULL;
	HANDLE hClient = NULL;
	DWORD dwMaxClient = 2;
	DWORD dwCurVersion = 0;
	DWORD dwResult = 0;
	DWORD dwRetVal = 0;

	/* Переменные используемые для  WlanEnumInterfaces */
	PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
	PWLAN_INTERFACE_INFO pIfInfo = NULL;
	PWLAN_BSS_LIST pBssList = NULL;
	//PWLAN_BSS_ENTRY pBssEntry = NULL;
	BOOL bSecurityEnabled = TRUE;

	if (argv[1]) {
		for (int i = 0; i < 100 & i<strlen(argv[1]); i++) {
			filename[i] = argv[1][i];
		}
	}


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
			//вызов нужной функции тут
			if(filename[0] != '\0') makefile(stream, filename, pBssList);
			else printconsole(pBssList);
		}
	}

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

void makefile(PFILE stream, char* filename, PWLAN_BSS_LIST pBssList)
{
	PWLAN_BSS_ENTRY pBssEntry = NULL;

	fopen_s(&stream, filename, "w");

		fprintf(stream,"Ssid;Bssid;Rssi;LinkQuality;Frequency\n");
		for (int j = 0; j < (int)pBssList->dwNumberOfItems; j++) {
			pBssEntry = (WLAN_BSS_ENTRY*)&pBssList->wlanBssEntries[j];
			for (int k = 0; k < (int)pBssEntry->dot11Ssid.uSSIDLength; k++) {
				fprintf(stream,"%c", pBssEntry->dot11Ssid.ucSSID[k]);
			}
			fprintf(stream,";");
			for (int k = 0; k < 6; k++) {
				if (k % 2 == 0 && k > 0)fprintf(stream,".");

				fprintf(stream,"%0*x", 2, pBssEntry->dot11Bssid[k]);
			}
			float Q = 0;
			if (pBssEntry->ulChCenterFrequency / 1000000 == 2)Q = (float)2.4;
			else if (pBssEntry->ulChCenterFrequency / 1000000 == 5)Q = (float)5.0;
			else Q = 0;
			fprintf(stream,";%i dBm;%i;%.*f kGz\n", pBssEntry->lRssi, pBssEntry->uLinkQuality, 1, Q);
		}
	fclose(stream);
}

void printconsole(PWLAN_BSS_LIST pBssList)
{
	PWLAN_BSS_ENTRY pBssEntry = NULL;

	wprintf(L"Ssid;Bssid;Rssi;LinkQuality;Frequency\n");
	for (int j = 0; j < (int)pBssList->dwNumberOfItems; j++) {
		pBssEntry = (WLAN_BSS_ENTRY*)&pBssList->wlanBssEntries[j];
		for (int k = 0; k < (int)pBssEntry->dot11Ssid.uSSIDLength; k++) {
			wprintf(L"%c", pBssEntry->dot11Ssid.ucSSID[k]);
		}
		wprintf(L";");
		for (int k = 0; k < 6; k++) {
			if (k % 2 == 0 && k > 0)wprintf(L".");

			wprintf(L"%0*x", 2, pBssEntry->dot11Bssid[k]);
		}
		float Q = 0;
		if (pBssEntry->ulChCenterFrequency / 1000000 == 2)Q = (float)2.4;
		else if (pBssEntry->ulChCenterFrequency / 1000000 == 5)Q = (float)5.0;
		else Q = 0;
		wprintf(L";%i dBm;%i;%.*f kGz\n", pBssEntry->lRssi, pBssEntry->uLinkQuality, 1, Q);
	}

}