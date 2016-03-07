#pragma once

class CSingleDocTemplate;
class CModelViewerThread;

#include <vector>

class ExportControlClass
{
	friend class CModelViewerThread;
	ExportControlClass(){};
	~ExportControlClass(){};
	static ExportControlClass* s_Instance;
public:
	std::vector<CModelViewerThread*> m_ThreadVector;
	std::vector<CSingleDocTemplate*> m_DocTemplateVector;
public:
	void CreateThread();
	void DestoryAllThread();
public:
	static ExportControlClass* GetInstance();
	static void ReleaseInstance();
};

extern "C" __declspec(dllexport) void CreateModelViewer(CString strPath="");
extern "C" __declspec(dllexport) void DestroyAllModelViewer();
extern "C" __declspec(dllexport) void OpenModel(CString strPath);
//extern "C" __declspec(dllexport) void SetInitialModel(CString strPath);