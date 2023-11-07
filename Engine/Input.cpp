#include "Input.h"

namespace Input
{
	LPDIRECTINPUT8   pDInput = nullptr;//directInput�̖{��
	LPDIRECTINPUTDEVICE8 pKeyDevice = nullptr;//�L�[�{�[�h�̂��
	LPDIRECTINPUTDEVICE8 pMouceDevice=nullptr;
	BYTE keyState[256] = { 0 };
	BYTE prevKeyState[256] = { 0 };  //�O�t���[���ł̊e�L�[�̏��
	//����LP�����Ă���|�C���^�ɂȂ��Ă�
	void Initialize(HWND hWnd)
	{
		DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&pDInput, nullptr);//�f�o�C�X�I�u�W�F�N�g���쐬
		pDInput->CreateDevice(GUID_SysKeyboard, &pKeyDevice, nullptr);
		pKeyDevice->SetDataFormat(&c_dfDIKeyboard);//�f�o�C�X�̎�ށi����̓L�[�{�[�h�j���w��
		pKeyDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);//�������x���i���̎��s���̃A�v���ɑ΂���D��x�j

	}

	void Update()
	{
		memcpy(prevKeyState, keyState, sizeof(BYTE) * 256);
		pKeyDevice->Acquire();//����Ȃ��Ƃ��܂ɃL�[�{�[�h�������炵��
		pKeyDevice->GetDeviceState(sizeof(keyState), &keyState);
	}

	bool IsKey(int keyCode)
	{
		if (keyState[keyCode] & 0x80)//8bit����f�[�^�̐擪bit��0��1�����肽������A128��&���Ƃ�
		{
			return true;
		}
		return false;
	}

	bool IsKeyDown(int keyCode)
	{
		//���͉����ĂāA�O��͉����ĂȂ�
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