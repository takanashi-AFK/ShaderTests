cbuffer global:register(b0)
{
    float4x4    matW;           //ワールド行列
    float4x4    matWVP;         // ワールド・ビュー・プロジェクションの合成行列
    float4x4    matNormal;      //法線
    float4      diffuseColor;   // ディフューズカラー（マテリアルの色）
    float4      ambientColor;
    float4      specularColor;
    float       shininess;
    bool        isTexture;      // テクスチャ貼ってあるかどうか

};

float4 VS : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}