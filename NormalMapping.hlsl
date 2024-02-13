//������������������������������������������������������������������������������
 // �e�N�X�`�����T���v���[�f�[�^�̃O���[�o���ϐ���`
//������������������������������������������������������������������������������
Texture2D       g_texture : register(t0);   //�e�N�X�`���[
SamplerState    g_sampler : register(s0);   //�T���v���[

Texture2D       g_normal_texture : register(t1);    //�e�N�X�`���[

//������������������������������������������������������������������������������
// �R���X�^���g�o�b�t�@
// DirectX �����瑗�M����Ă���A�|���S�����_�ȊO�̏����̒�`
//������������������������������������������������������������������������������
cbuffer gmodel:register(b0)
{
    float4x4    matW;           //���[���h�s��
    float4x4    matWVP;         //���[���h�r���[�v���W�F�N�V�����s��
    float4x4    matNormal;      //�m�[�}��
    float4      diffuseColor;   //�}�e���A���̐F
    float4      ambientColor;   //����
    float4      specularColor;  //���ʔ���
    float       shininess;
    int         hasTexture;     //�e�N�X�`���̗L��
    int         hasNormalTexture;
    float       scrollX;
    float       scrollY;
};

cbuffer gmodel:register(b1)
{
    float4      lightPosition;
    float4      eyePosition;
};

//������������������������������������������������������������������������������
// ���_�V�F�[�_�[�o�́��s�N�Z���V�F�[�_�[���̓f�[�^�\����
//������������������������������������������������������������������������������
struct VS_OUT
{
    float4 pos      : SV_POSITION;  //�ʒu
    float2 uv       : TEXCOORD;     //UV���W
    float4 color    : COLOR;    //�F�i���邳�j
    float4 eyev     : POSITION;
    float4 normal   : NORMAL;
    float4 Neyev    : POSITION1;
    float4 light    : POSITION2;
    float4 t        : POSITION3;
    float4 b        : POSITION4;
};

//������������������������������������������������������������������������������
// ���_�V�F�[�_
//������������������������������������������������������������������������������
VS_OUT VS(float4 pos : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL, float4 tangent : TANGENT)
{
    //�s�N�Z���V�F�[�_�[�֓n�����
    VS_OUT outData = (VS_OUT)0;

    //���[�J�����W�ɁA���[���h�E�r���[�E�v���W�F�N�V�����s���������
    //�X�N���[�����W�ɕϊ����A�s�N�Z���V�F�[�_�[��
    outData.pos = mul(pos, matWVP);
    outData.uv = (float2)uv;

    float3 binormal = cross(tangent, normal);
    binormal = mul(binormal, matNormal);
    binormal = normalize(binormal);

    normal = mul(normal, matNormal);
    normal = normalize(normal); //�@���x�N�g�������[�J�����W�ɕϊ��������
    normal.w = 0;
    outData.normal = normal;

    tangent.w = 0;
    tangent = mul(tangent, matNormal);
    tangent = normalize(tangent); //�ڐ��x�N�g�������[�J�����W�ɕϊ��������

    binormal = mul(binormal, matNormal);
    binormal = normalize(binormal); //�]�@���x�N�g�������[�J�����W�ɕϊ��������

    float4 posw = mul(pos, matW);
    outData.eyev = normalize(posw - eyePosition); //���[���h���W�̎����x�N�g��

    outData.Neyev.x = dot(outData.eyev, tangent);//�ڋ�Ԃ̎����x�N�g��
    outData.Neyev.y = dot(outData.eyev, binormal);
    outData.Neyev.z = dot(outData.eyev, outData.normal);
    outData.Neyev.w = 0;

    float4 light = normalize(lightPosition);
    light.w = 0;

    outData.color = mul(light, outData.normal);
    outData.color.w = 0.0;

    outData.light.x = dot(light, tangent);//�ڋ�Ԃ̌����x�N�g��
    outData.light.y = dot(light, binormal);
    outData.light.z = dot(light, outData.normal);
    outData.light.w = 1;

    outData.t = (tangent.x, tangent.y, tangent.z,1);
    outData.b = (binormal.x, binormal.y, binormal.z,1);


    //�܂Ƃ߂ďo��
    return outData;
}

//������������������������������������������������������������������������������
// �s�N�Z���V�F�[�_
//������������������������������������������������������������������������������
float4 PS(VS_OUT inData) : SV_Target
{
    float4 lightSource = float4(1.0, 1.0, 1.0, 1.0);
    float4 diffuse;
    float4 ambient;

    float2 tmpUV = inData.uv;
    tmpUV.x = tmpUV.x + scrollX;
    tmpUV.y = tmpUV.y - scrollY;
    
    // �m�[�}���}�b�v�e�N�X�`���̗L���̊m�F
    //if (hasNormalTexture)return float4(1, 0, 0, 1);
    //return float4(0, 0, 0, 1);


    /*if (hasNormalTexture)
        return g_normal_texture.Sample(g_sampler, inData.uv);
    else*/

    if (hasNormalTexture)
    {
        //inData.light = normalize(inData.light);
        float4 tmpNormal = g_normal_texture.Sample(g_sampler, tmpUV.xy) ;
        tmpNormal.w = 1;
        tmpNormal = mul(tmpNormal, matW);
        tmpNormal = normalize(tmpNormal);

        //return (tmpNormal);
        float NL = dot(normalize(lightPosition), tmpNormal);
        //return NL;
        float4 reflection = reflect(normalize(lightPosition), tmpNormal);//�������������ĂȂ�����Alightpos�����Ƃ�����


        //return (reflection);
        float4 specular = pow(saturate(dot(reflection, inData.Neyev)), shininess) * specularColor;

        if (hasTexture != 0) {
            diffuse = g_texture.Sample(g_sampler, inData.uv) * NL;
            ambient = g_texture.Sample(g_sampler, inData.uv) * ambientColor;

        }
        else {
            diffuse = diffuseColor * NL;
            ambient = diffuseColor * ambientColor;
        }
        return diffuse + specular + ambient;
    }
    else
    {
        float4 reflection = reflect(normalize(lightPosition), inData.normal);

        float4 specular = pow(saturate(dot(normalize(reflection), inData.eyev)), shininess) * specularColor;
        if (hasTexture == 0) {
            diffuse = lightSource * diffuseColor * inData.color;
            ambient = lightSource * diffuseColor * ambientColor;
        }
        else {
            diffuse = lightSource * g_texture.Sample(g_sampler, inData.uv) * inData.color;
            ambient = lightSource * g_texture.Sample(g_sampler, inData.uv) * ambientColor;
        }
        return diffuse + specular + ambient;
    }
}