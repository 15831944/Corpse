#include "StdAfx.h"
#include "DelayedDirectoryChangeHandler.h"

#include <shlwapi.h>				 // for PathMatchSpec
#pragma comment( lib, "shlwapi.lib") // function


// ASSERT( AfxGetInstanceHandle() == (HINSTANCE)GetModuleHandle(NULL) ); <-- true for building .exe's 
//NOTE: In Dll's using shared MFC, AfxGetInstanceHandle() != (HINSTANCE)GetModuleHandle(NULL)...
//don't know if this is the case for dll's using static MFC

static inline bool IsEmptyString(LPCTSTR sz)
{
	return (bool)(sz==NULL || *sz == 0);
}
/*********************************************************
  PathMatchSpec() requires IE 4.0 or greater on NT...
  if running on NT 4.0 w/ out IE 4.0, then uses this function instead.

  Based on code by Jack Handy:
  http://www.codeproject.com/string/wildcmp.asp

  Changed slightly to match the PathMatchSpec signature, be unicode compliant & to ignore case by myself.
  
*********************************************************/

#define _TESTING_WILDCMP_ONLY_ 

BOOL STDAPICALLTYPE wildcmp(LPCTSTR string, LPCTSTR wild ) 
{
	const TCHAR *cp, *mp;
	cp = mp = NULL;
	
	while ((*string) && (*wild != _T('*'))) 
	{
		if ((_toupper(*wild) != _toupper(*string)) && (*wild != _T('?'))) 
		{
			return FALSE;
		}
		wild++;
		string++;
	}
		
	while (*string) 
	{
		if (*wild == _T('*')) 
		{
			if (!*++wild) 
			{
				return TRUE;
			}
			mp = wild;
			cp = string+1;
		} 
		else 
		if ((_toupper(*wild) == _toupper(*string)) || (*wild == _T('?'))) 
		{
			wild++;
			string++;
		} 
		else 
		{
			wild = mp;
			string = cp++;
		}
	}
		
	while (*wild == _T('*')) 
	{
		wild++;
	}
	return (!*wild)? TRUE : FALSE;
}

///////////////////////////////////////////////////////
//static member data for CDelayedDirectoryChangeHandler
HINSTANCE CDelayedDirectoryChangeHandler::s_hShlwapi_dll = NULL;//for the PathMatchSpec() function
BOOL CDelayedDirectoryChangeHandler::s_bShlwapi_dllExists = TRUE;
long CDelayedDirectoryChangeHandler::s_nRefCnt_hShlwapi = 0L;
FUNC_PatternMatchSpec CDelayedDirectoryChangeHandler::s_fpPatternMatchSpec = wildcmp;//default
///////////////////////////////////////////////////////
//construction destruction
CDelayedDirectoryChangeHandler::CDelayedDirectoryChangeHandler(CDirectoryChangeHandler * pRealHandler, BOOL bAppHasGUI, LPCTSTR szIncludeFilter, LPCTSTR szExcludeFilter, DWORD dwFilterFlags)
	: m_pDelayNotifier( NULL )
	 ,m_pRealHandler( pRealHandler )
	 ,m_szIncludeFilter(NULL)
	 ,m_szExcludeFilter(NULL)
	 ,m_dwFilterFlags( dwFilterFlags )
	 ,m_dwPartialPathOffset( 0UL )
	 ,m_hWatchStoppedDispatchedEvent(NULL)
	 ,m_nNumIncludeFilterSpecs(0)
	 ,m_nNumExcludeFilterSpecs(0)
{
	ASSERT( m_pRealHandler ); 

	InitializePathMatchFunc( szIncludeFilter, szExcludeFilter );

	m_hWatchStoppedDispatchedEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);//AUTO-RESET, not initially signalled
	ASSERT( m_hWatchStoppedDispatchedEvent );
	
	if (bAppHasGUI)
	{
		m_pDelayNotifier = new CDelayedNotificationWindow();
	}
	else
	{
		m_pDelayNotifier = new CDelayedNotificationThread();
	}
}

CDelayedDirectoryChangeHandler::~CDelayedDirectoryChangeHandler()
{
	if (m_pRealHandler)
	{
		delete m_pRealHandler;
		m_pRealHandler = NULL;
	}

	if (m_pDelayNotifier)
	{
		delete m_pDelayNotifier;
		m_pDelayNotifier = NULL;
	}

	if (m_hWatchStoppedDispatchedEvent)
	{
		CloseHandle(m_hWatchStoppedDispatchedEvent);
		m_hWatchStoppedDispatchedEvent = NULL;
	}

	if (m_szIncludeFilter)
	{
		if (m_nNumIncludeFilterSpecs == 1)
			free(m_szIncludeFilter);
		else
		{
			TCHAR ** ppTmp = (TCHAR**)m_szIncludeFilter;
			for(int i(0); i < m_nNumIncludeFilterSpecs; ++i)
			{
				free( *ppTmp++ );
			}
			free( m_szIncludeFilter );
		}
		m_szIncludeFilter = NULL;
		m_nNumIncludeFilterSpecs;
	}

	if (m_szExcludeFilter)
	{
		if( m_nNumExcludeFilterSpecs == 1 )
			free(m_szExcludeFilter);
		else
		{
			TCHAR ** ppTmp = (TCHAR**)m_szExcludeFilter;
			for(int i(0); i < m_nNumExcludeFilterSpecs; ++i)
			{
				free( *ppTmp++ );
			}
			free( m_szExcludeFilter );
		}
		m_szExcludeFilter = NULL;
		m_nNumExcludeFilterSpecs = 0;
	}

	UninitializePathMatchFunc();
}

BOOL CDelayedDirectoryChangeHandler::_PathMatchSpec(LPCTSTR szPath, LPCTSTR szPattern)
{
	if( s_fpPatternMatchSpec )
	{
		return s_fpPatternMatchSpec(szPath, szPattern);
	}
	ASSERT( FALSE );
	return TRUE;//everything matches.
}

BOOL CDelayedDirectoryChangeHandler::InitializePathMatchFunc(LPCTSTR szIncludeFilter, LPCTSTR szExcludeFilter)
//
//
//	To support the Include and Exclude filters, the PathMatchSpec function is used.
//	PathMatchSpec is only supported on NT4.0 if IE 4.0 is installed.
//
//	for the case where this code is running on NT 4.0 w/out IE 4.0, we use
//	a different function: wildcmp ()
//
//
//	This function attempts to load shlwapi.dll dynamically and find the PathMatchSpec function.
//
//	if the function PathMatchSpec can't be found, the function pointer s_fpPathMatchSpec is set to wildcmp.
//
//
//	Note:  wildcmp doesn't support multiple file specs separated by a semi-colon
//	as PathMatchSpec does.... we'll support it by parsing them 
//	when we want to test the filters, we'll call the pattern matching functions multiple times...
//
{

	//
	//	Copy the include/exclude filters if specified...
	//
	//

	//both the include && exclude filters aren't specified
	//no need to initialize the pattern matching function....
	//if filters are never used, then
	//one less dll is loaded.
	if (IsEmptyString(szIncludeFilter) &&	IsEmptyString(szExcludeFilter))
	{
		return TRUE;
	}

#ifdef _TESTING_WILDCMP_ONLY_
	s_hShlwapi_dll = NULL;
	s_bShlwapi_dllExists = FALSE;
	return InitializePatterns(szIncludeFilter, szExcludeFilter);
#endif


	if( s_hShlwapi_dll != NULL )
	{
		ASSERT( s_fpPatternMatchSpec != NULL );
		InterlockedIncrement(&s_nRefCnt_hShlwapi);

		return InitializePatterns(szIncludeFilter, szExcludeFilter);
	}
	else
	{
		if( s_bShlwapi_dllExists == TRUE )//either the dll exists, or we haven't tried loading it yet...
		{
			//The pattern match function hasn't been initialized yet....
			//
		
			s_hShlwapi_dll = ::LoadLibrary(_T("Shlwapi.dll"));
			if( s_hShlwapi_dll == NULL )
			{
				s_bShlwapi_dllExists = FALSE;//don't try loading this dll again.
				s_fpPatternMatchSpec = wildcmp;//even though it's set buy default, and this code will only get here once, set it just for fun.

				return InitializePatterns(szIncludeFilter, szExcludeFilter);
				
			}
			else
			{
				//Shlwapi.dll was found....check it for PathMatchSpec()
#ifdef UNICODE 
				s_fpPatternMatchSpec = (FUNC_PatternMatchSpec)::GetProcAddress(s_hShlwapi_dll, "PathMatchSpecW");
#else
				s_fpPatternMatchSpec = (FUNC_PatternMatchSpec)::GetProcAddress(s_hShlwapi_dll, "PathMatchSpecA");
#endif
	
				if( s_fpPatternMatchSpec != NULL )
				{
					//UsesRealPathMatchSpec() will now return true.
					//we're on NT w/ IE 4.0 or greater...(or Win2k/XP)
					InterlockedIncrement(&s_nRefCnt_hShlwapi);
					return InitializePatterns(szIncludeFilter, szExcludeFilter);
				}
				else
				{
					//we found shlwapi.dll, but it didn't have PathMatchSpec()
					::FreeLibrary( s_hShlwapi_dll );
					s_hShlwapi_dll = NULL;
					s_bShlwapi_dllExists = FALSE;

					//instead of using PathMatchSpec()
					//we'll use wildcmp()
					s_fpPatternMatchSpec = wildcmp;
					//UsesRealPathMatchSpec() will now return false w/out asserting.

					return InitializePatterns(szIncludeFilter, szExcludeFilter);
				}
			}
			
		}
	}
	return (s_fpPatternMatchSpec != NULL);
}

BOOL CDelayedDirectoryChangeHandler::InitializePatterns(LPCTSTR szIncludeFilter, LPCTSTR szExcludeFilter)
{
	ASSERT( !IsEmptyString(szIncludeFilter)   //one of these must have something in it, 
		||  !IsEmptyString(szExcludeFilter) );//or else this function shouldn't be called.

	if( s_hShlwapi_dll != NULL )
	{
		//we're using Shlwapi.dll's PathMatchSpec function....
		//we're running on NT4.0 w/ IE 4.0 installed, or win2k/winXP(or greater)
		//
		//	Copy the include/exclude filters if specified...
		//
		//
		// we're using the real PathMatchSpec() function which
		//	supports multiple pattern specs...(separated by a semi-colon)
		//	so there's only one filter spec as far as my code is concerned.
		//
		if( !IsEmptyString(szIncludeFilter) )
		{
			m_szIncludeFilter = _tcsdup(szIncludeFilter);
			ASSERT( m_szIncludeFilter );
			m_nNumIncludeFilterSpecs = 1;
		}
		if( !IsEmptyString(szExcludeFilter) )
		{
			m_szExcludeFilter = _tcsdup(szExcludeFilter);	
			ASSERT( m_szExcludeFilter );
			m_nNumExcludeFilterSpecs = 1;
		}	
	}
	else
	{
		//shlwapi.dll isn't on this machine.... can happen on NT4.0 w/ out IE 4.0 installed.
		ASSERT( s_bShlwapi_dllExists == FALSE );

		//
		//	we're using the function wildcmp() instead of PathMatchSpec..
		//
		//	this means that multiple pattern specs aren't supported...
		// in order to support them, we'll tokenize the string into multiple
		// pattern specs and test the string multiple times(once per pattern spec) 
		// in order to support multiple patterns.
		//
		//
		//	m_szIncludeFilter & m_szExclude filter will be used like TCHAR**'s instead of TCHAR*'s
		//

		m_nNumIncludeFilterSpecs = 0;
		if( !IsEmptyString(szIncludeFilter) )
		{
			TCHAR * szTmpFilter = _tcsdup(szIncludeFilter);
			TCHAR * pTok = _tcstok( szTmpFilter, _T(";"));
			while(pTok)
			{
				m_nNumIncludeFilterSpecs++;
				pTok = _tcstok(NULL, _T(";"));
			}
			if( m_nNumIncludeFilterSpecs == 1 )
				m_szIncludeFilter = _tcsdup(szIncludeFilter);
			else
			{   //allocate room for pointers .. one for each token...
				m_szIncludeFilter = (TCHAR*)malloc( m_nNumIncludeFilterSpecs * sizeof(TCHAR*));

				free(szTmpFilter);
				szTmpFilter = _tcsdup(szIncludeFilter);
				pTok = _tcstok(szTmpFilter, _T(";"));
				TCHAR ** ppTmp = (TCHAR**)m_szIncludeFilter;
				while(pTok)
				{
					*ppTmp = _tcsdup(pTok);
					ppTmp++;
					pTok = _tcstok(NULL, _T(";"));
				}
			}

			free(szTmpFilter);
		}

		//
		//	Do the same for the Exclude filter...
		//
		m_nNumExcludeFilterSpecs = 0;
		if( !IsEmptyString(szExcludeFilter) )
		{
			TCHAR * szTmpFilter = _tcsdup(szExcludeFilter);
			TCHAR * pTok = _tcstok( szTmpFilter, _T(";"));
			while( pTok )
			{
				m_nNumExcludeFilterSpecs++;
				pTok = _tcstok(NULL, _T(";"));
			}
			if( m_nNumExcludeFilterSpecs == 1 )
				m_szExcludeFilter = _tcsdup(szExcludeFilter);
			else
			{   //allocate room for pointers .. one for each token...
				m_szExcludeFilter = (TCHAR*)malloc( m_nNumExcludeFilterSpecs * sizeof(TCHAR*));

				free(szTmpFilter);
				szTmpFilter = _tcsdup(szExcludeFilter);

				pTok = _tcstok(szTmpFilter, _T(";"));
				TCHAR ** ppTmp = (TCHAR**)m_szExcludeFilter;
				while(pTok)
				{
					*ppTmp = _tcsdup(pTok);
					ppTmp++;
					pTok = _tcstok(NULL, _T(";"));
				}
			}
			free(szTmpFilter);
		}
	}

	return (m_szExcludeFilter!= NULL || (m_szIncludeFilter != NULL));
}
void CDelayedDirectoryChangeHandler::UninitializePathMatchFunc()
{
	if( s_bShlwapi_dllExists == TRUE 
	&&  s_hShlwapi_dll != NULL )
	{
		if( InterlockedDecrement(&s_nRefCnt_hShlwapi) <= 0)
		{
			s_nRefCnt_hShlwapi = 0;
			FreeLibrary( s_hShlwapi_dll );
			s_hShlwapi_dll = NULL;
			s_fpPatternMatchSpec = wildcmp;
		}
	}
}

//are we using PathMatchSpec() or wildcmp()?
BOOL CDelayedDirectoryChangeHandler::UsesRealPathMatchSpec() const
{
	if( s_hShlwapi_dll != NULL && s_fpPatternMatchSpec != NULL )
		return TRUE;
	if( s_hShlwapi_dll == NULL && s_fpPatternMatchSpec != NULL )
		return FALSE;

	ASSERT(FALSE);//this function was called before InitializePathMatchFunc()
	//oops!
	return FALSE;
}

static inline BOOL HasTrailingBackslash(const CString & str )
{
	if (str.GetLength() > 0 &&	str[ str.GetLength() - 1 ] == _T('\\'))
		return TRUE;
	return FALSE;
}

void CDelayedDirectoryChangeHandler::SetPartialPathOffset(const CString & strWatchedDirName)
{
	if( m_dwFilterFlags & FILTERS_CHECK_PARTIAL_PATH )
	{
		//set the offset to 
		if (HasTrailingBackslash(strWatchedDirName))
			m_dwPartialPathOffset = strWatchedDirName.GetLength();
		else
			m_dwPartialPathOffset = strWatchedDirName.GetLength() + 1;
	}
	else
		m_dwPartialPathOffset = 0;
}

//
//	Maybe in future I'll keep a pool of these 
//	objects around to increase performance...
//	using objects from a cache will be faster 
//	than allocated and destroying a new one each time.
//	
//  
CDirChangeNotification * CDelayedDirectoryChangeHandler::GetNotificationObject()
{
	ASSERT( m_pRealHandler );
	return new CDirChangeNotification(this, m_dwPartialPathOffset);//helps support FILTERS_CHECK_PARTIAL_PATH
}

void CDelayedDirectoryChangeHandler::DisposeOfNotification(CDirChangeNotification * pNotification)
{
	delete pNotification;
}

//These functions are called when the directory to watch has had a change made to it
void CDelayedDirectoryChangeHandler::OnFileAdded(LPCTSTR lpszFileName)
{
	CDirChangeNotification * pNotify = GetNotificationObject();
	ASSERT(pNotify);
	if(pNotify) pNotify->PostOn_FileAdded(lpszFileName);
}

void CDelayedDirectoryChangeHandler::OnFileRemoved(LPCTSTR lpszFileName)
{
	CDirChangeNotification * pNotify = GetNotificationObject();
	ASSERT(pNotify) ;
	if(pNotify) pNotify->PostOn_FileRemoved(lpszFileName);
}

void CDelayedDirectoryChangeHandler::OnFileModified(LPCTSTR lpszFileName)
{
	CDirChangeNotification * pNotify = GetNotificationObject();
	ASSERT(pNotify);
	if(pNotify) pNotify->PostOn_FileModified(lpszFileName);
}

void CDelayedDirectoryChangeHandler::OnFileNameChanged(LPCTSTR lpszOldFileName, LPCTSTR lpszNewFileName)
{
	CDirChangeNotification * pNotify = GetNotificationObject();	
	ASSERT(pNotify);
	if(pNotify) pNotify->PostOn_FileNameChanged(lpszOldFileName, lpszNewFileName);
}

void CDelayedDirectoryChangeHandler::OnReadDirectoryChangesError(DWORD dwError, LPCTSTR lpszNewFileName)
{
	CDirChangeNotification * pNotify = GetNotificationObject();
	ASSERT(pNotify);
	if(pNotify) pNotify->PostOn_ReadDirectoryChangesError(dwError, lpszNewFileName);
}

void CDelayedDirectoryChangeHandler::OnWatchStarted(DWORD dwError, const CString & strDirectoryName)
{
	if( !(m_dwFilterFlags & FILTERS_NO_WATCHSTART_NOTIFICATION))
	{
		CDirChangeNotification * pNotify = GetNotificationObject();

		if(pNotify) pNotify->PostOn_WatchStarted(dwError, strDirectoryName);
	}
}

void CDelayedDirectoryChangeHandler::OnWatchStopped(const CString & strDirectoryName)
{
	if( !(m_dwFilterFlags & FILTERS_NO_WATCHSTOP_NOTIFICATION))
	{
		CDirChangeNotification * pNotify = GetNotificationObject();

		if (pNotify){
			if(m_hWatchStoppedDispatchedEvent)
				::ResetEvent(m_hWatchStoppedDispatchedEvent);

			pNotify->PostOn_WatchStopped( strDirectoryName );

			//	Wait that this function has been dispatched to the other thread
			//	before continueing.  This object may be getting deleted
			//	soon after this function returns, and before the function can be
			//	dispatched to the other thread....
			WaitForOnWatchStoppedDispatched();
		}
	}
}


void CDelayedDirectoryChangeHandler::PostNotification(CDirChangeNotification * pNotification)
{
	if (m_pDelayNotifier)
		m_pDelayNotifier->PostNotification(pNotification);
}

// Helper function
//	For filtering events..... these functions can not be filtered out.
//
inline BOOL IsNonFilterableEvent(FUNCTION_TO_DISPATCH eEvent)
{
	if (eEvent == eOn_WatchStarted ||	eEvent == eOn_WatchStopped ||	eEvent == eOn_ReadDirectoryChangesError)
		return TRUE;
	else
		return FALSE;
}

DWORD GetPathOffsetBasedOnFilterFlags(CDirChangeNotification * pNot, DWORD dwFilterFlags)
{
	ASSERT (pNot && dwFilterFlags != 0);
	//helps support the filter options FILTERS_CHECK_FULL_PATH, FILTERS_CHECK_PARTIAL_PATH, and FILTERS_CHECK_FILE_NAME_ONLY

	DWORD dwFileNameOffset = 0;//offset needed to support FILTERS_CHECK_FULL_PATH
	if( dwFilterFlags & FILTERS_CHECK_FILE_NAME_ONLY )
	{
		//set the offset to support FILTERS_CHECK_FILE_NAME_ONLY
		TCHAR * pSlash  = _tcsrchr(pNot->m_szFileName1, _T('\\'));
		if( pSlash )
			dwFileNameOffset = (++pSlash - pNot->m_szFileName1);

		//
		//	Because file name change notifications take place in the same directory,
		//	the same dwFileNameOffset can be used for the szNewFileName(pNot->m_szFileName2)
		//	when checking the filter against the new file name.
		//
	}
	else
	if( dwFilterFlags & FILTERS_CHECK_PARTIAL_PATH)
	{
		//
		//	partial path offset is the offset 
		//	from the beginning of the file name, 
		//	to the end of the watched directory path...
		//	ie: If you're watching "C:\Temp"
		//		and the file C:\Temp\SubFolder\FileName.txt" is changed,
		//		the partial path offset will give you "SubFolder\FileName.txt"
		//		when this is checked against the include/exclude filter.
		//
		dwFileNameOffset = pNot->m_dwPartialPathOffset;
	}
	//else
	//if( m_dwFilterFlags & CDirectoryChangeWatcher::FILTERS_CHECK_FULL_PATH )
	//	dwFileNameOffset = 0;
	
	return dwFileNameOffset;
}

//
//
//	Perform the tests to see if the client wants to be notified of this
//	file change notification.
//
//	Tests performed:
//
//	Event test:		Not all events can be filtered out.
//					On_ReadDirectoryChangesError
//					cannot be filtered out.
//	Filter flags test:  User can specify flags so that no tests are performed....all notifications are sent to the user.
//
//	Filter test:	Test the notification file name against include and exclude filters.
//
//					Only files changes matching the INCLUDE filter will be passed to the client.
//					By not specifying an include filter, all file changes are passed to the client.
//	
//				    Any files matching the EXCLUDE filter will not be passed to the client.
//
//
//					Note: For the file name change event:
//							If the old file name does not pass the tests for the include and exclude filter
//							but the NEW file name does pass the test for the filters, then the client IS notified.
//
//	Client test:	The CDirectoryChangeHandler derived class is given a chance to filter the event by calling
//					CDirectoryChangeHandler::On_FilterNotification()
//
//	RETURN VALUE:
//	If this function returns true, the notification function is called.
//	If it returns false, the notification is ignored.
//			The client is notified by calling CDirectoryChangeHandler's virtual functions On_FileAdded(),On_FileRemoved(), etc.

BOOL CDelayedDirectoryChangeHandler::NotifyClientOfFileChange(CDirChangeNotification * pNot)
{
	ASSERT(pNot);
	ASSERT(m_pRealHandler);

	//
	//	Some events can't be ignored, or filtered out.
	//

	// Either this is a non-filterable event, or we're not using any filters...
	// client is notified of all events..
	if (((m_dwFilterFlags & FILTERS_DONT_USE_ANY_FILTER_TESTS) == FILTERS_DONT_USE_ANY_FILTER_TESTS) || IsNonFilterableEvent( pNot->m_eFunctionToDispatch))
	{
		return TRUE;
	}

	//
	//	See if user wants to test CDirectoryChangeHandler::On_FilterNotification()
	//	before tests are performed against the file name, and filter specifications
	//
	if( (m_dwFilterFlags & FILTERS_TEST_HANDLER_FIRST )//specified that CDirectoryChangeHandler::On_FilterNotification is to be called before any other filter tests
	&&	!(m_dwFilterFlags & FILTERS_DONT_USE_HANDLER_FILTER)//and did not specify that this CDirectoryChangeHandler::On_FilterNotification is not to be called..
	&&	!m_pRealHandler->OnFilterNotification(pNot->m_eFunctionToDispatch, pNot->m_szFileName1, pNot->m_szFileName2) )
	{
		//
		//	Client specified to test handler first, and it didn't pass the test... don't notify the client.
		//
		return FALSE;
	}
	//else
	//
	//	this file change passed the user test, continue testing
	//	to see if it passes the filter tests.

	DWORD dwFileNameOffset = GetPathOffsetBasedOnFilterFlags(pNot, m_dwFilterFlags);

	//
	//	See if the changed file matches the include or exclude filter
	//	Only allow notifications for included files 
	//	that have not been exluded.
	//
	if(!(m_dwFilterFlags & FILTERS_DONT_USE_FILTERS) )
	{
		if (FALSE == IncludeThisNotification(pNot->m_szFileName1 + dwFileNameOffset) ||	FALSE != ExcludeThisNotification(pNot->m_szFileName1 + dwFileNameOffset) )
		{
			if( pNot->m_eFunctionToDispatch != eOn_FileNameChanged )
				return FALSE;
			else{
				//Special case for file name change:
				//
				// the old file name didn't pass the include/exclude filter
				// but if the new name passes the include/exclude filter, 
				// we will pass it on to the client...

				if (FALSE == IncludeThisNotification(pNot->m_szFileName2 + dwFileNameOffset) ||	 FALSE != ExcludeThisNotification(pNot->m_szFileName2 + dwFileNameOffset) )
				{
					// the new file name didn't pass the include/exclude filter test either
					// so don't pass the notification on...
					return FALSE;
				}
			}
		}
	}
	
	//
	//	Finally, let the client determine whether or not it wants this file notification
	//	if this test has not already been performed...
	//

	if( (m_dwFilterFlags & FILTERS_TEST_HANDLER_FIRST) || (m_dwFilterFlags & FILTERS_DONT_USE_HANDLER_FILTER) )
	{
		//	if we got this far, and this flag was specified, 
		//	it's already passed this test,
		//  or we're not checking it based on the filter flag FILTERS_DONT_USE_HANDLER_FILTER....
		return TRUE;
	}
	else
	{
		if( m_pRealHandler->OnFilterNotification(pNot->m_eFunctionToDispatch, pNot->m_szFileName1, pNot->m_szFileName2))
		{
			return TRUE;
		}
		else
		{
			//else  client's derived CDirectoryChangeHandler class chose 
			//		not to be notified of this file change
			return FALSE;
		}
	}
}

//
//	The Include filter specifies which file names we should allow to notifications
//	for... otherwise these notifications are not dispatched to the client's code.
//
//	Tests the file name to see if it matches a filter specification
//
//	RETURN VALUES:
//		
//		true : notifications for this file are to be included...
//			   notifiy the client by calling the appropriate CDirectoryChangeHandler::On_Filexxx() function.
//		false: this file is not included.... do not notifiy the client...
//
BOOL CDelayedDirectoryChangeHandler::IncludeThisNotification(LPCTSTR szFileName)
{
	ASSERT( szFileName );

	if( m_szIncludeFilter == NULL ) // no filter specified, all files pass....
		return TRUE;
	if( m_nNumIncludeFilterSpecs == 1 )
	{
		return _PathMatchSpec(szFileName, m_szIncludeFilter) ? TRUE : FALSE;
	}
	else
	{
		TCHAR ** ppTmp = (TCHAR**)m_szIncludeFilter;
		for(int i(0); i < m_nNumIncludeFilterSpecs; ++i )
		{
			if( _PathMatchSpec(szFileName, *ppTmp++) )
				return TRUE;
		}
		return FALSE;
	}

	return FALSE;
}

//
//	Tests the file name to see if it matches a filter specification
//	if this function returns true, it means that this notification
//	is NOT to be passed to the client.... changes to this kind of file
//	are not
//
//	RETURN VALUES:
//		
//		true :   notifications for this file are to be filtered out(EXCLUDED)...
//				 do not notifify the client code.
//		false:   notifications for this file are NOT to be filtered out
//
//
BOOL CDelayedDirectoryChangeHandler::ExcludeThisNotification(LPCTSTR szFileName)
{

	ASSERT( szFileName );

	if( m_szExcludeFilter == NULL ) // no exclude filter... nothing is excluded...
		return FALSE;
	if( m_nNumExcludeFilterSpecs == 1 )
	{
		if( _PathMatchSpec(szFileName, m_szExcludeFilter) )
			return TRUE;//exclude this notification...
		return FALSE;
	}
	else
	{
		TCHAR ** ppTmp = (TCHAR**)m_szExcludeFilter;
		for(int i(0); i < m_nNumExcludeFilterSpecs; ++i )
		{
			if( _PathMatchSpec(szFileName, *ppTmp++) )
				return TRUE;//exclude this one...
		}
		return FALSE;//didn't match any exclude filters...don't exclude it
	}
}

/*****************************************************
	This function is called when we want the notification to execute.

	
******************************************************/
void CDelayedDirectoryChangeHandler::DispatchNotificationFunction(CDirChangeNotification * pNotification)
{
	ASSERT( m_pRealHandler );
	ASSERT( pNotification );
	if( pNotification && m_pRealHandler )
	{
		if(NotifyClientOfFileChange(pNotification))
		{
			switch( pNotification->m_eFunctionToDispatch )
			{
			case eOn_FileAdded:
				m_pRealHandler->OnFileAdded( pNotification->m_szFileName1 ); 
				break;
			case eOn_FileRemoved:
				m_pRealHandler->OnFileRemoved( pNotification->m_szFileName1 );
				break;
			case eOn_FileNameChanged:
				m_pRealHandler->OnFileNameChanged( pNotification->m_szFileName1, pNotification->m_szFileName2 );
				break;
			case eOn_FileModified:
				m_pRealHandler->OnFileModified( pNotification->m_szFileName1 );
				break;
			case eOn_ReadDirectoryChangesError:
				m_pRealHandler->OnReadDirectoryChangesError( pNotification->m_dwError, pNotification->m_szFileName1 );
				break;
			case eOn_WatchStarted:
				m_pRealHandler->OnWatchStarted(pNotification->m_dwError, pNotification->m_szFileName1);
				break;
			case eOn_WatchStopped:
				try{
					//
					//	The exception handler is just in case of the condition described in DirectoryChanges.h
					//	in the comments for On_WatchStopped()
					//
					m_pRealHandler->OnWatchStopped(pNotification->m_szFileName1);

				}catch(...){
					MessageBeep( 0xffff );
					MessageBeep( 0xffff );
			#ifdef DEBUG
					MessageBox(NULL,_T("An RTFM Exception was raised in On_WatchStopped() -- see Comments for CDirectoryChangeHandler::On_WatchStopped() in DirectoryChanges.h."), _T("Programmer Note(DEBUG INFO):"), MB_ICONEXCLAMATION | MB_OK);
			#endif
				}
				//
				//	Signal that the On_WatchStopped() function has been dispatched.
				//
				if( m_hWatchStoppedDispatchedEvent )
					SetEvent(m_hWatchStoppedDispatchedEvent);
				break;
			case eFunctionNotDefined:
			default:
				break;
			}//end switch()
		}
	}	
	if( pNotification )						 //		
		DisposeOfNotification(pNotification);// deletes or releases the notification object from memory/use
											 //
}

//
//	When shutting down, m_pRealHandler->On_WatchStopped() will be called.
//	Because it's possible that this object will be deleted before the notification
//	can be dispatched to the other thread, we have to wait until we know that it's been executed
//	before returning control.
//
//	This function signals that the function has been dispatched to the other
//	thread and it will be safe to delete this object once this has returned.
//
BOOL CDelayedDirectoryChangeHandler::WaitForOnWatchStoppedDispatched( )
{
	ASSERT( m_hWatchStoppedDispatchedEvent );
	DWORD dwWait = WAIT_FAILED;
	if( m_hWatchStoppedDispatchedEvent )
	{

		if( m_bAppHasGUI == FALSE )
		{
			//
			//	The function will be dispatched to another thread...
			//	just wait for the event to be signalled....
			do{
				dwWait	= WaitForSingleObject(m_hWatchStoppedDispatchedEvent, 5000);//wait five seconds
				if( dwWait != WAIT_OBJECT_0 )
				{
					TRACE(_T("WARNING: Possible Deadlock detected! ThreadID: %d File: %s Line: %d\n"), GetCurrentThreadId(), _T(__FILE__), __LINE__);
				}
			}while( dwWait != WAIT_OBJECT_0 );
		}
		else
		{
			//
			//	Note to self:  This thread doesn't have a message Q, and therefore can't attach to 
			//	receive messages and process them... MsgWaitForMultipleObjects won't wake up for messages 
			//	unless i attach myself the the other threads input Q....
			//	just use MsgWaitForMultipleObjects() in place of WaitForSingleObject in the places where it's used...
			//
			do{
				dwWait = MsgWaitForMultipleObjects(1, &m_hWatchStoppedDispatchedEvent, 
												   FALSE, 5000, 
												   QS_ALLEVENTS);//wake up for all events, sent messages, posted messages etc.
				switch(dwWait)
				{
				case WAIT_OBJECT_0:
					{
						//
						// The event has become signalled
						//
						
					}break;
				case WAIT_OBJECT_0 + 1: 
					{
						//
						//	There is a message in this thread's queue, so 
						//	MsgWaitForMultipleObjects returned.
						//	Process those messages, and wait again.

						MSG msg;
						while( PeekMessage(&msg, NULL, 0,0, PM_REMOVE ) ) 
						{
							if( msg.message != WM_QUIT)
							{
								TranslateMessage(&msg);
								DispatchMessage(&msg);
							}
							else
							{
								/****
								NOTE: putting WM_QUIT back in the Q caused problems. forget about it.
								****/
								break;
							}
						}
					}break;
				case WAIT_TIMEOUT:
					{
						TRACE(_T("WARNING: Possible Deadlock detected! ThreadID: %d File: %s Line: %d\n"), GetCurrentThreadId(), _T(__FILE__), __LINE__);
					}break;
				}
			}while( dwWait != WAIT_OBJECT_0 );
			ASSERT( dwWait == WAIT_OBJECT_0 );
		}

	}
	else
	{
		TRACE(_T("WARNING: Unable to wait for notification that the On_WatchStopped function has been dispatched to another thread.\n"));
		TRACE(_T("An Exception may occur shortly.\n"));
		TRACE(_T("File: %s Line: %d"), _T( __FILE__ ), __LINE__);
	
	}


	return (dwWait == WAIT_OBJECT_0 );
}

void CDelayedDirectoryChangeHandler::SetChangedDirectoryName(const CString & strChangedDirName)
{
	ASSERT(m_pRealHandler);
	CDirectoryChangeHandler::SetChangedDirectoryName(strChangedDirName);
	if(m_pRealHandler)
		m_pRealHandler->SetChangedDirectoryName( strChangedDirName );
}

const CString & CDelayedDirectoryChangeHandler::GetChangedDirectoryName() const
{
	if(m_pRealHandler)
		return m_pRealHandler->GetChangedDirectoryName();
	return CDirectoryChangeHandler::GetChangedDirectoryName();
}