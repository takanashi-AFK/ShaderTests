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
    float4 pos  : SV_POSITION;	//�s�N�Z���ʒu
    float2 uv	: TEXCOORD;		//UV���W
    float4 eyev		:POSITION;	//���[���h���W�ɕϊ����ꂽ�����x�N�g��
    float4 Neyev	:POSITION1; //�m�[�}���}�b�v�p�̐ڋ�Ԃɕϊ����ꂽ�����x�N�g��
    float4 normal	:NORMAL;	//�@���x�N�g��
    float4 light	:POSITION2; //���C�g��ڋ�Ԃɕϊ������x�N�g��
    float4 color	:COLOR; //�ʏ�̃����o�[�g���f���̊g�U���˂̐F
};//������������������������������������������������������������������������������
// ���_�V�F�[�_
//������������������������������������������������������������������������������
VS_OUT VS(float4 pos : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL, float4 tangent:TANGENT)
{
    //�s�N�Z���V�F�[�_�[�֓n�����
    VS_OUT outData = (VS_OUT)0;
    //���[�J�����W�ɁA���[���h�E�r���[�E�v���W�F�N�V�����s���������
    //�X�N���[�����W�ɕϊ����A�s�N�Z���V�F�[�_�[��
    outData.pos = mul(pos, matWVP);
    outData.uv = (float2)uv;

    float3 temp = cross(tangent,normal);
    float4 binormal = { temp,0 };
    binormal = mul(binormal, matNormal);
    binormal = normalize(binormal);

    normal.w = 0;
    outData.normal = normalize(mul(normal,matNormal));

    tangent = mul(tangent, matNormal);
    tangent.w = 0;
    tangent = normalize(tangent);

    binormal = mul(binormal, matNormal);
    binormal = normalize(binormal);

    float4 posw = mul(pos, matW);
    outData.eyev = normalize(posw - eyepos);

    outData.Neyev.x = dot(outData.eyev, tangent);
    outData.Neyev.y = dot(outData.eyev, binormal);
    outData.Neyev.z = dot(outData.eyev, normal);
    outData.Neyev.w = 0;

    float4 light = normalize(lightPosition);
    light.w = 0;
    light = normalize(light);

    outData.color = float4(1,1,1,1);
    outData.color.w = 0.0;

    outData.light.x = dot(light, tangent);//�ڋ�Ԃ̌����x�N�g��
    outData.light.y = dot(light, binormal);
    outData.light.z = dot(light, normal);
    
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
    float4 lightSource = float4(1.0, 1.0, 1.0, 1.0);
    float4 diffuse;
    float4 ambient;
    float4 specular;

        if (isNormalMap)
        {
            //inData.light = normalize(inData.light);
            float4 tmpNormal = normalTex.Sample(g_sampler, inData.uv) * 2.0f - 1.0f;
            tmpNormal = normalize(tmpNormal);
            tmpNormal.w = 0;

            float4 NL = clamp(dot(normalize(inData.light),tmpNormal), 0, 1);

            float4 reflection = reflect(normalize(inData.light), tmpNormal);
            float4 specular = pow(saturate(dot(reflection, normalize(inData.Neyev))), shininess) * specularColor;

            if (isTexture != 0)
            {
                diffuse = lightSource* g_texture.Sample(g_sampler, inData.uv) * NL;
                ambient = lightSource* g_texture.Sample(g_sampler, inData.uv) * ambientColor;
            }
            else
            {
                diffuse = lightSource*diffuseColor * NL;
                ambient = lightSource*diffuseColor * ambientColor;
            }
            return diffuse ;
        }
        else
        {
            float4 reflection = reflect(normalize(lightPosition), inData.normal);

            float4 specular = pow(saturate(dot(reflection, inData.eyev)), shininess) * specularColor;
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