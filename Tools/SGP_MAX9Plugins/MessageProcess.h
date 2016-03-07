#pragma once

#define CHARLENGTH 64

#include <vector>
typedef struct
{
	char actionName[CHARLENGTH];
	int startFrameID;
	int endFrameID;
	int fileNo;
}ActionItemType;

typedef std::vector<ActionItemType> ActionList;

//process sgp-engine export setting dialog message
class MessageProcess
{
	friend class ActionModifyDlg;
public:
	static MessageProcess* GetInstance();
	static void ReleaseInstance();
private:
	static MessageProcess* s_instance;
	MessageProcess();
public:
	void InitDlg(HWND hwnd);//init dlg
	void InitCtrl(HWND hwnd);//init control
	void ShowToolTips(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);//show tooltips
	void DrawExportButton(LPARAM lParam);

	void AddAction(HWND hwnd);				//add action
	void DeleteAction(HWND hwnd);
	void ClearAction(HWND hwnd);
	void ModifyAction(HWND hwnd,BOOL bCheck=TRUE);

	void SetWorkDir(HWND hwnd);//设置工作路径
	void SetMeshDir(HWND hwnd);//设置mesh存放路径
	void SetBoneDir(HWND hwnd);//设置bone存放路径
	void SetTextureDir(HWND hwnd);//设置Texture存放路径
public:
	void SaveSettings(HWND hwnd);			//save settings
	void LoadSettings(HWND hwnd);			//load settings
	void SetToDefaultSettings(HWND hwnd);	//set to default
	BOOL CheckDlgSettings(HWND hwnd);		//check setting valid
	void ReleaseResource();					//release dynamic-located resource
private:
	void CreateToolTipCtrl(HWND hwnd);
	void AddToolTip(HWND hwnd,int nCtrlID,PTSTR pszText);

	int GetInsertPos(HWND hwnd);
	BOOL CheckActionFileNo(HWND hwnd,int pos);
	BOOL CheckInputAction(HWND hwnd);
	BOOL CheckStartEndTime(HWND hwnd,int nStart,int nEnd);
	BOOL CheckActionName(HWND hwnd,char* str);

	void SaveSettingsToFile();
	void LoadSettingsFromFile();
	void GetSettingFilePath(char* path);
	void LoadDefaultSetting();
	void GetDefaultFilePath(char *path);
private:
	HWND m_hToolTip;//tooltips
public:
	/*********************************************************
	通过以下数据成员即可得到导出对话框进行的设置
	*********************************************************/
	//目录
	char m_WorkDir[MAX_PATH];//工作路径
	char m_BoneDir[MAX_PATH];//Bone存放路径
	char m_MeshDir[MAX_PATH];//Mesh存放路径
	char m_TextureDir[MAX_PATH];//Texture存放路径
	//设置
	//[ouput]
	BOOL m_bStaticMesh;					//是否使用Static Mesh
	float m_fZoomRatio;					//缩放
	//[compress]
	BOOL m_bUseVertexCompress;			//是否使用顶点压缩
	BOOL m_bUseAnimationCompress;		//是否使用动画数据压缩
	BOOL m_bUseMaterialCompress;		//是否使用材质关键帧压缩
	//[option]
	BOOL m_bExportUV2;					//是否导出第二套UV
	BOOL m_bExportVertexColor;			//是否导出顶点颜色

	//动作列表
	ActionList m_ActionList;
};
