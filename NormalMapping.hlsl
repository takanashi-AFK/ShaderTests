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
    bool         isTexture;      // テクスチャ貼ってあるかどうか
    bool         isNormalMap;
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
    float4 pos  : SV_POSITION;	//ピクセル位置
    float2 uv	: TEXCOORD;		//UV座標
    float4 eyev		:POSITION;	//ワールド座標に変換された視線ベクトル
    float4 Neyev	:POSITION1; //ノーマルマップ用の接空間に変換された視線ベクトル
    float4 normal	:NORMAL;	//法線ベクトル
    float4 light	:POSITION2; //ライトを接空間に変換したベクトル
    float4 color	:COLOR; //通常のランバートモデルの拡散反射の色
};//───────────────────────────────────────
// 頂点シェーダ
//───────────────────────────────────────
VS_OUT VS(float4 pos : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL, float4 tangent:TANGENT)
{
    //ピクセルシェーダーへ渡す情報
    VS_OUT outData = (VS_OUT)0;
    //ローカル座標に、ワールド・ビュー・プロジェクション行列をかけて
    //スクリーン座標に変換し、ピクセルシェーダーへ
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

    outData.light.x = dot(light, tangent);//接空間の光源ベクトル
    outData.light.y = dot(light, binormal);
    outData.light.z = dot(light, normal);
    
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
    float4 specular;

    if (isNormalMap)
    {
        //inData.light = normalize(inData.light);
        float4 tmpNormal = normalTex.Sample(g_sampler, inData.uv) * 2.0f - 1.0f;
        tmpNormal = normalize(tmpNormal);
        tmpNormal.w = 0;

        float4 NL = clamp(dot(normalize(inData.light), tmpNormal), 0, 1);

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
            return diffuse + ambient + specular;
        }
        else
        {
            float4 reflection = reflect(normalize(lightPosition), inData.normal);

        float4 specular = pow(saturate(dot(reflection, inData.eyev)), shininess) * specularColor;
        if (isTexture != 0)
        {
            diffuse = lightSource * g_texture.Sample(g_sampler, inData.uv) * inData.color;
            ambient = lightSource * g_texture.Sample(g_sampler, inData.uv) * ambientColor;
        }
        else       
        {
            diffuse = lightSource * diffuseColor * inData.color;
            ambient = lightSource * diffuseColor * ambientColor;
        }
        return diffuse + ambient + specular;
    }
}