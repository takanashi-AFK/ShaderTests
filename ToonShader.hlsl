//������������������������������������������������������������������������������
// �e�N�X�`�����T���v���[�f�[�^�̃O���[�o���ϐ���`
//������������������������������������������������������������������������������
Texture2D   g_texture : register(t0);   //�e�N�X�`���[
Texture2D   g_toontexture : register(t1);

SamplerState    g_sampler : register(s0);   //�T���v���[

//������������������������������������������������������������������������������
 // �R���X�^���g�o�b�t�@
// DirectX �����瑗�M����Ă���A�|���S�����_�ȊO�̏����̒�`
//������������������������������������������������������������������������������

//cBuffer from FBX.h
cbuffer global:register(b0)
{
    float4x4    matW;           //���[���h�s��
    float4x4    matWVP;         // ���[���h�E�r���[�E�v���W�F�N�V�����̍����s��
    float4x4    matNormal;      //�@��
    float4      diffuseColor;   // �f�B�t���[�Y�J���[�i�}�e���A���̐F�j
    bool        isTexture;      // �e�N�X�`���\���Ă��邩�ǂ���
};

//cBuffer from stage.cpp
cbuffer global:register(b1)
{
    float4      lightPosition;  //���C�g�̕���
    float4      eyepos;         //���_
};
//������������������������������������������������������������������������������
// ���_�V�F�[�_�[�o�́��s�N�Z���V�F�[�_�[���̓f�[�^�\����
//������������������������������������������������������������������������������
struct VS_OUT
{
    float4 pos      : SV_POSITION;  //�ʒu
    float2 uv       : TEXCOORD; //UV���W
    float4 color    : COLOR;    //�F�i���邳�j
    float4 eyev     : POSITION1;
    float4 normal   : POSITION2;
    float4 light    : POSITION3;
};

//������������������������������������������������������������������������������
// ���_�V�F�[�_
//������������������������������������������������������������������������������
VS_OUT VS(float4 pos : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL)
{
    //�s�N�Z���V�F�[�_�[�֓n�����
    VS_OUT outData = (VS_OUT)0;
    //���[�J�����W�ɁA���[���h�E�r���[�E�v���W�F�N�V�����s���������
    //�X�N���[�����W�ɕϊ����A�s�N�Z���V�F�[�_�[��
    pos = pos + normal * 0.05;
    outData.pos = mul(pos, matWVP);
    outData.uv = uv;
    normal.w = 0;// 0����Ƃ��ƃo�O�Ƃ������Ȃ�
    normal = mul(normal, matNormal);
    normal = normalize(normal);
    outData.normal = normal;

    float4 light = normalize(lightPosition);
    light = normalize(light);

    outData.color = saturate(dot(normal, light));
    float4 posw = mul(pos, matW);
    outData.eyev = eyepos - posw;
    //�܂Ƃ߂ďo��
    return outData;
}

//������������������������������������������������������������������������������
// �s�N�Z���V�F�[�_
//������������������������������������������������������������������������������
float4 PS(VS_OUT inData) : SV_Target
{
    float4 lightSource = float4(1.0, 1.0, 1.0, 1.0);
    float4 ambentSource = float4(0.2, 0.2, 0.2, 1.0); //���̂��ǂꂾ�������𔽎˂܂��͕��˂��邩�𐧌䂷��
    float4 diffuse;
    float4 ambient;
    float4 NL = dot(inData.normal, normalize(lightPosition));

    float4 reflect = normalize(2 * NL * inData.normal - normalize(lightPosition));
    float4 specular = pow(saturate(dot(reflect, normalize(inData.eyev))), 8);

    /*  float4 n1 = float4(1 / 4.0, 1 / 4.0, 1 / 4.0, 1);
      float4 n2 = float4(2 / 4.0, 2 / 4.0, 2 / 4.0, 1);
      float4 n3 = float4(3 / 4.0, 3 / 4.0, 3 / 4.0, 1);
      float4 n4 = float4(4 / 4.0, 4 / 4.0, 4 / 4.0, 1);

      float4 tI = 0.1 * step(n1, inData.color) +
                  0.2 * step(n2, inData.color) +
                  0.3 * step(n3, inData.color) +
                  0.4 * step(n4, inData.color);*/
      float2 uv;
      uv.x = inData.color.x;
      uv.y = 0;

      if (abs(dot(inData.normal, normalize(inData.eyev))) > 0.3)
          return g_toontexture.Sample(g_sampler, inData.color);
      else
          return float4(0, 0, 0, 0);


      if (isTexture == false)
      {
          diffuse = lightSource * diffuseColor * inData.color;
          ambient = lightSource * diffuseColor * ambentSource;
      }
      else
      {
          diffuse = lightSource * g_texture.Sample(g_sampler, inData.uv) * inData.color;
          ambient = lightSource * g_texture.Sample(g_sampler, inData.uv) * ambentSource;
      }

      //{//�֊s�̏���
      //if (abs(dot(inData.normal,normalize(inData.eyev)))>0.3)
      //    return float4(1, 1, 1, 1);
      //else
      //    return float4(0, 0, 0, 0);
      //}

}