#pragma once
#define SAFE_DELETE(p) if(p!= nullptr){delete p; p= nullptr;}
#define SAFE_RELEASE(p) if(p != nullptr){ p->Release(); p = nullptr;}
#include <d3d11.h>

enum SHADER_TYPE
{
	SHADER_3D,
	SHADER_2D,
	SHADER_TOON,
	SHADER_OUTLINE,
	SHADER_NORMAL,
	SHADER_MAX,
};

//�����J
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace Direct3D
{
	extern ID3D11Device* pDevice_;		//�f�o�C�X
	extern 	ID3D11DeviceContext* pContext_;		//�f�o�C�X�R���e�L�X�g
	//������
	HRESULT Initialize(int winW, int winH, HWND hWnd);

	//�V�F�[�_�[����
	HRESULT InitShader();

	HRESULT InitShader3D();

	HRESULT InitShader2D();

	HRESULT InitToonShader();

	HRESULT InitOutLineShader();

	HRESULT InitNormalShader();

	void SetShader(SHADER_TYPE type);


	//�`��J�n
	void BeginDraw();

	//�`��I��
	void EndDraw();

	//���
	void Release();

	void SetDepthBafferWriteEnable(bool _toggle);
}