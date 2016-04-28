#include <windows.h>
#include "Resources.h"
//#include "Picture.cpp"

HDC hdcMem = NULL;
HBITMAP hBmp = NULL;
HBITMAP hBmpOld = NULL;

// call this function once in your WM_COMMAND handler, or wherever you want the image to be loaded
void LoadPic(HDC hWinDC) {
	if(hdcMem)
		return;  // already loaded, no need to load it again

	// note:  here you must put the file name in a TEXT() macro.  DO NOT CAST IT TO LPCSTR
	hBmp = (HBITMAP)LoadImage(NULL,TEXT("C:/Users/1896491/Desktop/Danielson/workspace/ComputerWarningsLibrary/teste2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );

	if(!hBmp) {  // the load failed (couldn't find file?  Invalid file?)
		::MessageBox(NULL, TEXT("LoadImage Failed"), TEXT("Error"), MB_OK);
		return;
	}

	hdcMem = CreateCompatibleDC(NULL);
	if (hdcMem == NULL) {
		::MessageBox(NULL, TEXT("CreateCompatibleDC Failed"), TEXT("Error"), MB_OK);
		return;
	}
	
	BITMAP qBitmap;
	int iReturn = GetObject(reinterpret_cast<HGDIOBJ>(hBmp), sizeof(BITMAP), reinterpret_cast<LPVOID>(&qBitmap));
	if (!iReturn) {
		::MessageBox(NULL, TEXT("GetObject Failed"), TEXT("Error"), MB_OK);
		return;
	}
	
	hBmpOld = (HBITMAP)::SelectObject(hdcMem, hBmp);
	if (!hBmpOld) {
		::MessageBox(NULL, TEXT("SelectObject Failed"), TEXT("Error"), MB_OK);
		return;
	}
	
	BOOL qRetBlit = ::BitBlt(hWinDC, 0, 0, qBitmap.bmWidth, qBitmap.bmHeight, hdcMem, 0, 0, SRCCOPY );
	if (!qRetBlit) {
		::MessageBox(NULL, TEXT("BitBlt Failed"), TEXT("Error"), MB_OK);
		return;
	}
	return;
}

// call this function when the program is done (shutting down)
void FreePic() {
	if(hdcMem){
		SelectObject(hdcMem, hBmpOld);
		DeleteObject(hBmp);
		DeleteDC(hdcMem);
	}
}

/* This is where all the input to the window goes to */
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch(Message) {
		
		/* Upon destruction, tell the main thread to stop */
		case WM_DESTROY: {
			FreePic();
			PostQuitMessage(0);
			break;
		}		
		case WM_COMMAND: {
            if (LOWORD(wParam) == 1) {
//                MessageBox(hwnd, "Loaded", "Load", MB_OK | MB_ICONINFORMATION);
			}
            break;
        }
        case WM_PAINT: {
        	// do this in your WM_PAINT message handler:
			PAINTSTRUCT ps;
			HDC screen = BeginPaint(hwnd,&ps);
			LoadPic(screen);
			//StretchBlt( screen, ... /* fill in your coords and junk here );
			EndPaint( hwnd, &ps );
			break;
		}
		/* All other messages (a lot of them) are processed using default procedures */
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

/* The 'main' function of Win32 GUI programs: this is where execution starts */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc; /* A properties struct of our window */
	HWND hwnd; /* A 'HANDLE', hence the H, or a pointer to our window */
	MSG msg; /* A temporary location for all messages */

	/* zero out the struct and set the stuff we want to modify */
	memset(&wc,0,sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc; /* This is where we will send messages to */
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	
	/* White, COLOR_WINDOW is just a #define for a system color, try Ctrl+Clicking it */
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName = "WindowClass";
	wc.hIcon		 = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(MyIcon)); /* Load a standard icon */
	wc.hIconSm		 = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(MyIcon)); /* use the name "A" to use the project icon */

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	int WIDTH = 1200;
	int HEIGHT = 800;

	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"WindowClass",
		"AVISO DOS COMPUTADORES DA BIBLIOTECA IFRN/PARNAMIRIM", /* caption */
		WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		(GetSystemMetrics(SM_CXSCREEN) - WIDTH) /2, /* x */
		(GetSystemMetrics(SM_CYSCREEN) - HEIGHT)/2 - 15, /* y */
		WIDTH,  /* width  */
		HEIGHT, /* height */
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if(hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	/*
		This is the heart of our program where all input is processed and 
		sent to WndProc. Note that GetMessage blocks code flow until it receives something, so
		this loop will not produce unreasonably high CPU usage
	*/
	while(GetMessage(&msg, NULL, 0, 0) > 0) { /* If no error is received... */
		TranslateMessage(&msg); /* Translate key codes to chars if present */
		DispatchMessage(&msg); /* Send it to WndProc */
	}
	return msg.wParam;
}
