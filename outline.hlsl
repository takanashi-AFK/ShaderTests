cbuffer global:register(b0)
{
    float4x4    matW;           //���[���h�s��
    float4x4    matWVP;         // ���[���h�E�r���[�E�v���W�F�N�V�����̍����s��
    float4x4    matNormal;      //�@��
    float4      diffuseColor;   // �f�B�t���[�Y�J���[�i�}�e���A���̐F�j
    float4      ambientColor;
    float4      specularColor;
    float       shininess;
    bool        isTexture;      // �e�N�X�`���\���Ă��邩�ǂ���

};

float4 VS : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}