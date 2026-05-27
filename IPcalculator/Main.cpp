#include<Windows.h>
#include<CommCtrl.h>
#include"resource.h"

bool g_NoRecurse = false;


BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR ipCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, (DLGPROC)DlgProc, NULL);
	return 0 ;
}
BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hwnd, IDC_SPIN_PREFIX), UDM_SETRANGE, 0, MAKEWORD(32, 0));
		break;
	case WM_COMMAND:
	{
		HWND hIPaddress		= GetDlgItem(hwnd, IDC_IPADDRESS);
		HWND hIPmask		= GetDlgItem(hwnd, IDC_IPMASK);
		HWND hEditPrefix	= GetDlgItem(hwnd, IDC_EDIT_PREFIX);
		DWORD dwIPaddress	= 0;
		DWORD dwIPmask		= 0;
		//DWORD dwPrefix		= 0;
		switch (LOWORD(wParam))
		{
		case IDC_IPADDRESS:
			if (HIWORD(wParam) == EN_CHANGE)
			{

				SendMessage(hIPaddress, IPM_GETADDRESS, 0,(LPARAM) &dwIPaddress);
				DWORD dwFirst = FIRST_IPADDRESS(dwIPaddress);
				LPARAM lpMask = 0;
				if (dwFirst < 128) lpMask = MAKEIPADDRESS(255, 0, 0, 0);
				else if (dwFirst < 192) lpMask = MAKEIPADDRESS(255, 255, 0, 0);
				else if (dwFirst < 224) lpMask = MAKEIPADDRESS(255, 255, 255, 0);
				SendMessage(hIPmask, IPM_SETADDRESS, 0, lpMask);
			}
			break;
		case IDC_IPMASK:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				if (g_NoRecurse) break;
				SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);

				int prefix = 0;
				for (int i = 31; i >= 0; i--)
				{
					if (dwIPmask & (1 << i))
						prefix++;
					else
						break;
				}
				g_NoRecurse = true;
				SetDlgItemInt(hwnd, IDC_EDIT_PREFIX, prefix, FALSE);
				g_NoRecurse = false;
			}
			break;
		case IDC_EDIT_PREFIX:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				if (g_NoRecurse) break;
				int	prefix = GetDlgItemInt(hwnd, IDC_EDIT_PREFIX, NULL, FALSE);

				if (prefix < 0)	 prefix = 0;
				if (prefix > 32) prefix = 32;

				DWORD dwIPmask = 0;
				for (int i = 0; i < prefix; i++)
					dwIPmask |= (1 << (31 - i));

				BYTE a = (dwIPmask >> 24) & 0xFF;
				BYTE b = (dwIPmask >> 16) & 0xFF;
				BYTE c = (dwIPmask >> 8) & 0xFF;
				BYTE d =  dwIPmask & 0xFF;

				LPARAM lpMask = MAKEIPADDRESS(a, b, c, d);
				g_NoRecurse = true;
				SendMessage(hIPmask, IPM_SETADDRESS, 0, lpMask);
				g_NoRecurse = false;;
			}
			break;


		case IDOK:
			break;
		case IDCANCEL:EndDialog(hwnd, 0);
		}
	}
			break;
	case WM_CLOSE:EndDialog(hwnd, 0);
		break;
	}
	return FALSE;
}
