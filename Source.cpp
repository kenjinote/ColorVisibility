#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>

TCHAR szClassName[] = TEXT("Window");

// 0=見にくい, 1=まあOK, 2=完璧
int ColorVisibility(COLORREF color1, COLORREF color2)
{
	const int nBrightness1 = (int)((GetRValue(color1)*299.0 + GetGValue(color1)*587.0 + GetBValue(color1)*114.0) / 1000.0);
	const int nBrightness2 = (int)((GetRValue(color2)*299.0 + GetGValue(color2)*587.0 + GetBValue(color2)*114.0) / 1000.0);
	const int nDifference =
		(max(GetRValue(color1), GetRValue(color2)) - min(GetRValue(color1), GetRValue(color2)))
		+ (max(GetGValue(color1), GetGValue(color2)) - min(GetGValue(color1), GetGValue(color2)))
		+ (max(GetBValue(color1), GetBValue(color2)) - min(GetBValue(color1), GetBValue(color2)));
	return (abs(nBrightness1 - nBrightness2) >= 125) + (nDifference >= 500);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static CHOOSECOLOR cc;
	static COLORREF TextColor;
	static COLORREF BkColor;
	static COLORREF CustColors[16];
	static TCHAR szText[1024];
	static int nColorVisibility;
	switch (msg)
	{
	case WM_CREATE:
		CreateWindow(TEXT("BUTTON"), TEXT("文字色..."), WS_CHILD | WS_VISIBLE, 10, 10, 256, 32, hWnd, (HMENU)100, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		CreateWindow(TEXT("BUTTON"), TEXT("背景色..."), WS_CHILD | WS_VISIBLE, 10, 50, 256, 32, hWnd, (HMENU)101, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		CreateWindow(TEXT("BUTTON"), TEXT("ランダム"), WS_CHILD | WS_VISIBLE, 10, 90, 256, 32, hWnd, (HMENU)102, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		cc.lStructSize = sizeof(CHOOSECOLOR);
		cc.hwndOwner = hWnd;
		cc.lpCustColors = CustColors;
		cc.Flags = CC_FULLOPEN | CC_RGBINIT;
		PostMessage(hWnd, WM_COMMAND, 102, 0);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == 100)
		{
			cc.rgbResult = TextColor;
			if (ChooseColor(&cc))
			{
				TextColor = cc.rgbResult;
				nColorVisibility = ColorVisibility(TextColor, BkColor);
				InvalidateRect(hWnd, 0, 0);
			}
		}
		else if (LOWORD(wParam) == 101)
		{
			cc.rgbResult = BkColor;
			if (ChooseColor(&cc))
			{
				BkColor = cc.rgbResult;
				nColorVisibility = ColorVisibility(TextColor, BkColor);
				InvalidateRect(hWnd, 0, 0);
			}
		}
		else if (LOWORD(wParam) == 102)
		{
			while (1)
			{
				TextColor = RGB(rand() % 256, rand() % 256, rand() % 256);
				BkColor = RGB(rand() % 256, rand() % 256, rand() % 256);
				nColorVisibility = ColorVisibility(TextColor, BkColor);
				if (nColorVisibility == 2)break;
			}
			InvalidateRect(hWnd, 0, 0);
		}
		break;
	case WM_PAINT:
		{
			RECT rect;
			GetClientRect(hWnd, &rect);
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			SetTextColor(hdc, TextColor);
			SetBkColor(hdc, BkColor);
			lstrcpy(szText, (nColorVisibility == 2) ? TEXT("完璧に見やすい") : ((nColorVisibility == 1) ? TEXT("やや見やすい") : TEXT("見にくい")));
			ExtTextOut(hdc, 10, 130, ETO_OPAQUE, &rect, szText, lstrlen(szText), 0);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("文字の組み合わせが見やすいかどうか判定"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
