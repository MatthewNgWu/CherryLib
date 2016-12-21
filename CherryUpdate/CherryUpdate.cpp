// CherryUpdate.cpp : �ش� DLL�� �ʱ�ȭ ��ƾ�� �����մϴ�.
//

#include "stdafx.h"
#include "CherryUpdate.h"
#include <string>

#include "winver.h"
#pragma comment(lib, "version.lib")

#include "shlwapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCherryUpdate::CCherryUpdate()
{
	Initialize();
}

CCherryUpdate::~CCherryUpdate()
{
	Close();
}

void CCherryUpdate::Initialize()
{
#ifndef USE_MFC_CLASS
	m_hSession = NULL;
#endif
	m_pUpdateProcessThread = NULL;
	m_bStarted = FALSE;
	m_bRunPackage = TRUE;

	m_llReceivedSize = 0;

	m_pDownloadStatusCallback = NULL;
	m_pDownloadCompleteCallback = NULL;
	m_pPatchCompleteCallback = NULL;

	m_pDownloadStatusUserData = NULL;
	m_pDownloadCompleteUserData = NULL;
	m_pPatchCompleteUserData = NULL;
}

BOOL CCherryUpdate::Open(LPCTSTR lpszAgent, LPCTSTR lpszClientVersion, LPCTSTR lpszRootUrl, LPCTSTR lpszUpdateProfileXml, LPCTSTR lpszPatchTargetPath, BOOL bUseUtf8)
{
	// TODO.... ���� ó�� �߰� �ʿ�
#ifdef USE_MFC_CLASS
	CInternetSession::CInternetSession(lpszAgent, 1, INTERNET_OPEN_TYPE_DIRECT);

	if (FALSE == IsOpen())
		return FALSE;
#else
	m_hSession = InternetOpen(lpszAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	if (NULL == m_hSession)
		return FALSE;
#endif
	if (NULL == lpszAgent ||
		NULL == lpszRootUrl ||
		NULL == lpszClientVersion ||
		NULL == lpszUpdateProfileXml ||
		NULL == lpszPatchTargetPath)
		return FALSE;

	// xml ���� Ȯ���� �˻�
	CString strXmlExt = PathFindExtension(lpszUpdateProfileXml);
	if (0 != strXmlExt.CompareNoCase(_T(".xml")))
		return FALSE;

	m_strAgent = lpszAgent;
	m_strClientVersion = lpszClientVersion;
	m_strPatchTargetPath = lpszPatchTargetPath;

	if (FALSE == OpenUpdateProfile(lpszRootUrl, lpszUpdateProfileXml, bUseUtf8))
	{
		Close();

		return FALSE;
	}

	return TRUE;
}

void CCherryUpdate::ParseFileNode(BOOL bPackageNode, XMLNode *pFileListNode, LPCTSTR lpszRootUrl, BOOL bUseUtf8)
{
	if (!pFileListNode)
		throw NULL;

	XMLNode *pFileListChildNode = pFileListNode->FirstChild();
	
	CString strRootUrl(lpszRootUrl);
	CString strSubUrl = CA2W(pFileListNode->ToElement()->Attribute("Root")); // sub root path �ε�
	
	// �����ʿ������� ��� '/' ����
	while (_T('/') == strRootUrl.Right(1))
		strRootUrl = strRootUrl.Left(strRootUrl.GetLength() - 1);

	// Ex. Root="./" or Root="."
	// Sub root�� Root �̸�
	if (TRUE == strSubUrl.IsEmpty() ||
		0 == strSubUrl.Compare(_T(".")) ||
		0 == strSubUrl.Compare(_T("./")))
	{
		m_updateProfile.strRootUrl = strRootUrl;
	}
	// Sub url�� ������ �����ϸ�
	else
	{
		// �ϳ��� �߰�
		strRootUrl += _T('/');

		// ���ʿ������� '/' ����
		while (_T('/') == strSubUrl.Left(1))
			strSubUrl = strSubUrl.Right(strSubUrl.GetLength() - 1);

		// �����ʿ������� '/' ����
		while (_T('/') == strSubUrl.Right(1))
			strSubUrl = strSubUrl.Left(strSubUrl.GetLength() - 1);

		DWORD dwLength = strRootUrl.GetLength() + strSubUrl.GetLength() + 1;
		UrlCombine(strRootUrl, strSubUrl, m_updateProfile.strRootUrl.GetBuffer(dwLength), &dwLength, 0);
		m_updateProfile.strRootUrl.ReleaseBuffer();
	}

	// Root ���� UrlCombine ����� ���� '/' �߰�
	m_updateProfile.strRootUrl += _T('/');

	UINT nCodePage = bUseUtf8 ? CP_UTF8 : CP_ACP;

#ifdef INCLUDE_FILE_SIZE_INFO
	FILE_ATTR fileAttr;
#endif
	CString strFileUrl;

	do
	{
		// ���� ������Ʈ�� ���� Version �Ӽ��� Ȯ���Ͽ� ������Ʈ �� ����� �߸���.
		if (FALSE == bPackageNode)
		{
			CString strFileVersion = CA2W(pFileListChildNode->ToElement()->Attribute("Version"));

			// 2014-10-21 ogoons: ���� ������ ������ ���� �ٿ�ε� ��Ͽ� �߰����� �ʴ´�.
			if (TRUE == strFileVersion.IsEmpty())
				continue;

			// 2014-10-21 ogoons: ���� ���� ������ ���� app ������ ���ų� ���� �����̸� ���� �ٿ�ε� ��Ͽ� �߰����� �ʴ´�.
			if (1 > CompareVersion(m_strClientVersion, strFileVersion))
				continue;
		}

		strFileUrl = CA2W(pFileListChildNode->ToElement()->GetText(), nCodePage);

		while (_T('/') == strFileUrl.Left(1))
			strFileUrl = strFileUrl.Right(strFileUrl.GetLength() - 1);

		DWORD dwLength = m_updateProfile.strRootUrl.GetLength() + strFileUrl.GetLength() + 1;
		UrlCombine(m_updateProfile.strRootUrl, strFileUrl, strFileUrl.GetBuffer(dwLength), &dwLength, 0);
		strFileUrl.ReleaseBuffer();

#ifdef INCLUDE_FILE_SIZE_INFO
		fileAttr.strFile = strFileUrl;
		fileAttr.llSize = _ttol(CA2W(pFileListChildNode->ToElement()->Attribute("Size")));

		m_updateProfile.fileUrlVector.push_back(fileAttr);
#else
		m_updateProfile.fileUrlVector.push_back(strFileUrl);
#endif
		if (NULL != m_hSession)
		{
			HINTERNET hUrl = InternetOpenUrl(m_hSession, strFileUrl, NULL, 0, INTERNET_FLAG_KEEP_CONNECTION, 0);

			if (NULL == hUrl)
				break;

			// ���� ������ ������ �����ϴ� ���ϵ��� �� ������ ��������
			TCHAR szContentInfo[32] = { 0, };
			DWORD dwBufLen = sizeof(szContentInfo);

			HttpQueryInfo(hUrl, HTTP_QUERY_CONTENT_LENGTH, (LPVOID)&szContentInfo, &dwBufLen, 0);

			m_updateProfile.llFileTotalSize += (ULONGLONG)_ttol(szContentInfo);

			InternetCloseHandle(hUrl);
		}
	} 
	while (pFileListChildNode = pFileListChildNode->NextSibling());
}

CHERRY_RET CCherryUpdate::OpenUpdateProfile(LPCTSTR lpszRootUrl, LPCTSTR lpszUpdateProfileXml, BOOL bUseUtf8)
{
	CHERRY_RET cherryRet = CCherryException::ERROR_CHERRY_SUCCESS;

	m_updateProfile.Initialize();

	// ���� ���� �غ�
	CString strTargetRootPath = GetIncomingPath();
	if (FALSE == PathFileExists(strTargetRootPath))
	{
		if (FALSE == CreateDirectory(strTargetRootPath, NULL))
			return CCherryException::ERROR_UPDATE_INCOMING_DIR_CREATE_FAIL;
	}

	// ������ '/' ��� ���� ��
	CString strRootUrl(lpszRootUrl);
	while (_T('/') == strRootUrl.Right(1))
		strRootUrl = strRootUrl.Left(strRootUrl.GetLength() - 1);

	// �ϳ��� �߰�
	strRootUrl += _T('/');

	CString strUpdateProfileXmlUrl;
	DWORD dwLength = strRootUrl.GetLength() + _tcslen(lpszUpdateProfileXml) + 1;
	UrlCombine(strRootUrl, lpszUpdateProfileXml, strUpdateProfileXmlUrl.GetBuffer(dwLength), &dwLength, 0);
	strUpdateProfileXmlUrl.ReleaseBuffer();

	CString strHttpResponse;
	if (FALSE == RequestHttp(strUpdateProfileXmlUrl, FALSE, NULL, strHttpResponse))
		return CCherryException::ERROR_UPDATE_RECEIVE_PROFILE_FAIL;

	if (TRUE == strHttpResponse.IsEmpty())
		return CCherryException::ERROR_UPDATE_NO_CONTEXT_PROFILE;

	if (TRUE == bUseUtf8)
		strHttpResponse = ConvertUtf8ToUnicode(strHttpResponse);

	// xml �Ľ� ����
	// ��Ű�� ������Ʈ ������ �ִٸ� ��Ű���� �켱������ ������Ʈ �Ѵ�.
	/*
	// UpdateProfile.xml ����
	<?xml version="1.0" enconding="UTF-8"?>

	<Update>
		<Package 
			Version="2.0.0.1" 
			Root="/Package" 
			Run="KizONResetPCTool_v0.9.6.0_Setup.exe">
			<File>KizONResetPCTool_v0.9.6.0_Setup.exe</File>
			<File>KizONResetPCTool_v0.9.6.0_Setup.dat</File> // ��Ű���� ���� ������ �ƴ� ���� �ִ�.
		</Package>
		<Partial Version="2.0.0.1" Root="/Files">
			<File Version="1.0.0.0" Size="45568">TransportLayerU.dll</File>
			<File Version="1.0.0.0" Size="25088">TNLogManager08.dll</File>
			<File Version="2.1.0.0" Size="53248">TinyXML2.dll</File>
			<File Version="1.0.0.0" Size="118784">MegaLock.dll</File>
			<File Version="1.0.0.0" Size="297000">KizONResetPCTool.exe</File>
			<File Version="1.0.0.0" Size="57856">DeviceManagerEx08U.dll</File>
			<File Version="1.1.0.0" Size="321026780">vs6ent.zip</File>
		</Partial>
	</Update>
	*/

	USES_CONVERSION;

	tinyxml2::XMLDocument xmlDoc;

	try
	{
		if (XML_SUCCESS != xmlDoc.Parse(W2CA(strHttpResponse)))
			return CCherryException::ERROR_UPDATE_PROFILE_XML_PARSE_FAIL;

		XMLNode *pRootNode = xmlDoc.FirstChildElement("Update");

		if (!pRootNode)
			throw CCherryException::ERROR_UPDATE_PROFILE_XML_PARSE_FAIL;

		// 1. Server ������ �����´�.
		XMLNode *pPackageNode = pRootNode->FirstChildElement("Package");
		
		if (NULL == pPackageNode)
			throw CCherryException::ERROR_UPDATE_PROFILE_XML_PARSE_FAIL;

		CString strServerPackageVersion = CA2W(pPackageNode->ToElement()->Attribute("Version"));

		if (TRUE == strServerPackageVersion.IsEmpty())
			throw CCherryException::ERROR_UPDATE_PROFILE_XML_PARSE_FAIL;

		XMLNode *pPartialNode = pRootNode->FirstChildElement("Partial");

		if (NULL == pPartialNode)
			throw CCherryException::ERROR_UPDATE_PROFILE_XML_PARSE_FAIL;

		CString strServerPartialVersion = CA2W(pPartialNode->ToElement()->Attribute("Version"));

		if (TRUE == strServerPartialVersion.IsEmpty())
			throw CCherryException::ERROR_UPDATE_PROFILE_XML_PARSE_FAIL;

		// 2. ������ �����ϴ� ��Ű�� ������ �� ũ��
		if (1 == CompareVersion(m_strClientVersion, strServerPackageVersion))
		{
			// 3. ��Ű�� ������Ʈ
			m_updateProfile.bPackageUpdate = TRUE;

			// ��Ű�� ��带 �¿��.
			ParseFileNode(TRUE, pPackageNode, lpszRootUrl, bUseUtf8); 

			// ��Ű�� execute�� ���� path�� �����´�.
			CString strRunPackageFilePath(GetIncomingPath());
			CString strRunFile = CA2W(pPackageNode->ToElement()->Attribute("Run")); // ����� ���� �ε�

			if (FALSE == strRunFile.IsEmpty())
			{
				PathAppend(strRunPackageFilePath.GetBuffer(MAX_PATH), strRunFile);
				strRunPackageFilePath.ReleaseBuffer();
			}

			m_updateProfile.strRunPackageFilePath = strRunPackageFilePath;
		}
		// 2. ������ �����ϴ� ���� ���� ������ �� ũ��
		else if (1 == CompareVersion(m_strClientVersion, strServerPartialVersion))
		{
			// 3. ���� ������Ʈ
			m_updateProfile.bPackageUpdate = FALSE;

			// �ļ� ��带 �¿��.
			ParseFileNode(FALSE, pPartialNode, lpszRootUrl, bUseUtf8);
		}
		// 2. �Ѵ� �ƴϸ�
		else
		{
			// 3. ������Ʈ �� ������ ���ڴ�.
			return CCherryException::ERROR_UPDATE_NO_UPDATE_IS_REQUIRED;
		}
	}
	catch (const CHERRY_RET &errorRet)
	{
		cherryRet = errorRet;
	}

	xmlDoc.Clear();

	return cherryRet;
}

///////////////////////////////////////////////////////////////////////////
///
/// \brief		open ���� ���� �ٿ�ε� �� ������Ʈ ���� ���� ������ ����
/// \author		ogoons
/// \date		2014-04-25
/// \param		
/// \return		
/// \remark		
/// \section	
///
///////////////////////////////////////////////////////////////////////////
BOOL CCherryUpdate::StartUpdate()
{
	if (FALSE == IsOpen())
		return FALSE;

	// ���� ����Ʈ�� ������ �������� ������ �������� �ʴ´�.
	if (true == m_updateProfile.fileUrlVector.empty())
		return FALSE;

	m_bStarted = TRUE;
	m_llReceivedSize = 0;

	m_pUpdateProcessThread = new CWinThread(&CCherryUpdate::UpdateProcessThread, this);
	m_pUpdateProcessThread->m_bAutoDelete = FALSE;
	m_pUpdateProcessThread->CreateThread();
	//m_pUpdateProcessThread = AfxBeginThread(UpdateProcessThread, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	//m_pUpdateProcessThread->m_bAutoDelete = FALSE;
	//m_pUpdateProcessThread->ResumeThread();

	return TRUE;
}

BOOL CCherryUpdate::StopUpdate()
{
	if (FALSE == IsOpen())
		return FALSE;

	m_bStarted = FALSE;

	return TRUE;
}

#ifdef USE_MFC_CLASS

///////////////////////////////////////////////////////////////////////////
///
/// \brief		HTTP Post ��� �������� ���� ��û �Լ�
/// \author		ogoons
/// \date		2013-05-08
/// \param		pszUrl		: �������� Url
///				strResult	: �ؽ�Ʈ ������ �������� ��û�� ���� ����
///				bUseUtf8	: TRUE = UTF8, FALSE = ANSI
/// \return		
/// \remark		
/// \section	
///
///////////////////////////////////////////////////////////////////////////
BOOL CCherryUpdate::ReceiveHttpResponse(_In_ LPCTSTR lpszUrl, _Out_ CString &strResponse, _In_ BOOL bUseUtf8)
{
	BOOL bRet = TRUE;

	CHttpFile *pData = NULL;

	try
	{
		pData = (CHttpFile *)CInternetSession::OpenUrl(lpszUrl, 1, INTERNET_FLAG_RELOAD | INTERNET_FLAG_TRANSFER_ASCII);

		if (NULL == pData)
			throw NULL;

		DWORD dwStatus;
		pData->QueryInfoStatusCode(dwStatus);

		if (HTTP_STATUS_OK != dwStatus)
			throw NULL;

		TCHAR *pszBuf = NULL;
		TCHAR szBuf[1024] = { 0, };
		std::string strBuf;

		do
		{
			pszBuf = pData->ReadString(szBuf, 1023); // null ó���� ���� 1024 - 1
			strBuf.apppend((const char *)pszBuf);
		} 
		while (pszBuf);

		USES_CONVERSION;

		strResponse.Empty();
		strResponse = CA2W(strBuf.c_str(), bUseUtf8 ? CP_UTF8 : CP_ACP); // ���������� charset=utf-8�̹Ƿ� unicode�� ����
	}
	catch (CInternetException *e)
	{
		// error
		TCHAR szErr[1024] = { 0, };
		e->GetErrorMessage(szErr, 1023);

		strResult = szErr;

		bRet = FALSE;
		e->Delete();
	}
	catch (...)
	{
		bRet = FALSE;
	}

	if (NULL != pData)
	{
		pData->Close();
		
		delete pData;
		pData = NULL;
	}

	return bRet;
}

///////////////////////////////////////////////////////////////////////////
///
/// \brief		���� �ٿ�ε� �Լ� (MFC ����)
/// \author		ogoons
/// \date		2014-04-22
/// \param		
/// \return		
/// \remark		MFC class ��ü exception ó���� Ȱ���Ͽ� ���ܿ� ����.
/// \section	
///
///////////////////////////////////////////////////////////////////////////
BOOL CCherryUpdate::DownloadFile(_In_ LPCTSTR lpszUrl, _In_ LPCTSTR lpszDownloadPath, _Out_ CString &strDownloadedFullPath)
{
	if (FALSE == IsOpen())
		return FALSE;

	BOOL bRet = TRUE;

	CHttpFile *pData = NULL;
	CFile file;
	CFileException fileException;

	try
	{
		pData = (CHttpFile *)CInternetSession::OpenUrl(lpszUrl, 1, INTERNET_FLAG_RELOAD | INTERNET_FLAG_TRANSFER_BINARY, NULL, 0);

		if (NULL == pData)
			throw NULL;

		DWORD dwStatus;
		pData->QueryInfoStatusCode(dwStatus);

		if (HTTP_STATUS_OK != dwStatus)
			throw NULL;

		CString strDownloadPath(lpszDownloadPath);
		PathAppend(strDownloadPath.GetBuffer(MAX_PATH), PathFindFileName(lpszUrl));
		strDownloadPath.ReleaseBuffer();
		strDownloadedFullPath = strDownloadPath;

		if (FALSE == file.Open(szReceivePath, CFile::modeCreate | CFile::modeWrite, &fileException))
		{
			// error
			TCHAR szErr[1024] = { ,0 };
			fileException.GetErrorMessage(szErr, 1023);

			return FALSE;
		}

		BYTE byBuf[1024] = { 0, };
		DWORD dwReadSize = 0;

		do
		{
			dwReadSize = pData->Read(byBuf, 1023);

			if (dwReadSize != 0)
			{
				file.Write(byBuf, dwReadSize);

				if (TRUE == m_bStarted)
				{
					m_llReceivedSize += static_cast<ULONGLONG>(dwReadSize);

					if (NULL != m_pDownloadStatusCallback)
						m_pDownloadStatusCallback(m_llReceivedSize, m_updateProfile.llFileTotalSize, m_pDownloadStatusViewWnd);
				}
			}
		} 
		while (0 != dwReadSize || FALSE == m_bStarted);

		file.Close();
	}
	catch (CInternetException *e)
	{
		// error
		TCHAR szErr[1024] = { 0, };
		e->GetErrorMessage(szErr, 1023);

		bRet = FALSE;
	}
	catch (...)
	{
		// error
		bRet = FALSE;
	}

	if (NULL != pData)
	{
		pData->Close();
		
		delete pData;
		pData = NULL;
	}

	return bRet;
}
#else
///////////////////////////////////////////////////////////////////////////
///
/// \brief		HTTP �������� ��û �Լ�
/// \author		ogoons
/// \date		2015-08-24
/// \param		pszUrl		: �������� Url
///				bPost		: GET, POST ��� ����
///				lpszPostData: bPost == TRUE �� ��� ������ POST data
///				strResponse	: �ؽ�Ʈ ������ �������� ��û�� ���� text ����
/// \return		���� ����
/// \remark		
/// \section	
///
///////////////////////////////////////////////////////////////////////////
BOOL CCherryUpdate::RequestHttp(_In_ LPCTSTR lpszUrl, _In_ BOOL bPost, _In_ LPCTSTR lpszPostData, _Out_ CString &strResponse)
{
	DWORD dwServiceType = AFX_INET_SERVICE_HTTP;
	CString strServer;
	CString strObject;
	INTERNET_PORT port = INTERNET_INVALID_PORT_NUMBER;

	if (FALSE == AfxParseURL(lpszUrl, dwServiceType, strServer, strObject, port))
		return FALSE;

	if (TRUE == strServer.IsEmpty())
		return FALSE;


	if (INTERNET_INVALID_PORT_NUMBER == port)
	{
		switch (dwServiceType)
		{
		case AFX_INET_SERVICE_HTTP:
			port = INTERNET_DEFAULT_HTTP_PORT;
			break;
		case AFX_INET_SERVICE_HTTPS:
			port = INTERNET_DEFAULT_HTTPS_PORT;
			break;
		default:
			return FALSE; // Empty port
		}
	}

	if (FALSE == IsOpen())
		return FALSE;

	HINTERNET hConnect = InternetConnect(m_hSession, strServer, port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);

	if (NULL == hConnect)
		return FALSE;

	DWORD dwContext = 0;

	if (AFX_INET_SERVICE_HTTPS == dwServiceType)
		dwContext = INTERNET_FLAG_SECURE;

	const TCHAR *pszAccept[] = { _T("*/*"), NULL };

	HINTERNET hRequest = HttpOpenRequest(hConnect, TRUE == bPost ? _T("POST") : _T("GET"), strObject, NULL, NULL, pszAccept, dwContext, 0);

	if (NULL == hRequest)
	{
		InternetCloseHandle(hConnect);

		return FALSE;
	}

	const TCHAR *pszHeaders = _T("Content-Type: application/x-www-form-urlencoded");

	if (FALSE == HttpSendRequest(hRequest, pszHeaders, _tcslen(pszHeaders), TRUE == bPost ? (LPVOID)lpszPostData : NULL, TRUE == bPost ? _tcslen(lpszPostData) : 0))
	{
		InternetCloseHandle(hRequest);
		InternetCloseHandle(hConnect);

		return FALSE;
	}

	DWORD dwReadSize = 1;
	DWORD dwBytesRead = 0;

	while (TRUE == InternetQueryDataAvailable(hRequest, &dwReadSize, 0, 0) && 0 < dwReadSize)
	{
		TCHAR *pszData = new TCHAR[dwReadSize + 1];
		ZeroMemory(pszData, dwReadSize + 1);

		InternetReadFile(hRequest, pszData, dwReadSize, &dwBytesRead);

		// ���� ����
		strResponse += pszData;
		delete[] pszData;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
///
/// \brief		���� �ٿ�ε� �Լ� (Win32 API ����)
/// \author		ogoons
/// \date		2014-04-22
/// \param		
/// \return		
/// \remark		
/// \section	
///
///////////////////////////////////////////////////////////////////////////
BOOL CCherryUpdate::DownloadFile(_In_ LPCTSTR lpszUrl, _In_ LPCTSTR lpszDownloadPath, _Out_ CString &strDownloadedFullPath)
{
	if (FALSE == IsOpen())
		return FALSE;

	HINTERNET hUrl = InternetOpenUrl(m_hSession, lpszUrl, NULL, 0, INTERNET_FLAG_KEEP_CONNECTION, 0);

	if (NULL == hUrl)
		return FALSE;

	CString strDownloadPath(lpszDownloadPath);
	CString strFileName(PathFindFileName(lpszUrl));
	PathAppend(strDownloadPath.GetBuffer(strDownloadPath.GetLength() + strFileName.GetLength() + 1), strFileName);
	strDownloadPath.ReleaseBuffer();
	strDownloadedFullPath = strDownloadPath;

	HANDLE hFile = CreateFile(strDownloadPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (NULL == hFile)
	{
		InternetCloseHandle(hUrl);

		return FALSE;
	}

	DWORD dwReadSize = 1;
	DWORD dwBytesRead = 0;
	DWORD dwWritten = 0;

	while (TRUE == InternetQueryDataAvailable(hUrl, &dwReadSize, 0, 0) && 0 < dwReadSize)
	{
		BYTE *pData = new BYTE[dwReadSize];
		ZeroMemory(pData, dwReadSize);

		InternetReadFile(hUrl, pData, dwReadSize, &dwBytesRead);
		WriteFile(hFile, pData, dwBytesRead, &dwWritten, NULL);

		if (TRUE == m_bStarted)
		{
			m_llReceivedSize += static_cast<ULONGLONG>(dwBytesRead);

			if (NULL != m_pDownloadStatusCallback)
				m_pDownloadStatusCallback(m_llReceivedSize, m_updateProfile.llFileTotalSize, m_pDownloadStatusUserData);
		}

		delete[] pData;
	}

	CloseHandle(hFile);
	InternetCloseHandle(hUrl);

	return TRUE;
}
#endif

void CCherryUpdate::UrlEncode(char *output, char *input)
{
	int opt_inx, ipt_inx;
	for (ipt_inx = 0, opt_inx = 0; input[ipt_inx]; ipt_inx++, opt_inx++)
	{
		int char_val = input[ipt_inx];
		if (char_val < 0) char_val += 256;

		if (char_val <= 0x1F ||
			char_val == 0x7F ||
			char_val >= 0x80 ||
			char_val == ' ' ||
			char_val == '{' ||
			char_val == '}' ||
			char_val == '[' ||
			char_val == ']' ||
			char_val == '|' ||
			char_val == '\\' ||
			char_val == '^' ||
			char_val == '~' ||
			char_val == '`' ||
			char_val == '#' ||
			char_val == ';' ||
			char_val == '/' ||
			char_val == '?' ||
			char_val == '@' ||
			char_val == '=' ||
			char_val == '&')
		{
			output[opt_inx] = '%';

			int UpperBit = char_val / 0x10;


			if (UpperBit >= 0 && UpperBit <= 9)
				output[++opt_inx] = UpperBit + '0';
			else
				output[++opt_inx] = UpperBit + 'A' - 10;

			int LowerBit = char_val % 0x10;
			if (LowerBit >= 0 && LowerBit <= 9)
				output[++opt_inx] = LowerBit + '0';
			else
				output[++opt_inx] = LowerBit + 'A' - 10;
		}
		else
		{
			output[opt_inx] = char_val;
		}
	}
	output[opt_inx] = 0;
}

UINT CCherryUpdate::UpdateProcessThread(LPVOID lpParam)
{
	CCherryUpdate *pThis = (CCherryUpdate *)lpParam;
	const CString strTargetRootPath = pThis->GetIncomingPath();

	// ���� ���� �غ�
	if (FALSE == PathFileExists(strTargetRootPath)) // ���丮 ���� ����
	{
		if (FALSE == CreateDirectory(strTargetRootPath, NULL))
		{
			pThis->m_bStarted = FALSE;

			return 0;
		}
	}

	try
	{
		BOOL bReceiveSuccess = TRUE; // �ٿ�ε� ���� ����

		// ���� �ٿ�ε�
		CString strSourceFileUrl;
#ifdef INCLUDE_FILE_SIZE_INFO
		LONGLONG llFileSize = 0;
#endif
		CString strTargetPath;
		CString strSubPath;
		CString strDownloadedFullPath;
		vector<CString> receivedFiles;

#ifdef INCLUDE_FILE_SIZE_INFO
		for (vector<FILE_ATTR>::iterator it = pThis->m_updateProfile.fileUrlVector.begin(); it != pThis->m_updateProfile.fileUrlVector.end(); ++it)
#else
		for (vector<CString>::iterator it = pThis->m_updateProfile.fileUrlVector.begin(); it != pThis->m_updateProfile.fileUrlVector.end(); ++it)
#endif
		{
			if (FALSE == pThis->m_bStarted)
			{
				bReceiveSuccess = FALSE;

				break;
			}

#ifdef INCLUDE_FILE_SIZE_INFO
			strSourceFileUrl = ((FILE_ATTR)*it).strFile;
			llFileSize = ((FILE_ATTR)*it).llSize;
#else
			strSourceFileUrl = (CString)*it;
#endif	
			strSubPath = strSourceFileUrl;
			strSubPath.Replace(pThis->m_updateProfile.strRootUrl, NULL);
			strSubPath.Replace(_T('/'), _T('\\')); // DOS Path �������� ����
			
			// ���� �̸� �տ� ���� sub dir path�� �ִٸ� ���� �ٿ�����
			strTargetPath = strTargetRootPath;
			PathAppend(strTargetPath.GetBuffer(MAX_PATH), strSubPath);
			strTargetPath.ReleaseBuffer();

			// ���� �̸��� ���ŵȴ�.
			PathRemoveFileSpec(strTargetPath.GetBuffer()); // ���� �̸� ����
			strTargetPath.ReleaseBuffer();

			if (FALSE == PathFileExists(strTargetPath)) // ���丮 ���� ����
				pThis->CreateDirs((LPTSTR)(LPCTSTR)strTargetPath); // ���丮 ����

			if (TRUE == pThis->DownloadFile(strSourceFileUrl, strTargetPath, strDownloadedFullPath))
			{
#ifdef INCLUDE_FILE_SIZE_INFO
				LONGLONG llReceivedFileSize = 0;

				if (TRUE == pThis->GetFileSize(strDownloadedFullPath, llReceivedFileSize))
				{
					if (llFileSize != llReceivedFileSize) // ���� ����� ��ġ ���� ������ �ٿ�ε� ���� ó��
					{
						bReceiveSuccess = FALSE;

						break;
					}
				}
				else
				{
					bReceiveSuccess = FALSE; // ���� ����� �˾ƿ��� ���ص� ���� ó��

					break;
				}
#endif
				receivedFiles.push_back(strDownloadedFullPath);
			}
			else
			{
				bReceiveSuccess = FALSE;

				break;
			}
		}

		// �ٿ�ε� �Ϸ� callback func ȣ��
		if (NULL != pThis->m_pDownloadCompleteCallback)
			pThis->m_pDownloadCompleteCallback(bReceiveSuccess, pThis->m_updateProfile.bPackageUpdate, pThis->m_pDownloadCompleteUserData);

		if (FALSE == bReceiveSuccess)
			throw NULL; // ������ �ϳ� �̻� ���������� ���� ���ߴٸ� ���������� �������� ����

		BOOL bPatchSuccess = TRUE; // ���� ���� ����

		if (TRUE == pThis->m_updateProfile.bPackageUpdate) // ��Ű�� ������Ʈ �̸�
		{
			// ��Ű���� �����ؾ� �Ѵٸ�
			if (TRUE == pThis->m_bRunPackage)
			{
				if (FALSE == pThis->ExecuteProcess(pThis->m_updateProfile.strRunPackageFilePath))
					bPatchSuccess = FALSE;
			}

			// ���� �Ϸ� callback func ȣ��
			if (NULL != pThis->m_pPatchCompleteCallback)
				pThis->m_pPatchCompleteCallback(bPatchSuccess, pThis->m_updateProfile.bPackageUpdate, pThis->m_pPatchCompleteUserData);

			throw NULL; // ���� ����, �ٿ�ε� ���� �ӽ� ���� ���� ���� ����
		}

		// ���� ����
		CString strCopySourcePath;
		CString strCopyTargetPath;
		CString strCopyTargetTempPath;

		for (vector<CString>::iterator it = receivedFiles.begin(); it != receivedFiles.end(); ++it)
		{
			if (FALSE == pThis->m_bStarted) // �����Ǹ�
			{
				bPatchSuccess = FALSE;

				break;
			}

			strCopySourcePath = (CString)*it;
			strCopyTargetTempPath = (CString)*it;

			// ���� Temp ��� ����
			strCopyTargetTempPath.Replace(pThis->GetIncomingPath(), NULL);

			// Copy ���� ��� ��� ������ ��
			strCopyTargetPath = pThis->m_strPatchTargetPath;

			// ���� sub path �߰�
			PathAppend(strCopyTargetPath.GetBuffer(MAX_PATH), strCopyTargetTempPath);
			strCopyTargetPath.ReleaseBuffer();

			// Sub dir ������ ���� ���� �̸� ����
			CString strCopyTargetDirPath(strCopyTargetPath);
			PathRemoveFileSpec(strCopyTargetDirPath.GetBuffer());
			strCopyTargetDirPath.ReleaseBuffer();

			// Sub dir ����
			if (FALSE == PathFileExists(strCopyTargetDirPath))
				pThis->CreateDirs((LPTSTR)(LPCTSTR)strCopyTargetDirPath);

			if (FALSE == CopyFile(strCopySourcePath, strCopyTargetPath, FALSE)) // Overwrite
			{
				bPatchSuccess = FALSE;

				break;
			}
		}

		// ���� �Ϸ� Callback func ȣ��
		if (NULL != pThis->m_pPatchCompleteCallback)
			pThis->m_pPatchCompleteCallback(bPatchSuccess, pThis->m_updateProfile.bPackageUpdate, pThis->m_pPatchCompleteUserData);

	}
	catch (...)
	{
	}

	pThis->DeleteDirs(strTargetRootPath); // Incoming dir ����(�ٿ�ε� ���� �ӽ� ���� ����)
	pThis->m_llReceivedSize = 0;
	pThis->m_bStarted = FALSE;

	return 0;
}

BOOL CCherryUpdate::IsInternetConnected()
{
	DWORD dwNetState = 0;
	DWORD dwConnectionTypes = 0;

	// ���ͳ� ���� 1�� üũ
	if (TRUE == IsNetworkAlive(&dwNetState))
	{
		// ���ͳ� ���� 2�� üũ
		if (TRUE == InternetGetConnectedState(&dwConnectionTypes, 0))
		{
			// ���ͳ� ���� �Ǿ� �ִ�.
			return TRUE;
		}
	}

	return FALSE;
}

#ifdef USE_MFC_CLASS
void CCherryUpdate::Close()
{
	if (FALSE == IsOpen())
		return;

	m_updateProfile.Initialize();
	m_strAgent.Empty();
	m_strRootUrl.Empty();
	m_strPatchTargetPath.Empty();

	CInternetSession::Close();
}
#else
BOOL CCherryUpdate::Close()
{
	if (NULL == m_hSession)
		return FALSE;

	if (NULL != m_pUpdateProcessThread)
	{
		DWORD dwRet = WaitForSingleObject(m_pUpdateProcessThread->m_hThread, INFINITE);

		switch (dwRet)
		{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			break;
		default:
			break;
		}

		delete m_pUpdateProcessThread;
		m_pUpdateProcessThread = NULL;
	}

	BOOL bRet = InternetCloseHandle(m_hSession);
	m_hSession = NULL;

	m_updateProfile.Initialize();
	m_strAgent.Empty();
	m_strClientVersion.Empty();
	m_strPatchTargetPath.Empty();

	return bRet;
}
#endif

CString CCherryUpdate::GetIncomingPath()
{
	if (FALSE == IsOpen())
		return NULL;

	if (TRUE == m_strAgent.IsEmpty())
		return NULL;

	//TCHAR szPath[MAX_PATH] = { 0, };
	//SHGetSpecialFolderPath(NULL, szPath, CSIDL_PROGRAM_FILES_COMMONX86, FALSE);

	CString strPath;

	GetTempPath(MAX_PATH, strPath.GetBuffer(MAX_PATH)); /* C:\Users\[User Name]\AppData\Local\Temp\ */
	strPath.ReleaseBuffer();

	PathAppend(strPath.GetBuffer(MAX_PATH), m_strAgent); // Open �޼ҵ� ȣ�� �� �ο� ���� Agent name���� path�� �����Ѵ�.
	strPath.ReleaseBuffer();

	return strPath;
}

BOOL CCherryUpdate::GetFileVersion(_In_ LPCTSTR lpszFilePath, _Out_ CString &strVersion)
{
	BOOL bRet = TRUE;

	DWORD dwHandle;
	DWORD dwVersionInfoSize = GetFileVersionInfoSize(lpszFilePath, &dwHandle);

	if (0 == dwVersionInfoSize)
		return FALSE;

	HANDLE hBuf = GlobalAlloc(GMEM_MOVEABLE, dwVersionInfoSize);
	LPVOID lpBuf = GlobalLock(hBuf);

	try
	{
		if (FALSE == GetFileVersionInfo(lpszFilePath, dwHandle, dwVersionInfoSize, lpBuf))
			throw FALSE;

		VS_FIXEDFILEINFO *pBuf = NULL;
		UINT nDataSize = 0;

		if (FALSE == VerQueryValue(lpBuf, _T("\\"), (void **)&pBuf, &nDataSize))
			throw FALSE;

		if (NULL == pBuf)
			throw FALSE;

		strVersion.Format(_T("%d.%d.%d.%d"), (pBuf->dwFileVersionMS / 65536),
			(pBuf->dwFileVersionMS % 65536), (pBuf->dwFileVersionLS / 65536),
			(pBuf->dwFileVersionLS % 65536));

	}
	catch (BOOL &bErrorRet)
	{
		bRet = bErrorRet;
	}

	GlobalUnlock(lpBuf);
	GlobalFree(hBuf);

	return bRet;
}

BOOL CCherryUpdate::GetFileSize(_In_ LPCTSTR lpszFilePath, _Out_ LONGLONG &llFileSize)
{
	BOOL bRet = TRUE;

	HANDLE hFile = CreateFile(lpszFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

	LARGE_INTEGER largeInt = { 0, };

	if (TRUE == GetFileSizeEx(hFile, &largeInt))
	{
		llFileSize = largeInt.QuadPart;
	}
	else
	{
		llFileSize = 0;
		bRet = FALSE;
	}

	CloseHandle(hFile);

	return bRet;
}

///////////////////////////////////////////////////////////////////////////
///
/// \brief		���� �� �Լ�
/// \author		ogoons
/// \date		2014-04-27
///
/// \param		lpszCurrentVersion : ���� ����
///				lpszNewVersion : �� ����
///
/// \return		-1 : �� ������ ���� ����.
///				 0 : �� ������ ���� ����.
///				 1 : �� ������ ���� ����.
///
/// \remark		ex. int nRet = CompareVersion(_T("1.0.0.1"), _T("1.0.0.2"));
///					printf(_T("Result: %d"), nRet);
///					.
///					.
///					.
///					Result: 1
///				
/// \section	
///
///////////////////////////////////////////////////////////////////////////
int CCherryUpdate::CompareVersion(LPCTSTR lpszCurrentVersion, LPCTSTR lpszNewVersion)
{
	int nRet = 0; // is equals

	CString strCurVerBuf(lpszCurrentVersion);
	CString strNewVerBuf(lpszNewVersion);
	int nCurVerPos = 0;
	int nNewVerPos = 0;

	do
	{
		UINT nCurVerNum = (UINT)_ttoi(strCurVerBuf.Tokenize(_T("."), nCurVerPos));
		UINT nNewVerNum = (UINT)_ttoi(strNewVerBuf.Tokenize(_T("."), nNewVerPos));

		if (nNewVerNum > nCurVerNum)
		{
			nRet = 1;

			break;
		}
		else if (nNewVerNum < nCurVerNum)
		{
			nRet = -1;

			break;
		}
	} while (-1 < nCurVerPos || -1 < nNewVerPos);

	return nRet;
}

BOOL CCherryUpdate::CheckNewVersion(LPCTSTR lpszCurrentVersion)
{
	if (FALSE == IsOpen())
		return FALSE;

	if (TRUE == m_updateProfile.strNewVerion.IsEmpty())
		return FALSE;

	int nRet = CompareVersion(lpszCurrentVersion, m_updateProfile.strNewVerion);

	if (1 == nRet)
		return TRUE;

	return FALSE;
}

BOOL CCherryUpdate::IsOpen() const
{
#ifdef USE_MFC_CLASS
	if (NULL == CInternetSession::m_hSession)
#else
	if (NULL == m_hSession)
#endif
		return FALSE;

	return TRUE;
}

BOOL CCherryUpdate::IsStarted() const
{
	if (FALSE == IsOpen())
		return FALSE;

	return m_bStarted;
}

void CCherryUpdate::CreateDirs(LPTSTR lpszPath)
{
	TCHAR szDirName[MAX_PATH] = { 0, };	// ������ ���ʸ� �̸�
	TCHAR *pszPath = lpszPath;			// ���ڷ� ���� ���丮
	TCHAR *pszDirName = szDirName;

	while (NULL != *pszPath)
	{
		if ((_T('\\') == *pszPath) || (_T('/') == *pszPath))   // ��Ʈ���丮 Ȥ�� Sub���丮
		{
			if (_T(':') != *(pszPath - 1))
			{
				CreateDirectory(szDirName, NULL);
			}
		}

		*pszDirName++ = *pszPath++;
		*pszDirName = _T('\0');
	}

	CreateDirectory(szDirName, NULL);
}

ULONGLONG CCherryUpdate::GetTotalReceiveFileSize()
{
	if (FALSE == IsOpen())
		return 0L;

	return m_updateProfile.llFileTotalSize;
}

void CCherryUpdate::SetDownloadStatusCallback(LPVOID lpCallbackFunc, void *pUserData)
{
	m_pDownloadStatusCallback = (void(__cdecl *)(ULONGLONG, ULONGLONG, void *))lpCallbackFunc;
	m_pDownloadStatusUserData = pUserData;
}

void CCherryUpdate::SetDownloadCompleteCallback(LPVOID lpCallbackFunc, void *pUserData)
{
	m_pDownloadCompleteCallback = (void(__cdecl *)(BOOL, BOOL, void *))lpCallbackFunc;
	m_pDownloadCompleteUserData = pUserData;
}

void CCherryUpdate::SetPatchCompleteCallback(LPVOID lpCallbackFunc, void *pUserData)
{
	m_pPatchCompleteCallback = (void(__cdecl *)(BOOL, BOOL, void *))lpCallbackFunc;
	m_pPatchCompleteUserData = pUserData;
}

///////////////////////////////////////////////////////////////////////////
///
/// \brief		���� ���� (���� ���� �� ���� ����)
/// \author		ogoons
/// \date		2014-02-10
/// \param		
/// \return		
/// \remark		
/// \section	
///
///////////////////////////////////////////////////////////////////////////
BOOL CCherryUpdate::DeleteDirs(LPCTSTR lpszPath)
{
	if (NULL == lpszPath)
		return FALSE;

	CString strNextDirPath;
	CString strRootUrl;
	CFileFind fileFind;

	BOOL bRet = fileFind.FindFile(lpszPath);

	if (FALSE == bRet)
		return bRet;

	while (bRet)
	{
		bRet = fileFind.FindNextFile();

		if (TRUE == fileFind.IsDots())
			continue;

		if (TRUE == fileFind.IsDirectory())
		{
			strNextDirPath = fileFind.GetFilePath() + _T("\\*.*");
			DeleteDirs(strNextDirPath);	// ���ȣ��
		}
		else
		{
			CString strFilePath(fileFind.GetFilePath());

			// Remove readonly attributes
			DWORD dwAttr = GetFileAttributes(strFilePath);
			if (FILE_ATTRIBUTE_READONLY & dwAttr)
				SetFileAttributes(strFilePath, dwAttr & ~FILE_ATTRIBUTE_READONLY);

			DeleteFile(strFilePath);
		}
	}

	strRootUrl = fileFind.GetRoot();
	fileFind.Close();

	return RemoveDirectory(strRootUrl);
}

void CCherryUpdate::EnableRunPackage(BOOL bRun)
{
	m_bRunPackage = bRun;
}

BOOL CCherryUpdate::ExecuteProcess(_In_ LPCTSTR lpszPath, _In_ LPCTSTR lpszParam, _In_ BOOL bWaitProcess)
{
	SHELLEXECUTEINFO shellExecInfo;
	ZeroMemory(&shellExecInfo, sizeof(shellExecInfo));

	shellExecInfo.cbSize = sizeof(shellExecInfo);
	shellExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	shellExecInfo.hwnd = NULL;
	shellExecInfo.lpVerb = _T("open");
	shellExecInfo.lpParameters = lpszParam;
	shellExecInfo.nShow = SW_SHOWNORMAL;
	shellExecInfo.lpFile = lpszPath;

	// ��Ű�� ����
	if (FALSE == ShellExecuteEx(&shellExecInfo))
		return FALSE;

	if (TRUE == bWaitProcess)
	{
		DWORD dwRet = WaitForSingleObject(shellExecInfo.hProcess, INFINITE);

		switch (dwRet)
		{
		case WAIT_OBJECT_0:
			break;

		case WAIT_FAILED:
			return FALSE;
		}
	}

	return TRUE;
}

CString CCherryUpdate::ConvertUtf8ToUnicode(_In_ LPCWSTR lpszUtf8)
{
	if (NULL == lpszUtf8)
		return NULL;

	int nLen = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpszUtf8, -1, NULL, 0);

	WCHAR *pszBuf = new WCHAR[nLen + 1];
	ZeroMemory(pszBuf, nLen + 1);

	// UTF8 -> Unicode
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpszUtf8, -1, pszBuf, nLen);

	CString strRet;

	if (NULL != pszBuf)
	{
		strRet = pszBuf;

		delete[] pszBuf;
		pszBuf = NULL;
	}

	return strRet;
}
