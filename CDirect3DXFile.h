#ifndef CDirect3DXfile_h
#define CDirect3DXfile_h
#include	<d3d9.h>
#include	<d3dx9.h>
/*-------------------------------------------------------------------------------
	DirectX X File 基本クラス
--------------------------------------------------------------------------------*/
class CDirect3DXFile{
private:
	LPD3DXMESH					m_lpmesh;					// メッシュインターフェイス
	D3DMATERIAL9*				m_lpmeshmaterials;		// モデルデータのマテリアル
	LPDIRECT3DTEXTURE9*			m_lpmeshtextures;		// モデルデータのテクスチャ
	DWORD						m_nummaterial;			// マテリアル数
public:
	CDirect3DXFile():m_lpmeshmaterials(NULL),m_lpmeshtextures(NULL),m_lpmesh(NULL){
	}

	virtual ~CDirect3DXFile()
	{
		UnLoadXFile();
	}
	bool LoadXFile(char* xfilename,	LPDIRECT3DDEVICE9 lpd3ddevice);

	LPD3DXMESH GetMesh() const;

	void UnLoadXFile();
	
	void Draw(LPDIRECT3DDEVICE9 lpd3ddevice);

	void DrawWithAxis(LPDIRECT3DDEVICE9 lpd3ddevice);
	
	void DrawAxis(LPDIRECT3DDEVICE9 lpd3ddevice);
};

#endif