
#ifndef _DEBUG
#define WIN32_LEAN_AND_MEAN
#endif // !_DEBUG

#include<windows.h>
#include<memory>
#include"Model.h"

Model D3DObject;

bool animate = false;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);

	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = "CourseWork";

	if (!RegisterClassEx(&wndClass))
		return -1;

	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hwnd = CreateWindowA("CourseWork", "View 3D model", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
		NULL, NULL, hInstance, NULL);
	if (!hwnd)
		return -1;

	ShowWindow(hwnd, cmdShow);

	// D3DObject Initialize
	bool result = D3DObject.Initialize(hInstance, hwnd);

	if (!result)
		return -1;

	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Update and Draw
			if (animate)
				D3DObject.Update();
			D3DObject.Render();
		}
	}

	// D3DObject Shutdown
	D3DObject.Shutdown();

	return static_cast<int>(msg.wParam);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct;
	HDC hDC;

	switch (message)
	{
	case WM_PAINT:
		hDC = BeginPaint(hwnd, &paintStruct);
		EndPaint(hwnd, &paintStruct);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
	{
		int vkey = wParam;

		switch (vkey)
		{
		case VK_UP:
			if (GetAsyncKeyState(VK_CONTROL) < 0)
				D3DObject.CameraRotate(-0.01f);
			else
				D3DObject.CameraTranslate(-0.1f);
			break;
		case VK_DOWN:
			if (GetAsyncKeyState(VK_CONTROL) < 0)
				D3DObject.CameraRotate(0.01f);
			else
				D3DObject.CameraTranslate(0.1f);
			break;
		case VK_RIGHT:
			D3DObject.Translate(-0.1f);
			break;
		case VK_LEFT:
			D3DObject.Translate(0.1f);
			break;
		case 0x41:
			animate = !animate;
			break;
		default:
			break;
		}
	}
	break;
	case WM_MOUSEMOVE: {
		POINT ptCurrentPoint = { 0 };
		GetCursorPos(&ptCurrentPoint);

		MOUSEMOVEPOINT mmpSamplePoint = { 0 };
		mmpSamplePoint.x = ptCurrentPoint.x;
		mmpSamplePoint.y = ptCurrentPoint.y;

		MOUSEMOVEPOINT mmpRecordedMovements[2] = { 0 };

		const int nPtCount = GetMouseMovePointsEx(sizeof(mmpSamplePoint), &mmpSamplePoint, mmpRecordedMovements, 2, GMMP_USE_DISPLAY_POINTS);

		if (GetAsyncKeyState(VK_LBUTTON) < 0) {
			if (mmpRecordedMovements[1].x - mmpRecordedMovements[0].x > 0)
				D3DObject.Rotate(0.05f, 0.0f);
			if (mmpRecordedMovements[1].x - mmpRecordedMovements[0].x < 0)
				D3DObject.Rotate(-0.05f, 0.0f);
		}
	}
					   break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}