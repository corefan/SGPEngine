
#define SAFE_DELETE(p) if((p)!=NULL){delete (p);(p)=NULL;}
#define SAFE_DELETE_ARRAY(p) if((p)!=NULL){delete [] (p);(p)=NULL;}

#define DECLARE_SINGLETON(classname) \
	private:\
		static classname* s_pInstance;\
	public:\
		static classname* GetInstance();\
		static void ReleaseInstance();

#define IMPLEMENT_SINGLETON(classname) \
	classname* classname::s_pInstance=NULL;\
	classname* classname::GetInstance()\
	{\
		if(s_pInstance==NULL) s_pInstance=new classname;\
		return s_pInstance;\
	}\
	void classname::ReleaseInstance()\
	{\
		SAFE_DELETE(s_pInstance)\
	}

