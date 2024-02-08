//───────────────────────────────────────
 // テクスチャ＆サンプラーデータのグローバル変数定義
//───────────────────────────────────────
Texture2D       g_texture : register(t0);   //テクスチャー
SamplerState    g_sampler : register(s0);   //サンプラー

Texture2D       g_normal_texture : register(t1);    //テクスチャー

//───────────────────────────────────────
// コンスタントバッファ
// DirectX 側から送信されてくる、ポリゴン頂点以外の諸情報の定義
//───────────────────────────────────────
cbuffer gmodel:register(b0)
{
    float4x4    matW;           //ワールド行列
    float4x4    matWVP;         //ワールドビュープロジェクション行列
    float4x4    matNormal;      //ノーマル
    float4      diffuseColor;   //マテリアルの色
    float4      ambientColor;   //環境光
    float4      specularColor;  //鏡面反射
    float       shininess;
    int        hasTexture;     //テクスチャの有無
    int        hasNormalTexture;
    float       scrollX;
    float       scrollY;
};

cbuffer gmodel:register(b1)
{
    float4      lightPosition;
    float4      eyePosition;
};

//───────────────────────────────────────
// 頂点シェーダー出力＆ピクセルシェーダー入力データ構造体
//───────────────────────────────────────
struct VS_OUT
{
    float4 pos      : SV_POSITION;  //位置
    float2 uv       : TEXCOORD;     //UV座標
    float4 color    : COLOR;    //色（明るさ）
    float4 eyev     : POSITION;
    float4 normal   : NORMAL;
    float4 Neyev    : POSITION1;
    float4 light    : POSITION2;
    float4 t        : POSITION3;
    float4 b        : POSITION4;
};

//───────────────────────────────────────
// 頂点シェーダ
//───────────────────────────────────────
VS_OUT VS(float4 pos : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL, float4 tangent : TANGENT)
{
    //ピクセルシェーダーへ渡す情報
    VS_OUT outData = (VS_OUT)0;

    //ローカル座標に、ワールド・ビュー・プロジェクション行列をかけて
    //スクリーン座標に変換し、ピクセルシェーダーへ
    outData.pos = mul(pos, matWVP);
    outData.uv = (float2)uv;

    float3 binormal = cross(tangent, normal);
    binormal = mul(binormal, matNormal);
    binormal = normalize(binormal);

    normal = mul(normal, matNormal);
    normal = normalize(normal); //法線ベクトルをローカル座標に変換したやつ
    normal.w = 0;
    outData.normal = normal;

    tangent.w = 0;
    tangent = mul(tangent, matNormal);
    tangent = normalize(tangent); //接線ベクトルをローカル座標に変換したやつ

    binormal = mul(binormal, matNormal);
    binormal = normalize(binormal); //従法線ベクトルをローカル座標に変換したやつ

    float4 posw = mul(pos, matW);
    outData.eyev = normalize(posw - eyePosition); //ワールド座標の視線ベクトル

    outData.Neyev.x = dot(outData.eyev, tangent);//接空間の視線ベクトル
    outData.Neyev.y = dot(outData.eyev, binormal);
    outData.Neyev.z = dot(outData.eyev, outData.normal);
    outData.Neyev.w = 0;

    float4 light = normalize(lightPosition);
    light.w = 0;

    outData.color = mul(light, outData.normal);
    outData.color.w = 0.0;

    outData.light.x = dot(light, tangent);//接空間の光源ベクトル
    outData.light.y = dot(light, binormal);
    outData.light.z = dot(light, outData.normal);
    outData.light.w = 1;

    outData.t = (tangent.x, tangent.y, tangent.z,1);
    outData.b = (binormal.x, binormal.y, binormal.z,1);


    //まとめて出力
    return outData;
}

//───────────────────────────────────────
// ピクセルシェーダ
//───────────────────────────────────────
float4 PS(VS_OUT inData) : SV_Target
{
    float4 lightSource = float4(1.0, 1.0, 1.0, 1.0);
    float4 diffuse;
    float4 ambient;

    float2 tmpUV = inData.uv;
    tmpUV.x = tmpUV.x + scrollX;
    tmpUV.y = tmpUV.y - scrollY;
    
    // ノーマルマップテクスチャの有無の確認
    //if (hasNormalTexture)return float4(1, 0, 0, 1);
    //return float4(0, 0, 0, 1);


    /*if (hasNormalTexture)
        return g_normal_texture.Sample(g_sampler, inData.uv);
    else*/

    if (hasNormalTexture)
    {
        //inData.light = normalize(inData.light);
        float4 tmpNormal = g_normal_texture.Sample(g_sampler, inData.uv) ;
        tmpNormal.w = 1;
        tmpNormal = mul(tmpNormal, matW);
        tmpNormal = normalize(tmpNormal);

        //return (tmpNormal);
        float NL = dot(normalize(lightPosition), tmpNormal);
        //return NL;
        float4 reflection = reflect(normalize(lightPosition), tmpNormal);

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