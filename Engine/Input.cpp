#include "Input.h"

namespace Input
{
	LPDIRECTINPUT8   pDInput = nullptr;//directInputの本体
	LPDIRECTINPUTDEVICE8 pKeyDevice = nullptr;//キーボードのやつ
	LPDIRECTINPUTDEVICE8 pMouceDevice=nullptr;
	BYTE keyState[256] = { 0 };
	BYTE prevKeyState[256] = { 0 };  //前フレームでの各キーの状態
	//頭にLPがついてたらポインタになってる
	void Initialize(HWND hWnd)
	{
		DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&pDInput, nullptr);//デバイスオブジェクトを作成
		pDInput->CreateDevice(GUID_SysKeyboard, &pKeyDevice, nullptr);
		pKeyDevice->SetDataFormat(&c_dfDIKeyboard);//デバイスの種類（今回はキーボード）を指定
		pKeyDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);//強調レベル（他の実行中のアプリに対する優先度）

	}

	void Update()
	{
		memcpy(prevKeyState, keyState, sizeof(BYTE) * 256);
		pKeyDevice->Acquire();//これないとたまにキーボード見失うらしい
		pKeyDevice->GetDeviceState(sizeof(keyState), &keyState);
	}

	bool IsKey(int keyCode)
	{
		if (keyState[keyCode] & 0x80)//8bitあるデータの先頭bitが0か1かしりたいから、128の&をとる
		{
			return true;
		}
		return false;
	}

	bool IsKeyDown(int keyCode)
	{
		//今は押してて、前回は押してない
		if (IsKey(keyCode) && !(prevKeyState[keyCode] & 0x80))
		{
			return true;
		}
		return false;
	}

	bool IsKeyUp(int keyCode)
	{
		if (!IsKey(keyCode) && prevKeyState[keyCode] & 0x80)
		{
			return true;
		}
		return false;
	}

	void Release()
	{
		SAFE_RELEASE(pDInput);
		SAFE_RELEASE(pKeyDevice);
	}
}