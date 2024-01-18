//������������������������������������������������������������������������������
// �e�N�X�`�����T���v���[�f�[�^�̃O���[�o���ϐ���`
//������������������������������������������������������������������������������
Texture2D   g_texture : register(t0);   //�e�N�X�`���[
SamplerState    g_sampler : register(s0);   //�T���v���[
Texture2D normalTex : register(t1);
//������������������������������������������������������������������������������
 // �R���X�^���g�o�b�t�@
// DirectX �����瑗�M����Ă���A�|���S�����_�ȊO�̏����̒�`
//������������������������������������������������������������������������������
cbuffer global:register(b0)
{
    float4x4    matW;           //���[���h�s��
    float4x4    matWVP;         // ���[���h�E�r���[�E�v���W�F�N�V�����̍����s��
    float4x4    matNormal;      //�@��
    float4      diffuseColor;   // �f�B�t���[�Y�J���[�i�}�e���A���̐F�j
    float4      ambientColor;
    float4      specularColor;
    float       shininess;
    int         isTexture;      // �e�N�X�`���\���Ă��邩�ǂ���
    int         isNormalMap;
};

cbuffer global:register(b1)
{
    float4      lightPosition;  //���C�g�̕���
    float4      eyepos;         //���_
}
//������������������������������������������������������������������������������
// ���_�V�F�[�_�[�o�́��s�N�Z���V�F�[�_�[���̓f�[�^�\����
//������������������������������������������������������������������������������
struct VS_OUT
{
    float4 pos      : SV_POSITION;  //�ʒu
    float2 uv       : TEXCOORD; //UV���W
    float4 eyev     : POSITION;
    float4 Neyev    : POSITION1;
    float4 normal   : POSITION2;
    float4 light    : POSITION3;
    float4 color    : POSITION4;    //�F�i���邳�j
};

//������������������������������������������������������������������������������
// ���_�V�F�[�_
//������������������������������������������������������������������������������
VS_OUT VS(float4 pos : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL, float4 tangent:TANGENT)
{
    //�s�N�Z���V�F�[�_�[�֓n�����
    VS_OUT outData = (VS_OUT)0;
    //���[�J�����W�ɁA���[���h�E�r���[�E�v���W�F�N�V�����s���������
    //�X�N���[�����W�ɕϊ����A�s�N�Z���V�F�[�_�[��
    outData.pos = mul(pos, matWVP);
    outData.uv = uv;
    float3 binormal = cross(normal, tangent);

    normal.w = 0;
    normal = mul(normal, matNormal);
    normal = normalize(normal);
    outData.normal = normal;

    tangent.w = 0;
    tangent = mul(tangent, matNormal);
    tangent = normalize(tangent);

    binormal = mul(binormal, matNormal);
    binormal = normalize(binormal);

    float4 posw = mul(pos, matW);
    outData.eyev = eyepos - posw;

    outData.Neyev.x = dot(outData.eyev, tangent);
    outData.Neyev.y = dot(outData.eyev, binormal);
    outData.Neyev.z = dot(outData.eyev, normal);
    outData.Neyev.w = 0;

    float4 light = normalize(lightPosition);
    light = normalize(light);

    outData.color = mul(light, normal);

    outData.light.x = dot(light, tangent);//�ڋ�Ԃ̌����x�N�g��
    outData.light.y = dot(light, binormal);
    //�܂Ƃ߂ďo��	outData.light.z = dot(light, normal);
    outData.light.w = 0;

    //�܂Ƃ߂ďo��
    return outData;
}

//������������������������������������������������������������������������������
// �s�N�Z���V�F�[�_
//������������������������������������������������������������������������������
float4 PS(VS_OUT inData) : SV_Target
{
  float4 lightSource = float4(1.0, 1.0, 1.0, 0.0);
    float4 diffuse;
    float4 ambient;

    if (isNormalMap)
    {
        inData.light = normalize(inData.light);

        float4 diffuse;
        float4 ambient;
        float4 specular;

        float4 tmpNormal = normalTex.Sample(g_sampler, inData.uv) * 2 - 1;
        tmpNormal.w = 0;
        tmpNormal = normalize(tmpNormal);

        float4 S = dot(tmpNormal, normalize(inData.light));
        S = clamp(S, 0, 1);

        float4 R = reflect(-inData.light, tmpNormal);
        specular = pow(saturate(dot(R, inData.Neyev)), shininess) * specularColor;

        if (isTexture != 0)
        {
            diffuse = lightSource * g_texture.Sample(g_sampler, inData.uv) * S ;
            ambient = lightSource * g_texture.Sample(g_sampler, inData.uv) * ambientColor;
        }
        else
        {
            diffuse = diffuseColor * S;
            ambient = diffuseColor * ambientColor;
        }
        return  diffuse + ambient + specular;
    }
    else
    {
        float4 NL = saturate(dot(inData.normal, normalize(lightPosition)));
        float4 reflection = reflect(normalize(-lightPosition), inData.normal);
        float4 specular = pow(saturate(dot(reflection, normalize(inData.eyev))), shininess) * specularColor;
        if (isTexture == 0)
        {
            diffuse = lightSource * diffuseColor * inData.color;
            ambient = lightSource * diffuseColor * ambientColor;
        }
        else
        {
            diffuse = lightSource * g_texture.Sample(g_sampler, inData.uv) * inData.color;
            ambient = lightSource * g_texture.Sample(g_sampler, inData.uv) * ambientColor;
        }
        return diffuse + ambient + specular;
    }


}