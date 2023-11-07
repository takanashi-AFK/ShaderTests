//������������������������������������������������������������������������������
 // �e�N�X�`�����T���v���[�f�[�^�̃O���[�o���ϐ���`
//������������������������������������������������������������������������������
Texture2D   g_texture : register(t0);   //�e�N�X�`���[
SamplerState    g_sampler : register(s0);   //�T���v���[

//������������������������������������������������������������������������������
// �R���X�^���g�o�b�t�@
// DirectX �����瑗�M����Ă���A�|���S�����_�ȊO�̏����̒�`
//������������������������������������������������������������������������������
cbuffer global
{
    float4x4    matWVP;     // ���[���h�E�r���[�E�v���W�F�N�V�����̍����s��
    float4x4    matNormal;       // ���[���h�s��
    float4		diffuseColor;		// �f�B�t���[�Y�J���[�i�}�e���A���̐F�j
    bool 		isTexture;		// �e�N�X�`���\���Ă��邩�ǂ���
};

//������������������������������������������������������������������������������
// ���_�V�F�[�_�[�o�́��s�N�Z���V�F�[�_�[���̓f�[�^�\����
//������������������������������������������������������������������������������
struct VS_OUT
{
    float4 pos      : SV_POSITION;  //�ʒu
    float2 uv       : TEXCOORD; //UV���W
    float4 color    : COLOR;    //�F�i�P�x�j
};

//������������������������������������������������������������������������������
// ���_�V�F�[�_
//������������������������������������������������������������������������������
VS_OUT VS(float4 pos : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL)
{
    //�s�N�Z���V�F�[�_�[�֓n�����
    VS_OUT outData;

    //���[�J�����W�ɁA���[���h�E�r���[�E�v���W�F�N�V�����s���������
    //�X�N���[�����W�ɕϊ����A�s�N�Z���V�F�[�_�[��
    outData.pos = mul(pos, matWVP);

    //�e�N�X�`���f�[�^���s�N�Z���V�F�[�_�[��
    outData.uv = uv;

    //�@������]
    normal = mul(normal, matNormal);

    //�P�x�����s�N�Z���V�F�[�_�\��
    float4 light = float4(-1, 0.5, -0.7, 0);
    light = normalize(light);
    outData.color = clamp(dot(normal, light), 0, 1);;

    //�܂Ƃ߂ďo��
    return outData;
}

//������������������������������������������������������������������������������
// �s�N�Z���V�F�[�_
//������������������������������������������������������������������������������
float4 PS(VS_OUT inData) : SV_Target
{
    float4 ambientSource = float4(0.5,0.5,0.5,1.0);//�Â��ꏊ�ł����Z�Ȃǂ����˂��Č�����F
    float4 lightSource = float4(1.0f,1.0f,1.0f,0.0f);//���̐F
    float4 diffuse;
    float4 ambient;
    //return lightSource *  g_texture.Sample(g_sampler, inData.uv) * inData.color;

    if(isTexture == 0)
    {
        diffuse = lightSource * diffuseColor * inData.color;
        ambient = lightSource * diffuseColor * ambientSource;
    }
    else
    {
        diffuse = lightSource * g_texture.Sample(g_sampler, inData.uv) * inData.color;
        ambient = lightSource * g_texture.Sample(g_sampler, inData.uv) * ambientSource;//�\�ʐF*����
    }
    return diffuse + ambient;
}