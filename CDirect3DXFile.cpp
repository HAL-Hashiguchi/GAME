#include	"CDirect3DXFile.h"

	/*-----------------------------
		X Fileを読み込む

			引数
				xfilename  : Ｘファイル名
				lpd3ddevice : デバイスオブジェクト

			戻り値
				true	成功
				false   失敗

	------------------------------*/
	bool CDirect3DXFile::LoadXFile(char* xfilename,	LPDIRECT3DDEVICE9 lpd3ddevice){

		LPD3DXBUFFER	pd3dxmtrlbuffer;
		LPD3DXBUFFER	pd3dxmtrlbuffer2;
		HRESULT			hr;
		unsigned int	i;

		// Ｘファイルを読み込む
		hr = D3DXLoadMeshFromX(	xfilename,				// Ｘファイル名
							D3DXMESH_SYSTEMMEM,			// 読み込みメモリ
							lpd3ddevice,				// デバイスオブジェクト
							NULL,
							&pd3dxmtrlbuffer,			// マテリアルバッファ
							&pd3dxmtrlbuffer2,			// マテリアルバッファ2
							&m_nummaterial,				// マテリアル数
							&m_lpmesh);					// メッシュ

		if(SUCCEEDED(hr)){
			// バッファポインタを獲得
			D3DXMATERIAL*	d3dxmaterials = (D3DXMATERIAL*)pd3dxmtrlbuffer->GetBufferPointer();		
			// マテリアルコンテナ
			m_lpmeshmaterials = new D3DMATERIAL9[m_nummaterial];				
			// テクスチャコンテナ	
			m_lpmeshtextures = new LPDIRECT3DTEXTURE9[m_nummaterial];			
		
			for(i=0;i<m_nummaterial;i++)
			{
				m_lpmeshmaterials[i] = d3dxmaterials[i].MatD3D;
//				m_lpmeshmaterials[i].Emissive = m_lpmeshmaterials[i].Specular = m_lpmeshmaterials[i].Ambient = m_lpmeshmaterials[i].Diffuse;			

				hr = D3DXCreateTextureFromFile(lpd3ddevice,	
											d3dxmaterials[i].pTextureFilename,
											&m_lpmeshtextures[i]);
				if(FAILED(hr))
				{
					m_lpmeshtextures[i]=NULL;
				}
			}
			pd3dxmtrlbuffer->Release();			// マテリアルバッファのリリース
			pd3dxmtrlbuffer2->Release();		// マテリアルバッファのリリース
		}
		else{
			return false;
		}
		return true;
	}

	/*-----------------------------
		X FileをＵｎｌｏａｄする
			引数
				なし

			戻り値
				true	成功
				false   失敗

	------------------------------*/
	void CDirect3DXFile::UnLoadXFile(){

		unsigned int	i;

		// マテリアルの解放
		if(m_lpmeshmaterials!=NULL){
			delete [] m_lpmeshmaterials;
			m_lpmeshmaterials=NULL;
		}

		// テクスチャの解放
		if(m_lpmeshtextures!=NULL){
			for(i=0;i<m_nummaterial;i++)
			{
				if(m_lpmeshtextures[i]!=NULL)
					m_lpmeshtextures[i]->Release();
			}

			delete [] m_lpmeshtextures;
			m_lpmeshtextures=NULL;
		}

		// メッシュ解放
		if(m_lpmesh!=NULL)
		{
			m_lpmesh->Release();
			m_lpmesh=NULL;
		}
	}

	/*-----------------------------
		X Fileを描画する
			引数
				lpd3ddevice :	デバイスオブジェクト

			戻り値
				なし

	------------------------------*/
	void CDirect3DXFile::Draw(LPDIRECT3DDEVICE9 lpd3ddevice){
		unsigned int i;
		for(i=0;i<m_nummaterial;i++)
		{
			lpd3ddevice->SetMaterial(&m_lpmeshmaterials[i]);	// マテリアルのセット
			lpd3ddevice->SetTexture(0,m_lpmeshtextures[i]);		// テクスチャのセット
			m_lpmesh->DrawSubset(i);							// サブセットの描画
		}
	}

	/*-----------------------------
		X Fileを描画する(3軸を描画する)
			引数
				lpd3ddevice :	デバイスオブジェクト

			戻り値
				なし

	------------------------------*/
	void CDirect3DXFile::DrawWithAxis(LPDIRECT3DDEVICE9 lpd3ddevice){
		unsigned int i;

		// ３軸の描画
		DrawAxis(lpd3ddevice);

		// サブセットを描画する
		for(i=0;i<m_nummaterial;i++)
		{
			lpd3ddevice->SetMaterial(&m_lpmeshmaterials[i]);	// マテリアルのセット
			lpd3ddevice->SetTexture(0,m_lpmeshtextures[i]);		// テクスチャのセット
			m_lpmesh->DrawSubset(i);							// サブセットの描画
		}
	}

	/*---------------------
		３軸の描画

			引数
				lpd3ddevice :	デバイスオブジェクト

			戻り値
				なし
	---------------------*/
	void CDirect3DXFile::DrawAxis(LPDIRECT3DDEVICE9 lpd3ddevice)
	{
		struct VERTEXDATA{
			float		x,y,z;
			D3DCOLOR	color;
		};
	
		// ３軸の頂点データ
		struct	VERTEXDATA	linevertices[6]={
			-400,  0,   0,  D3DCOLOR_XRGB(255,0,0),			// Ｘ軸始点  赤
			 400,  0,   0,  D3DCOLOR_XRGB(255,0,0),			// Ｘ軸終点　赤
			   0,-400,   0,  D3DCOLOR_XRGB(0,255,0),		// Ｙ軸始点　緑
			   0,400,   0,  D3DCOLOR_XRGB(0,255,0),			// Ｙ軸終点　緑
			   0,  0,-400,  D3DCOLOR_XRGB(0,0,255),			// Ｚ軸始点　青
			   0,  0, 400,  D3DCOLOR_XRGB(0,0,255)			// Ｚ軸終点　青
		};

		lpd3ddevice->SetRenderState(D3DRS_LIGHTING,false);
		// 頂点フォーマットをセット
		lpd3ddevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
		
		// ３軸を描画
		lpd3ddevice->DrawPrimitiveUP(D3DPT_LINELIST,
									 3,
									 &linevertices[0],
									 sizeof(VERTEXDATA));
		lpd3ddevice->SetRenderState(D3DRS_LIGHTING,true);
	}

	LPD3DXMESH CDirect3DXFile::GetMesh() const {
		return m_lpmesh;
	}