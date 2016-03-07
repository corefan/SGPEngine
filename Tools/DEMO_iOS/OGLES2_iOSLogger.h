

class PVRShellLogger : public Logger
{
public:
	/*
        @param welcomeMessage   when opened, the logger will write a welcome message to the console,
								along with the current date and time 
	*/
	PVRShellLogger(const String& welcomeMessage, PVRShell* pPVRShell) 
		: m_pPVRShell(pPVRShell)
	{
		String welcome;
		welcome << newLine
				<< "**********************************************************" << newLine
				<< welcomeMessage << newLine
				<< "Log started: " << Time::getCurrentTime().toString (true, true) << newLine;

		logMessage( welcome, ELL_NONE );
	}
	virtual ~PVRShellLogger() {}

	// (implementation of the Logger virtual method)
	void logMessage (const String& message, ESGPLOG_LEVEL ll=ELL_INFORMATION)
	{
		if( ll < m_LogLevel )
			return;

		m_pPVRShell->PVRShellOutputDebug( "%s\n", message.toUTF8().getAddress() );
	}

	void setLogLevel( ESGPLOG_LEVEL _level ) { m_LogLevel = _level; }

private:
	PVRShell*				m_pPVRShell;
	SGP_DECLARE_NON_COPYABLE (PVRShellLogger)
};