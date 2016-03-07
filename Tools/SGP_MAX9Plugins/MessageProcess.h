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

	void SetWorkDir(HWND hwnd);//���ù���·��
	void SetMeshDir(HWND hwnd);//����mesh���·��
	void SetBoneDir(HWND hwnd);//����bone���·��
	void SetTextureDir(HWND hwnd);//����Texture���·��
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
	ͨ���������ݳ�Ա���ɵõ������Ի�����е�����
	*********************************************************/
	//Ŀ¼
	char m_WorkDir[MAX_PATH];//����·��
	char m_BoneDir[MAX_PATH];//Bone���·��
	char m_MeshDir[MAX_PATH];//Mesh���·��
	char m_TextureDir[MAX_PATH];//Texture���·��
	//����
	//[ouput]
	BOOL m_bStaticMesh;					//�Ƿ�ʹ��Static Mesh
	float m_fZoomRatio;					//����
	//[compress]
	BOOL m_bUseVertexCompress;			//�Ƿ�ʹ�ö���ѹ��
	BOOL m_bUseAnimationCompress;		//�Ƿ�ʹ�ö�������ѹ��
	BOOL m_bUseMaterialCompress;		//�Ƿ�ʹ�ò��ʹؼ�֡ѹ��
	//[option]
	BOOL m_bExportUV2;					//�Ƿ񵼳��ڶ���UV
	BOOL m_bExportVertexColor;			//�Ƿ񵼳�������ɫ

	//�����б�
	ActionList m_ActionList;
};
