#include<Windows.h>
#include<CommCtrl.h>
#include"resource.h"

bool g_NoRecurse = false;
HBRUSH hBackground = CreateSolidBrush(RGB(169, 169, 169));


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
	{
		SendMessage(GetDlgItem(hwnd, IDC_SPIN_PREFIX), UDM_SETRANGE, 0, MAKEWORD(32, 0));
		break;
		
	}
	case WM_CTLCOLORDLG:
		return (INT_PTR)hBackground;

	/*case WM_CTLCOLORSTATIC:
	{
		HDC hdc = (HDC)wParam;
		SetTextColor(hdc, RGB(255, 255, 255));   // белый текст
		SetBkMode(hdc, TRANSPARENT);
		return (INT_PTR)hBackground;
	}

	case WM_CTLCOLOREDIT:
	{
		HDC hdc = (HDC)wParam;
		SetTextColor(hdc, RGB(0, 255, 0));       // зелёный текст
		SetBkColor(hdc, RGB(0, 0, 0));           // чёрный фон
		return (INT_PTR)CreateSolidBrush(RGB(0, 0, 0));
	}

	break;*/
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
	
				DWORD dwIPmask = 0;
				for (int i = 0; i < prefix; i++)
					dwIPmask |= (1 << (31 - i));

				BYTE a = (dwIPmask >> 24) & 255;
				BYTE b = (dwIPmask >> 16) & 255;
				BYTE c = (dwIPmask >> 8) & 255;
				BYTE d =  dwIPmask & 255;

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
