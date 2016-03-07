#pragma once

#include "CommonObjDef.h"
#include <vector>

namespace sgp
{
	struct ISGPObject;
	struct ISGPLightObject;
}

class SceneObjectManager
{
	SceneObjectManager();
	~SceneObjectManager();
	DECLARE_SINGLETON(SceneObjectManager)
public:
	// Add/Delete
	void AddSceneObject(const std::vector<CommonObject>& objVector);
	void AddSceneObject(const CommonObject& obj);
	void EditorAddSceneObject(const CommonObject& obj);
	void EngineAddSceneObject(const CommonObject& obj);
	void DeleteSceneObject(const std::vector<CommonObject>& objVector);
	void DeleteSceneObject(const CommonObject& obj);
	void EditorDeleteSceneObject(const CommonObject& obj);
	void EngineDeleteSceneObject(const CommonObject& obj);

	sgp::ISGPObject* CreateObject(CString& strFile,float fScale);
	sgp::ISGPLightObject* CreateLightObject();
	void SetObjectConfigSetting(const CommonObject& obj,unsigned int index);
	void SceneObjNameChanged(const CommonObject& obj);

	void MoveSceneObject(const std::vector<CommonObject>& objVector,float dx,float dy,float dz,BOOL bSet);
	void RotateSceneObject(const std::vector<CommonObject>& objVector,float dx,float dy,float dz,BOOL bSet);
	void ScaleSceneObject(const std::vector<CommonObject>& objVector,float fScale,BOOL bSet);

	void MoveSceneObject(const CommonObject& obj,float dx,float dy,float dz,BOOL bSet);//Position
	void RotateSceneObject(const CommonObject& obj,float dx,float dy,float dz,BOOL bSet);// Rotation
	void ScaleSceneObject(const CommonObject& obj,float fScale,BOOL bSet);// Scale
	void SceneObjectSetAlpha(const CommonObject& obj,float fAlpha);// Alpha
	
	void SelectedObjectsChanged(const std::vector<CommonObject>& objVector);

	void DeleteObjectOperation(const std::vector<CommonObject>& objVector);
	void DeleteObjectOperation(const CommonObject& obj);
	void ReloadSceneObject(const std::vector<CommonObject>& objVector);
	void ReloadSceneObject(sgp::ISGPObject* pObj);

	void OpenSceneObject(const CommonObject& obj);
	void ShowSceneObjectAttr(const CommonObject& obj);
	void DeleteSelectedObjOperation();

	CommonObject CloneSceneObject(const CommonObject& obj);

	void FocusObject(const CommonObject& obj,void *pSender);
};