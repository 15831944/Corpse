#pragma once
#include <WinCrypt.h>

typedef enum tagCERT_TYPE
{
	CERT_TYPE_ENCRYPT		= AT_KEYEXCHANGE,
	CERT_TYPE_SIGNATURE	= AT_SIGNATURE,
}CERT_TYPE;

typedef enum tagSIGNATURE_ALGORITHM
{
		RSA_SHA1RSA			= 0,
		RSA_SHA256RSA		= 1,
		RSA_SHA384RSA    = 2,
		RSA_SHA512RSA    = 3,
}SIGNATURE_ALGORITHM;

typedef enum tagCERT_LOCALTION
{
	CERT_LOCALTION_LOCAL_USER			= 0,
	CERT_LOCALTION_LOCAL_MACHINE		= 1,
}CERT_LOCALTION;

class DLL_API CCryptHelper
{
public:
	CCryptHelper(CERT_TYPE CertType);
	~CCryptHelper();

public:

	/************************************************************************/
	/* �������ƣ�GenericKey							                                                   */
	/* �������ܣ����������˽Կ��Կ��															   */
	/* ����������dwKeyBit ��Կ����(bit)														   */
	/*	�� ��  ֵ������TRUE����ʾ�ɹ�������FALSE����ʾʧ��						   */
	/************************************************************************/
	BOOL GenericKey(DWORD dwKeyBit);

	void SetSignatureAlgorithm(SIGNATURE_ALGORITHM SignAlg);

	BOOL OpenContainer(LPCTSTR lpszContainerName, LPCTSTR lpszProviderName);

	/************************************************************************/
	/* �������ƣ�CreateSelfSignedCertificate                                                  */
	/* �������ܣ�������ǩ��֤��																	   */
	/* ����������lpszSubjectName ʹ������Ϣ												   */
	/*                  CN=֤��ͨ������/����, C��ʹ�������ڹ��ң�L������  		   */
	/*					 S��ʹ��������ʡ��, O:ʹ����������֯,OU��ʹ�������ڲ���  */
	/*					E/Email:ʹ���ߵ����ʼ�														   */
	/************************************************************************/
	BOOL CreateSelfSignedCertificate(LPCTSTR lpszSubjectName, SYSTEMTIME* pStartTime, SYSTEMTIME* pEndTime);

	/************************************************************************/
	/* �������ƣ�AddCertificateToStore		                                                   */
	/* ����������lpszSubjectName ʹ������Ϣ												   */
	/************************************************************************/
	BOOL AddCertificateToStore(CERT_LOCALTION CertLocal, LPCVOID lpData);

	BOOL SetCertSubjectName(LPCTSTR lpszSubjectName, CERT_NAME_BLOB* pSubjectIssuerBlob);

	BOOL ExportPublicKey(CERT_PUBLIC_KEY_INFO* pCertPublicKeyInfo, DWORD* pdwLen);

	/************************************************************************/
	/* �������ƣ�CryptSignAndEncodeCertificate                                           */
	/* ����������ʹ��˽Կǩ��֤�������ļ�													   */
	/************************************************************************/
	BOOL CryptSignAndEncodeCertificate(CERT_REQUEST_INFO* pRequestInfo, BYTE* pEncode, DWORD* pcbEncode);

	static BOOL LoadCertRequestFile(LPCTSTR lpszRequestFile, LPVOID lpData, DWORD* pcbLen);
protected:
	CERT_TYPE		m_CertType;
	HCRYPTKEY		m_hKey;								// ֤��洢�����
	HCRYPTPROV m_hCryptProv;						//
	CString				m_strContainer;					// ��Կ����
	CString				m_strProviderName;			// �ṩ��
	CStringA			m_strSignatureAlgorithm;	// ��Կ�㷨
	PCCERT_CONTEXT	m_pCertContext;			// ֤����
};