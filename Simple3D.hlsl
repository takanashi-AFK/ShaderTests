//───────────────────────────────────────
 // テクスチャ＆サンプラーデータのグローバル変数定義
//───────────────────────────────────────
Texture2D   g_texture : register(t0);   //テクスチャー
SamplerState    g_sampler : register(s0);   //サンプラー

//───────────────────────────────────────
// コンスタントバッファ
// DirectX 側から送信されてくる、ポリゴン頂点以外の諸情報の定義
//───────────────────────────────────────
cbuffer global
{
    float4x4    matWVP;     // ワールド・ビュー・プロジェクションの合成行列
    float4x4    matNormal;       // ワールド行列
    float4		diffuseColor;		// ディフューズカラー（マテリアルの色）
    bool 		isTexture;		// テクスチャ貼ってあるかどうか
};

//───────────────────────────────────────
// 頂点シェーダー出力＆ピクセルシェーダー入力データ構造体
//───────────────────────────────────────
struct VS_OUT
{
    float4 pos      : SV_POSITION;  //位置
    float2 uv       : TEXCOORD; //UV座標
    float4 color    : COLOR;    //色（輝度）
};

//───────────────────────────────────────
// 頂点シェーダ
//───────────────────────────────────────
VS_OUT VS(float4 pos : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL)
{
    //ピクセルシェーダーへ渡す情報
    VS_OUT outData;

    //ローカル座標に、ワールド・ビュー・プロジェクション行列をかけて
    //スクリーン座標に変換し、ピクセルシェーダーへ
    outData.pos = mul(pos, matWVP);

    //テクスチャデータをピクセルシェーダーへ
    outData.uv = uv;

    //法線を回転
    normal = mul(normal, matNormal);

    //輝度情報をピクセルシェーダ―へ
    float4 light = float4(-1, 0.5, -0.7, 0);
    light = normalize(light);
    outData.color = clamp(dot(normal, light), 0, 1);;

    //まとめて出力
    return outData;
}

//───────────────────────────────────────
// ピクセルシェーダ
//───────────────────────────────────────
float4 PS(VS_OUT inData) : SV_Target
{
    float4 ambientSource = float4(0.5,0.5,0.5,1.0);//暗い場所でも環境校などが反射して見える色
    float4 lightSource = float4(1.0f,1.0f,1.0f,0.0f);//光の色
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
        ambient = lightSource * g_texture.Sample(g_sampler, inData.uv) * ambientSource;//表面色*強さ
    }
    return diffuse + ambient;
}