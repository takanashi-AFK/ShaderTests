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
    float4x4    matW;       // ワールド行列
    float4x4    matNormal;
    float       scroll;
};

//───────────────────────────────────────
// 頂点シェーダー出力＆ピクセルシェーダー入力データ構造体
//───────────────────────────────────────
struct VS_OUT
{
    float4 pos      : SV_POSITION;  //位置
    float2 uv       : TEXCOORD; //UV座標
};

//───────────────────────────────────────
// 頂点シェーダ
//───────────────────────────────────────
VS_OUT VS(float4 pos : POSITION, float4 uv : TEXCOORD)
{
    //ピクセルシェーダーへ渡す情報
    VS_OUT outData;

    //ローカル座標に、ワールド行列をかけて
    //スクリーン座標に変換し、ピクセルシェーダーへ
    outData.pos = mul(pos, matW);

    //テクスチャデータをピクセルシェーダーへ
    outData.uv = uv;

    //まとめて出力
    return outData;
}

//───────────────────────────────────────
// ピクセルシェーダ
//───────────────────────────────────────
float4 PS(VS_OUT inData) : SV_Target
{
    float4 output;
    float2 tmpUV = inData.uv;
    tmpUV.x = tmpUV.x + scroll;

    output = g_texture.Sample(g_sampler, tmpUV.xy);


   // output =( (output.x * 2) + (output.y * 4) + (output.z))/7; //GlayScale
    //output = floor(g_texture.Sample(g_sampler, inData.uv) * 8) / 8; //postrerization
    return  output;
}