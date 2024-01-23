//───────────────────────────────────────
// テクスチャ＆サンプラーデータのグローバル変数定義
//───────────────────────────────────────
Texture2D   g_texture : register(t0);   //テクスチャー
SamplerState    g_sampler : register(s0);   //サンプラー
Texture2D normalTex : register(t1);
//───────────────────────────────────────
 // コンスタントバッファ
// DirectX 側から送信されてくる、ポリゴン頂点以外の諸情報の定義
//───────────────────────────────────────
cbuffer global:register(b0)
{
    float4x4    matW;           //ワールド行列
    float4x4    matWVP;         // ワールド・ビュー・プロジェクションの合成行列
    float4x4    matNormal;      //法線
    float4      diffuseColor;   // ディフューズカラー（マテリアルの色）
    float4      ambientColor;
    float4      specularColor;
    float       shininess;
    int         isTexture;      // テクスチャ貼ってあるかどうか
    int         isNormalMap;
};

cbuffer global:register(b1)
{
    float4      lightPosition;  //ライトの方向
    float4      eyepos;         //視点
}
//───────────────────────────────────────
// 頂点シェーダー出力＆ピクセルシェーダー入力データ構造体
//───────────────────────────────────────
struct VS_OUT
{
    float4 pos      : SV_POSITION;  //位置
    float2 uv       : TEXCOORD; //UV座標
    float4 eyev     : POSITION;
    float4 Neyev    : POSITION1;
    float4 normal   : POSITION2;
    float4 light    : POSITION3;
    float4 color    : POSITION4;    //色（明るさ）
};

//───────────────────────────────────────
// 頂点シェーダ
//───────────────────────────────────────
VS_OUT VS(float4 pos : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL, float4 tangent:TANGENT)
{
    //ピクセルシェーダーへ渡す情報
    VS_OUT outData = (VS_OUT)0;
    //ローカル座標に、ワールド・ビュー・プロジェクション行列をかけて
    //スクリーン座標に変換し、ピクセルシェーダーへ
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

    outData.light.x = dot(light, tangent);//接空間の光源ベクトル
    outData.light.y = dot(light, binormal);
    //まとめて出力	outData.light.z = dot(light, normal);
    outData.light.w = 0;

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

    if (isNormalMap)
    {
        //inData.light = normalize(inData.light);
        float4 tmpNormal = normalTex.Sample(g_sampler, inData.uv) * 2.0f - 1.0f;
        tmpNormal.w = 0;
        tmpNormal = normalize(tmpNormal);

        float4 NL = clamp(dot(tmpNormal, inData.light), 0, 1);

        float4 reflection = reflect(-inData.light, tmpNormal);
        float4 specular = pow(saturate(dot(reflection, inData.Neyev)), shininess) * specularColor;

        if (isTexture != 0)
        {
            diffuse = g_texture.Sample(g_sampler, inData.uv) * NL;
            ambient = g_texture.Sample(g_sampler, inData.uv) * ambientColor;
        }
        else
        {
            diffuse = diffuseColor * NL;
            ambient = diffuseColor * ambientColor;
        }
        return   specular;
    }
    else
    {
        float4 reflection = reflect(normalize(lightPosition), inData.normal);

        float4 specular = pow(saturate(dot(normalize(reflection), inData.eyev)), shininess) * specularColor;
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
        return specular;
    }
}