#pragma once

/********************************************************************************************/
/* software [in]��ָ������ϵͳ��Ϊ�̶�ֵ��30																			*/
/*	server [in]���ַ�ָ�룬ָ������ϵͳ���ݿⰲװ�ķ�������SQL Server������							*/
/*	username [in]���ַ�ָ�룬�û�����ָ����ǰ����ϵͳ�Ĳ�����Ա��											*/
/*	Port [in]�����ںţ�0-USB, 1-COM1��2-COM2��3-COM3��4-COM4�������ơ�Ĭ��ֵ��0	*/
/*	Encoder [in]�����л����ͣ�0-�ֶ����л���1-�Զ����л���2-MSR206���ſ�����Ĭ��ֵ��0	*/
/*	TMEncoder [in]��TM���л����ͣ�1-DS9097E��5-DS9097U��Ĭ��ֵ��5                               */
/* ʾ���� i := Init(30, '192.168.0.1','Interface',port,0,5) ;																*/
/*********************************************************************************************/

typedef int (WINAPI* FN_INIT)(int nSoftWare, char* lpszServer, char* lpszUserName, int nPort, int nEncoder, int nTMEncoder);

/************************************************************************/
/* ����������                                                                     */
/* EndSession ;*/
/************************************************************************/
typedef int (WINAPI* FN_EndSession)(void);

/************************************************************************/
/* username [in]���ַ�ָ�룬�û�������������ϵͳ��¼������Ա��           */
/* ChangeUser('����'); */
/************************************************************************/
typedef void (WINAPI* FN_ChangeUser)(char* lpszUserName);

/************************************************************************/
/* room [in]�����ţ����12�ֽ��ַ���������������ϵͳ���õķ��š�
/* gate [in]������ͨ�����ַ�������������Ϊָ��ͨ�����롣���磺��010203����ʾ��Ȩ01��02��03����ͨ�������Ϊ15��
/* Stime [in]����ֹʱ�䣬24�ֽ��ַ�������ʽyyyymmddhhnnyyyymmddhhnn�����磺��200012311230200101011230����ʾ2000��12��31��12ʱ30�ֵ�2001��1��1��12ʱ30�֡���ʼʱ��Ϊ��ǰϵͳʱ�䣬��ֹʱ����������ʼʱ�䡣
/* Guestname [in]���������������30�ֽڣ�����ΪNULL��
/* Guestid [in]������ID�����30�ֽڣ�����ΪNULL��
/* Overflag[in]�����Σ��Ƿ񸲸ǵ�ǰ���ϵ����ݡ�1-���ǣ���ǰ�������¿����Զ�ע���������ǿ��˿�����0-�����ǣ������¿��򷵻ء�
/* openflag[in]�����Σ������Ŀ��ŷ�ʽ��0��������; 1: ����ָ�ƿ��ţ�2��ָ�ƿ��ţ�3��ָ�ƺͿ����š�ͨ��ѡ0
/* Cardno [out]��������ָ�룬���տ��ţ�����������Ψһ�����룩������ΪNULL��
/* Breakfast[in]:���Σ����ȯ������0Ϊ����ͣ�1Ϊ��Чʱ���ڵ�ÿ��1�ݣ�2Ϊ��Чʱ���ڵ�ÿ��2�ݡ�
/* track1 [in]���ַ���ָ�룬�ſ���1�����ݣ��粻д�˹��ΪNULL��
/* track2 [in]���ַ���ָ�룬�ſ���2�����ݣ��粻д�˹��ΪNULL��
/* Lift[in]: �ַ���ָ�룬���ݣ���1����ʾ¥�����Ϊ��B001������2����ʾ¥�����Ϊ��B002�����Դ����ơ�00��ʾ��ϵͳĬ�����ã�99��ʾ����¥�㡣
/* Fingerflag[in]:����,�Ƿ�ɼ�ָ��. 1-�ɼ�,0-���ɼ�                                                                     */
/************************************************************************/
/*typedef int (WINAPI* FN_NewKey)(char* lpszRoom, char* lpszGate, char* lpszSTime, char* lpszGuestName,
														char* lpszGuestId, int nOverFlag, int nOpenFlag, LONG* lCardNo, int nBreakFast, 
														char* lpszTrack1, char* lpszTrack2, char* lpszLift, BOOL bFingerFlag);*/
typedef int (WINAPI* FN_NewKey)(char *lpszRoom,char *lpszGate,char *lpszSTime,char *lpszGuestName,
														char *lpszGuestId, char *lpszLift, int  nOverFlag, int nBreakFast, long *lCardNo, 
														char * lpszTrack1, char * lpszTrack2);
/************************************************************************/
/* �����ơ����˿����·��еĿ��˿���ԭ���Ŀ��˿���ͬʱʹ�ã�����ʼʱ����ͬ��                                                                     */
/* room [in]�����ţ����12�ֽ��ַ���������������ϵͳ���õķ��š�
/* gate [in]������ͨ�����ַ�������������Ϊָ��ͨ�����롣���磺��010203����ʾ��Ȩ01��02��03����ͨ�������Ϊ15��
/* Stime [in]����ֹʱ�䣬24�ֽ��ַ�������ʽyyyymmddhhnnyyyymmddhhnn�����磺��200012311230200101011230����ʾ2000��12��31��12ʱ30�ֵ�2001��1��1��12ʱ30�֡���ʼʱ��Ϊ��ǰϵͳʱ�䣬��ֹʱ����������ʼʱ�䡣
/* Guestname [in]���������������30�ֽڣ�����ΪNULL��
/* Guestid [in]������ID�����30�ֽڣ�����ΪNULL��
/* Overflag[in]�����Σ��Ƿ񸲸ǵ�ǰ���ϵ����ݡ�1-���ǣ���ǰ�������¿����Զ�ע���������ǿ��˿�����0-�����ǣ������¿��򷵻ء�
/* openflag[in]�����Σ������Ŀ��ŷ�ʽ��0��������; 1: ����ָ�ƿ��ţ�2��ָ�ƿ��ţ�3��ָ�ƺͿ����š�ͨ��ѡ0
/* Cardno [out]��������ָ�룬���տ��ţ�����������Ψһ�����룩������ΪNULL��
/* Breakfast[in]:���Σ����ȯ������0Ϊ����ͣ�1Ϊ��Чʱ���ڵ�ÿ��1�ݣ�2Ϊ��Чʱ���ڵ�ÿ��2�ݡ�
/* track1 [in]���ַ���ָ�룬�ſ���1�����ݣ��粻д�˹��ΪNULL��
/* track2 [in]���ַ���ָ�룬�ſ���2�����ݣ��粻д�˹��ΪNULL��
/* Lift[in]: �ַ���ָ�룬���ݣ���1����ʾ¥�����Ϊ��B001������2����ʾ¥�����Ϊ��B002�����Դ����ơ�00��ʾ��ϵͳĬ�����ã�99��ʾ����¥�㡣
/* Fingerflag[in]:����,�Ƿ�ɼ�ָ��. 1-�ɼ�,0-���ɼ�
/************************************************************************/
/*typedef int (WINAPI* FN_DupKey)(char* lpszRoom, char* lpszGate, char* lpszSTime, char* lspzGuestName,
														char* lpszGuestId, int nOverFalg, int nOpenFlag, LONG* lCardNo, int nBreakFast,
														char* lpszTrack1, char* lpszTrack2, char* lpszLift, BOOL bFingerFlag);*/

typedef int (WINAPI* FN_DupKey)(char *lpszRoom,char *lpszGate,char *lpszSTime,char *lspzGuestName,
														char *lpszGuestId, char *lpszLift, int  nOverFalg, int nBreakFast, long *lCardNo,
														char * lpszTrack1,char * lpszTrack2);

/*typedef int (WINAPI* FN_AddKey)(char* lpszRoom, char* lpszGate, char* lpszSTime, char* lspzGuestName,
														char* lpszGuestId, int nOverFalg, int nOpenFlag, LONG* lCardNo, int nBreakFast,
														char* lpszTrack1, char* lpszTrack2, char* lpszLift, BOOL bFingerFlag);*/

/************************************************************************/
/* ����
/*	room [in]�����ţ�6�ֽ��ַ���������������ϵͳ���õķ��š�
/*	gate [in]������ͨ�����ַ�����������00����ʾ��Ĭ����Ȩͨ������99����ʾ��Ȩ���й���ͨ��������Ϊָ��ͨ�����롣���磺��010203����ʾ��Ȩ01��02��03����ͨ�������Ϊ15��
/*	Stime [in]����ֹʱ�䣬24�ֽ��ַ�������ʽyyyymmddhhnnyyyymmddhhnn�����磺��200012311230200101011230����ʾ2000��12��31��12ʱ30�ֵ�2001��1��1��12ʱ30�֡�
/*	Guestname [in]���������������30�ֽڣ�����ΪNULL��
/*	Guestid [in]������ID�����30�ֽڣ�����ΪNULL��
/*	Overflag[in]�����Σ��Ƿ񸲸ǵ�ǰ���ϵ����ݡ�1-���ǣ���ǰ�������¿����Զ�ע���������ǿ��˿�����0-�����ǣ������¿��򷵻ء�
/*	openflag[in]�����Σ������Ŀ��ŷ�ʽ��0��������; 1: ����ָ�ƿ��ţ�2��ָ�ƿ��ţ�3��ָ�ƺͿ����š�ͨ��ѡ0
/*	Cardno [out]��������ָ�룬���տ��ţ�����������Ψһ�����룩������ΪNULL��
/*	St [out]������ָ�룬���շ��صĿ�״̬��1-����ʹ�ã�3-����ע����4-��ʧע����5-���ע����6-�Զ�ע��������ΪNULL��
/*	Breakfast[in]:���Σ����ȯ������0Ϊ����ͣ�1Ϊ��Чʱ���ڵ�ÿ��1�ݣ�2Ϊ��Чʱ���ڵ�ÿ��2�ݡ�
/*	track1 [in]���ַ���ָ�룬�ſ���1�����ݣ��粻д�˹��ΪNULL��
/*	track2 [in]���ַ���ָ�룬�ſ���2�����ݣ��粻д�˹��ΪNULL��
/*	Lift[in]: �ַ���ָ�룬��1����ʾ¥�����Ϊ��B001������2����ʾ¥�����Ϊ��B002�����Դ����ơ�*/
/************************************************************************/
/*typedef int (WINAPI* FN_ReadCard)(char* lpszRoom, char* lpszGate, char* lpszSTime, char* lpszGuestName,
															char* lpszGuestId, char* lpszTrack1, char* lpszTrack2, char* lpszLift, LONG* lpszCardNo, int* st, int* nOpenFlag, int* nBreakFast);*/
typedef int (WINAPI* FN_ReadCard)(char *lpszRoom, char *lpszGate,char *lpszSTime, char *lpszGuestName,
															char *lpszGuestId, char *lpszLift, char *lpszTrack1, char *lpszTrack2, long *lpszCardNo, int *st, int *nBreakFast);
/************************************************************************/
/* ע������ͬʱ�������ݿ�����                                                                     */
/* cardno [in]����ע�����Ŀ���, ͨ��������á�
/* track1 [in]���ַ���ָ�룬�ſ���1�����ݣ��粻д�˹��ΪNULL��
/* track2 [in]���ַ���ָ�룬�ſ���2�����ݣ��粻д�˹��ΪNULL��
/* fpindex [out]������ָ�룬���շ��ص�ָ�ƺţ�����ΪNULL��
/* ��fpindex����0����ʾ�ֿ��˵�ָ�����˷��󣬻����Կ��š���Ҫʹ��ע����ע���÷����ָ�ơ�
/************************************************************************/
/*typedef int (WINAPI* FN_EraseCard)(LONG lCardNo, char* lpszTrack1, char* lpszTrack2, int* fpIndex);*/
typedef int (WINAPI* FN_EraseCard)(LONG lCardNo, char* lpszTrack1, char* lpszTrack2);

//�˷������������ݿ����ݣ���ע����
// ������
// room [in]�����š�
// Cardno [in]�����ţ�����Ϊ0�����˲���Ϊ0ʱ�����ͷ����п��˿����Ϊ����ע����
typedef int (WINAPI* FN_CheckOut)(char *lpszRoom, LONG lCardno);

// ��ʧע�������������ݿ����ݣ���ע����
//room [in]�����š�
//Cardno [in]�����ţ����Ϊ��ʧע�����˿��Ŀ��š�
typedef int (WINAPI* FN_LostCard)(char *lpszRoom, LONG lCardno);

// ������
// ��ע1������ʹ��ʱ����20��ʾ��û�е���SetPort����
typedef int (WINAPI* FN_PopCard)(void);


//���ô��ڣ�����Init�����󲻱ص��ô˺���
// software [in]��ָ������ϵͳ���μ�ע2��
// Port [in]�����ںţ����Σ�1-COM1��2-COM2��3-COM3��4-COM4�������ơ�
// Encoder [in]�����л����ͣ�0-�ֶ����л���1-�Զ����л���
// TMEncoder [in]��TM���л����ͣ�1-DS9097E��5-DS9097U��

typedef int (WINAPI* FN_SetPort)(int nSoftware, int port, int nEncoder, int nTmEncoder);
/************************************************************************/
/* ��ȡ��Ƭ�����ݡ�                                                                     */
/* cardtype [int]�����ͣ���Ƭ���ͣ�Ĭ��дΪ18��
/* start [int]�����ͣ����ö�ȡ��Ƭ����ʼ�����Ϳ�ţ�start = block | (section << 8)������block Ϊ��ţ�sectionΪ������
/* len [int]�����ͣ����ö�ȡ��Ƭ���ݵĳ��ȡ�����ֵ ��16,32,48��
/* buff [out]���ֽ����飬���ÿ�Ƭ�����룬��ȡ��ȡ�����ݣ����е�һ���ֽ�Ϊ�������ͣ�0-A���룬1-B���롣2-7Ϊ���������룬������ִ�гɹ�ʱ��buffΪ��ȡ���������ݡ�
/************************************************************************/
//typedef int (WINAPI* FN_ReadCardData)(int nCardType, int nStart, int nLen, char* szBuff);

/************************************************************************/
/* ��Ƭд�����ݡ�                                                                     */
/* cardtype [in]����Ƭ������,Ĭ��18��
/* start [in]�����ͣ�����д�뿨Ƭ����ʼ�����Ϳ�ţ�start = block | (section << 8)������block Ϊ��ţ�sectionΪ������
/* len [in]�����ͣ�����д�뿨Ƭ���ݵĳ��ȡ���
/* buff [int]���ֽ����飬���ÿ�Ƭ�����룬����Ҫд������ݣ����е�һ���ֽ�Ϊ�������ͣ�0-A���룬1-B���롣2-7Ϊ����������,8λ�Ժ�Ϊ��Ҫд�뿨Ƭ�����ݡ�
/************************************************************************/
//typedef int (WINAPI* FN_WriteCardData)(int nCardType, int nStart, int nLen, char* szBuff);


/************************************************************************/
/* ��ȡ��Ƭ��ID��                                                                     */
/* CardId [out]��������ָ�룬���շ��صĿ���ʶ������ΪNULL��
/************************************************************************/
/*typedef int (WINAPI* FN_ReadId)(LONG* lCardId);*/
// ��Mifare��ID��
//pid [out]��4�ֽ��޷�ָ�룬���տ�ID��
typedef int (WINAPI* FN_ReadCardId)(LONG *pCardId);

// �˶�4442������
//lpszSC [in]��IC�����룬3�ֽ��޷��ַ�ָ��
typedef int (WINAPI* FN_CheckSC)(char *lpszSC);

// // �������ݡ�
//start [in]����ʼ��ַ��ָ������д��λ�á�����Mifare����Adel9200����
//��ʼ��ַ�����ֽ���ɣ����ֽ�Ϊ��ţ����ֽ�Ϊ���š���0x0102��ʾ��ȡ2��1�顣
//Len [in]��Ҫ��ȡ���ݵĳ��ȡ�����Mifare����Adel9200������Ϊ16����������
//str [in��out]���޷��ַ�ָ�룬��������ȡ�����ݡ�����Mifare����A90��
//ͬʱ��Ϊ���������������������Ȩ���룬��7���ֽ���ɣ�
//��һ���ֽ�Ϊ��Ȩ�룺��Ϊ0��˶�A���룬��Ϊ1��˶�B���룬��6���ֽ�Ϊ���롣
typedef int (WINAPI* FN_ReadIC)(UINT nStart, UINT nLen, char* lpszData);

//д���ݵ�IC����
typedef int (WINAPI* FN_WriteIC)(UINT nStart, UINT nLen, char* lpszData);

//д�ſ��������û����ݴ洢��1��2�죬����ϵͳʹ�õ�3��
// track1 [in]���ַ���ָ�룬�ſ���1�����ݣ��粻д�˹��ΪNULL��
// track2 [in]���ַ���ָ�룬�ſ���2�����ݣ��粻д�˹��ΪNULL��
// track3 [in]���ַ���ָ�룬�ſ���3�����ݣ��粻д�˹��ΪNULL��
typedef int (WINAPI* FN_WriteMagCard)(char *lpszTrack1, char * lpszTrack2, char * lpszTrack3);

//���ſ���
// track1 [out]���ַ���ָ�룬���մſ���1�����ݣ��粻���˹�ΪNULL��
// track2 [out]���ַ���ָ�룬���մſ���2�����ݣ��粻���˹�ΪNULL��
// track3 [out]���ַ���ָ�룬���մſ���3�����ݣ��粻���˹�ΪNULL��
typedef int (WINAPI* FN_ReadMagCard)(char * lpszTrack1, char * lpszTrack2, char * lpszTrack3);

//��������
//Sound [in]��ָ��������LED״̬��11-�̵�1�룬����һ����12-���1�룬����һ����15-���1�룬�̽�������16-�̽�һ����
typedef int (WINAPI* FN_ReaderBeep)(int Sound);

// buff [in]���޷��ַ�ָ�룬�������ݡ�
// len [in]�����ݳ��ȣ��ֽڣ���
// cardtype[out]������ָ�룬���տ��ͣ�1-�ܲÿ���2-��������3-������4-�ܿؿ���5-ʱ�ӿ���6-��࿨��
//7-¥�㿨��8-��࿨��9-���˿���10-��ֹ����11-���鿨��12-Ӧ������13-�˷�����14-���ÿ�������ΪNULL��
// cardst[out]������ָ�룬���տ�״̬��1-������3-ע����4-��ʧ��5-��٣�6-���ڣ�����ΪNULL��
// cardno[out]������ָ�룬���տ��ţ�����ΪNULL��
// roomno[out]���ַ�ָ�룬���տ��˿�/���ÿ����ţ���࿨/¥�㿨/��࿨���򡣲�С��20�ֽڣ�����ΪNULL��
// username[out]���ַ�ָ�룬���տ�����������С��20�ֽڣ�����ΪNULL��
// ����ֵ����ע1��
// stime[out]���ַ�ָ�룬���տ��˿�/��࿨��Ч�ڣ���С��28�ֽڣ�����ΪNULL��
//��ʽyyyymmddhhnnyyyymmddhhnn�����磺��200012311230200101011230��
//��ʾ2000��12��31��12ʱ30�ֵ�2001��1��1��12ʱ30�֡�
typedef int (WINAPI* FN_GetBuffInfo)(char *Buff,int nLen,int *nCardType,int *nCardState,int *nCardNo,char *RoomNo,char *UserName,char *STime);

// buff [in]���ַ���ָ�룬�ſ����ݡ�
// cardtype[out]������ָ�룬���տ��ͣ�1-�ܲÿ���2-��������3-������4-�ܿؿ���5-ʱ�ӿ���6-��࿨��7-¥�㿨��8-��࿨��9-���˿���10-��ֹ����11-���鿨��12-Ӧ������13-�˷�����14-���ÿ�������ΪNULL��
// cardst[out]������ָ�룬���տ�״̬��1-������3-ע����4-��ʧ��5-��٣�6-���ڣ�����ΪNULL��
// cardno[out]������ָ�룬���տ��ţ�����ΪNULL��
// roomno[out]���ַ�ָ�룬���տ��˿�/���ÿ����ţ���࿨/¥�㿨/��࿨���򡣲�С��20�ֽڣ�����ΪNULL��
// username[out]���ַ�ָ�룬���տ�����������С��20�ֽڣ�����ΪNULL��
// ����ֵ����ע1��
// stime[out]���ַ�ָ�룬���տ��˿�/��࿨��Ч�ڣ���С��28�ֽڣ�����ΪNULL����ʽyyyymmddhhnnyyyymmddhhnn��
//���磺��200012311230200101011230����ʾ2000��12��31��12ʱ30�ֵ�2001��1��1��12ʱ30�֡�
typedef int (WINAPI* FN_GetMagBuffInfo)(char *Buff, int *nCardType,int *nCardState,int *nCardNo,char *RoomNo,char *UserName,char *STime);

//cardno [in]�����š�
//cardtype[in]�����͡�
//cardst[out]������ָ�룬���տ�״̬��1-������3-ע����4-��ʧ��5-��٣�6-���ڣ�����ΪNULL��
//roomno[out]���ַ�ָ�룬���տ��˿�/���ÿ����ţ���࿨/¥�㿨/��࿨���򡣲�С��20�ֽڣ�����ΪNULL��
//username[out]���ַ�ָ�룬���տ�����������С��20�ֽڣ�����ΪNULL��
//stime[out]���ַ�ָ�룬���տ��˿�/��࿨��Ч�ڣ���С��28�ֽڣ�����ΪNULL����ʽyyyymmddhhnnyyyymmddhhnn��
//���磺��200012311230200101011230����ʾ2000��12��31��12ʱ30�ֵ�2001��1��1��12ʱ30�֡�
typedef int (WINAPI* FN_GetCardInfo)(int nCardNo,int nCardType,int *nCardState, char *RoomNo,char *UserName,char *STime);

/************************************************************************/
/* ��ȡ��Ƭ��ID��                                                                     */
/* CardId [out]��������ָ�룬���շ��صĿ���ʶ������ΪNULL��
/************************************************************************/
typedef int (WINAPI* FN_ReadId)(LONG* lCardId);