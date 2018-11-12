#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#include <Stierr.h>

#ifdef _WIN32
#pragma comment(lib,"ntdll.lib")
#elif defined _WIN64
#pragma comment(lib,"ntdll(X64).lib")
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _NTDEF_
	typedef LONG NTSTATUS, *PNTSTATUS;
#endif

#define NT_SUCCESS(status)          ((NTSTATUS)(status)>=0)
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#define STATUS_ACCESS_DENIED        ((NTSTATUS)0xC0000022L)

#define ProcessBasicInformation 0

#define InitializeObjectAttributes( p, n, a, r, s ) { \
	(p)->Length = sizeof( OBJECT_ATTRIBUTES ); \
	(p)->RootDirectory = r; \
	(p)->Attributes = a; \
	(p)->ObjectName = n; \
	(p)->SecurityDescriptor = s; \
	(p)->SecurityQualityOfService = NULL; }


typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;

typedef struct
{
	ULONG          AllocationSize;
	ULONG          ActualSize;
	ULONG          Flags;
	ULONG          Unknown1;
	UNICODE_STRING Unknown2;
	HANDLE         InputHandle;
	HANDLE         OutputHandle;
	HANDLE         ErrorHandle;
	UNICODE_STRING CurrentDirectory;
	HANDLE         CurrentDirectoryHandle;
	UNICODE_STRING SearchPaths;
	UNICODE_STRING ApplicationName;
	UNICODE_STRING CommandLine;
	PVOID          EnvironmentBlock;
	ULONG          Unknown[9];
	UNICODE_STRING Unknown3;
	UNICODE_STRING Unknown4;
	UNICODE_STRING Unknown5;
	UNICODE_STRING Unknown6;
} PROCESS_PARAMETERS, *PPROCESS_PARAMETERS;


typedef struct tagPEB
{
	ULONG               AllocationSize;
	ULONG               Unknown1;
	HINSTANCE        ProcessHinstance;
	PVOID					ListDlls;
	PPROCESS_PARAMETERS ProcessParameters;
	ULONG               Unknown2;
	HANDLE              Heap;
} PEB, *PPEB;

typedef struct tagPROCESS_BASIC_INFORMATION
{
	DWORD ExitStatus;
	PPEB  PebBaseAddress;
	DWORD AffinityMask;
	DWORD BasePriority;
	ULONG UniqueProcessId;
	ULONG InheritedFromUniqueProcessId;
}PROCESS_BASIC_INFORMATION;

typedef struct tagSYSTEM_PROCESSOR_TIMES
{
	LARGE_INTEGER IdleTime;               //����ʱ��
	LARGE_INTEGER KernelTime;             //�ں�ģʽʱ��
	LARGE_INTEGER UserTime;               //�û�ģʽʱ��
	LARGE_INTEGER DpcTime;                //�ӳٹ��̵���ʱ��
	LARGE_INTEGER InterruptTime;          //�ж�ʱ��
	ULONG         InterruptCount;         //�жϴ���
}SYSTEM_PROCESSOR_TIMES,*PSYSTEM_PROCESSOR_TIMES;

typedef struct tagSYSTEM_PERFORMANCE_INFORMATION
{
	LARGE_INTEGER  IdleTime;                    //CPU����ʱ��
	LARGE_INTEGER  ReadTransferCount;           //I/O��������Ŀ
	LARGE_INTEGER  WriteTransferCount;          //I/Oд������Ŀ
	LARGE_INTEGER  OtherTransferCount;          //I/O����������Ŀ
	ULONG          ReadOperationCount;          //I/O��������Ŀ
	ULONG          WriteOperationCount;         //I/Oд������Ŀ
	ULONG          OtherOperationCount;         //I/O��������������Ŀ
	ULONG          AvailablePages;              //�ɻ�õ�ҳ��Ŀ
	ULONG          TotalCommittedPages;         //�ܹ��ύҳ��Ŀ
	ULONG          TotalCommitLimit;            //���ύҳ��Ŀ
	ULONG          PeakCommitment;              //ҳ�ύ��ֵ
	ULONG          PageFaults;                  //ҳ������Ŀ
	ULONG          WriteCopyFaults;             //Copy-On-Write������Ŀ
	ULONG          TransitionFaults;            //��ҳ������Ŀ
	ULONG          Reserved1;
	ULONG          DemandZeroFaults;            //����0������
	ULONG          PagesRead;                   //��ҳ��Ŀ
	ULONG          PageReadIos;                 //��ҳI/O������
	ULONG          Reserved2[2];
	ULONG          PagefilePagesWritten;        //��дҳ�ļ�ҳ��
	ULONG          PagefilePageWriteIos;        //��дҳ�ļ�������
	ULONG          MappedFilePagesWritten;      //��дӳ���ļ�ҳ��
	ULONG          MappedFileWriteIos;          //��дӳ���ļ�������
	ULONG          PagedPoolUsage;              //��ҳ��ʹ��
	ULONG          NonPagedPoolUsage;           //�Ƿ�ҳ��ʹ��
	ULONG          PagedPoolAllocs;             //��ҳ�ط������
	ULONG          PagedPoolFrees;              //��ҳ���ͷ����
	ULONG          NonPagedPoolAllocs;          //�Ƿ�ҳ�ط������
	ULONG          NonPagedPoolFress;           //�Ƿ�ҳ���ͷ����
	ULONG          TotalFreeSystemPtes;         //ϵͳҳ�����ͷ�����
	ULONG          SystemCodePage;              //����ϵͳ����ҳ��
	ULONG          TotalSystemDriverPages;      //�ɷ�ҳ��������ҳ��
	ULONG          TotalSystemCodePages;        //����ϵͳ����ҳ����
	ULONG          SmallNonPagedLookasideListAllocateHits; //
	ULONG          SmallPagedLookasideListAllocateHits;
	ULONG          Reserved3;                   
	ULONG          MmSystemCachePage;          //ϵͳ����ҳ��
	ULONG          PagedPoolPage;              //��ҳ��ҳ��
	ULONG          SystemDriverPage;           //�ɷ�ҳ����ҳ��
	ULONG          FastReadNoWait;             //�첽���ٶ���Ŀ
	ULONG          FastReadWait;               //ͬ�����ٶ���Ŀ
	ULONG          FastReadResourceMiss;       //���ٶ���Դ��ͻ��
	ULONG          FastReadNotPossible;        //���ٶ�ʧ����
	ULONG          FastMdlReadNoWait;          //�첽MDL���ٶ���Ŀ
	ULONG          FastMdlReadWait;            //ͬ��MDL���ٶ���Ŀ
	ULONG          FastMdlReadResourceMiss;    //MDL����Դ��ͻ��
	ULONG          FastMdlReadNotPossible;     //MDL��ʧ����
	ULONG          MapDataNoWait;              //�첽ӳ�����ݴ���
	ULONG          MapDataWait;                //ͬ��ӳ�����ݴ���
	ULONG          MapDataNoWaitMiss;          //�첽ӳ�����ݳ�ͻ����
	ULONG          MapDataWaitMiss;            //ͬ��ӳ�����ݳ�ͻ����
	ULONG          PinMappedDataCount;         //ǣ��ӳ��������Ŀ
	ULONG          PinReadNoWait;              //ǣ���첽����Ŀ
	ULONG          PinReadWait;                //ǣ��ͬ������Ŀ
	ULONG          PinReadNoWaitMiss;          //ǣ���첽����ͻ��Ŀ
	ULONG          PinReadWaitMiss;            //ǣ��ͬ������ͻ��Ŀ
	ULONG          CopyReadNoWait;             //�첽����������
	ULONG          CopyReadWait;               //ͬ������������
	ULONG          CopyReadNoWaitMiss;         //�첽���������ϴ���
	ULONG          CopyReadWaitMiss;           //ͬ�����������ϴ���
	ULONG          MdlReadNoWait;              //�첽MDL������
	ULONG          MdlReadWait;                //ͬ��MDL������
	ULONG          MdlReadNoWaitMiss;          //�첽MDL�����ϴ���
	ULONG          MdlReadWaitMiss;            //ͬ��MDL�����ϴ���
	ULONG          ReadAheadIos;               //��ǰ��������Ŀ
	ULONG          LazyWriteIos;               //LAZYд������Ŀ
	ULONG          LazyWritePages;             //LAZYдҳ�ļ���Ŀ
	ULONG          DataFlushes;                //����ˢ�´���
	ULONG          DataPages;                  //����ˢ��ҳ��
	ULONG          ContextSwitches;            //�����л���Ŀ
	ULONG          FirstLevelTbFills;          //��һ�㻺����������
	ULONG          SecondLevelTbFills;         //�ڶ��㻺����������
	ULONG          SystemCall;                 //ϵͳ���ô���
}SYSTEM_PERFORMANCE_INFORMATION,*PSYSTEM_PERFORMANCE_INFORMATION;

//��ѯ�������ú����Ĳ����ṹ
typedef struct _LS_SYSTEM_PROCESSE_INFO
{
    ULONG			NextEntryDelta;          //���ɽṹ���е�ƫ������
    ULONG			ThreadCount;             //�߳���Ŀ��
	ULONG			MemoryCount;			  // �ڴ�ʹ����
	ULONG			Reseved1[5];
    //ULONG			Reserved1[6];           
    ULARGE_INTEGER	qProcessCreateTime;		//���̴���ʱ��;
    __int64			UserTime;                //�û�ģʽ(Ring 3)��CPUʱ�䣻
    __int64			KernelTime;              //�ں�ģʽ(Ring 0)��CPUʱ�䣻
    UNICODE_STRING	ProcessName;             //�������ƣ�
    ULONG			dwBasePriority;            //��������Ȩ��
    ULONG			ProcessId;               //���̱�ʶ����
    ULONG			InheritedFromProcessId; //�����̵ı�ʶ����
    ULONG			HandleCount;             //�����Ŀ��
    DWORD			dwSessionId;
	ULONG			dwSpareUl3;
    SIZE_T			tPeakVirtualSize;
	SIZE_T			tVirtualSize;
	DWORD			dwPageFaultCount;
	DWORD			dwPeakWorkingSetSize; 
	DWORD			dwWorkingSetSize;						// ��������(�ڴ�)
	SIZE_T			tQuotaPeakPagedPoolUsage;
	SIZE_T			tQuotaPagedPoolUsage;
	SIZE_T			tQuotaPeakNonPagedPoolUsage;
	SIZE_T			tQuotaNonPagedPoolUsage;
	SIZE_T			tPagefileUsage;
	SIZE_T			tPeakPagefileUsage;
	SIZE_T			tPrivatePageCount;
	LARGE_INTEGER	qReadOperationCount;
	LARGE_INTEGER	qWriteOperationCount;
	LARGE_INTEGER	qOtherOperationCount;
	LARGE_INTEGER	qReadTransferCount;
	LARGE_INTEGER	qWriteTransferCount;
	LARGE_INTEGER	qOtherTransferCount;
}LS_SYSTEM_PROCESSES_INFO,*PLS_SYSTEM_PROCESSES_INFO;

typedef struct tagSYSTEM_PAGEFILE_INFORMATION
{
	ULONG NetxEntryOffset;                //��һ���ṹ��ƫ����
	ULONG CurrentSize;                    //��ǰҳ�ļ���С
	ULONG TotalUsed;                      //��ǰʹ�õ�ҳ�ļ���
	ULONG PeakUsed;                       //��ǰʹ�õ�ҳ�ļ���ֵ��
	UNICODE_STRING FileName;              //ҳ�ļ����ļ�����
}SYSTEM_PAGEFILE_INFORMATION,*PSYSTEM_PAGEFILE_INFORMATION;


typedef struct tagSYSTEM_CACHE_INFORMATION
{
	ULONG SystemCacheWsSize;              //���ٻ����С
	ULONG SystemCacheWsPeakSize;          //���ٻ����ֵ��С
	ULONG SystemCacheWsFaults;            //���ٻ���ҳ������Ŀ
	ULONG SystemCacheWsMinimum;           //���ٻ�����Сҳ��С
	ULONG SystemCacheWsMaximum;           //���ٻ������ҳ��С
	ULONG TransitionSharedPages;          //����ҳ��Ŀ
	ULONG TransitionSharedPagesPeak;      //����ҳ��ֵ��Ŀ
	ULONG Reserved[2];
}SYSTEM_CACHE_INFORMATION,*PSYSTEM_CACHE_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemBasicInformation = 0,
	SystemProcessorInformation = 1,
	SystemPerformanceInformation = 2,
	SystemTimeOfDayInformation = 3,
	SystemNotImplemented1 = 4,
	SystemProcessInformation = 5,
	SystemCallCounts = 6,                       // 6 Y N   
	SystemConfigurationInformation = 7,         // 7 Y N   
	SystemProcessorTimes = 8,                   // 8 Y N   
	SystemGlobalFlag = 9,                       // 9 Y Y   
	SystemNotImplemented2 = 10,                  // 10 Y N   
	SystemModuleInformation = 11,                // 11 Y N   
	SystemLockInformation = 12,                  // 12 Y N   
	SystemNotImplemented3 = 13,                  // 13 Y N   
	SystemNotImplemented4 = 14,                  // 14 Y N   
	SystemNotImplemented5 = 15,                  // 15 Y N   
    SystemHandleInformation = 16,                // 16 Y N   
	SystemObjectInformation = 17,                // 17 Y N   
	SystemPagefileInformation = 18,              // 18 Y N   
	SystemInstructionEmulationCounts = 19,       // 19 Y N   
	SystemInvalidInfoClass1 = 20,                // 20   
	SystemCacheInformation = 21,                 // 21 Y Y   
	SystemPoolTagInformation = 22,               // 22 Y N   
	SystemProcessorStatistics = 23,              // 23 Y N   
	SystemDpcInformation = 24,                   // 24 Y Y   
	SystemNotImplemented6 = 25,                  // 25 Y N   
	SystemLoadImage = 26,                        // 26 N Y   
	SystemUnloadImage = 27,                      // 27 N Y   
	SystemTimeAdjustment = 28,                   // 28 Y Y   
	SystemNotImplemented7 = 29,                  // 29 Y N   
	SystemNotImplemented8 = 30,                  // 30 Y N   
	SystemNotImplemented9 = 31,                  // 31 Y N   
	SystemCrashDumpInformation = 32,             // 32 Y N   
	SystemExceptionInformation = 33,             // 33 Y N   
	SystemCrashDumpStateInformation = 34,        // 34 Y Y/N   
	SystemKernelDebuggerInformation = 35,        // 35 Y N   
	SystemContextSwitchInformation = 36,         // 36 Y N   
	SystemRegistryQuotaInformation = 37,         // 37 Y Y   
	SystemLoadAndCallImage = 38,                 // 38 N Y   
	SystemPrioritySeparation = 39,               // 39 N Y   
	SystemNotImplemented10 = 40,                 // 40 Y N   
	SystemNotImplemented11 = 41,                 // 41 Y N   
	SystemInvalidInfoClass2 = 42,                // 42   
	SystemInvalidInfoClass3 = 43,                // 43   
	SystemTimeZoneInformation = 44,              // 44 Y N   
    SystemLookasideInformation = 45,             // 45 Y N   
	SystemSetTimeSlipEvent = 46,                 // 46 N Y   
	SystemCreateSession = 47,                    // 47 N Y   
	SystemDeleteSession = 48,                    // 48 N Y   
	SystemInvalidInfoClass4 = 49,                // 49   
	SystemRangeStartInformation = 50,            // 50 Y N   
	SystemVerifierInformation = 51,              // 51 Y Y   
	SystemAddVerifier = 52,                      // 52 N Y   
	SystemSessionProcessesInformation = 53       // 53 Y N   
} SYSTEM_INFORMATION_CLASS;

typedef enum _THREADINFOCLASS {
	ThreadBasicInformation,
	ThreadTimes,
	ThreadPriority,
	ThreadBasePriority,
	ThreadAffinityMask,
	ThreadImpersonationToken,
	ThreadDescriptorTableEntry,
	ThreadEnableAlignmentFaultFixup,
	ThreadEventPair_Reusable,
	ThreadQuerySetWin32StartAddress,
	ThreadZeroTlsCell,
	ThreadPerformanceCount,
	ThreadAmILastThread,
	ThreadIdealProcessor,
	ThreadPriorityBoost,
	ThreadSetTlsArrayAddress,
	ThreadIsIoPending,
	ThreadHideFromDebugger,
	ThreadBreakOnTermination,
	MaxThreadInfoClass
} THREADINFOCLASS;

typedef struct _CLIENT_ID {
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID;
typedef CLIENT_ID *PCLIENT_ID;

typedef struct _THREAD_BASIC_INFORMATION { // Information Class 0
	LONG     ExitStatus;
	PVOID    TebBaseAddress;
	CLIENT_ID ClientId;
	LONG AffinityMask;
	LONG Priority;
	LONG BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

typedef enum _THREAD_STATE   
{
	StateInitialized,   
	StateReady,   
	StateRunning,   
	StateStandby,   
	StateTerminated,   
	StateWait,   
	StateTransition,   
	StateUnknown
}THREAD_STATE;  

typedef enum _FILE_INFORMATION_CLASS {
	FileDirectoryInformation=1,
	FileFullDirectoryInformation,
	FileBothDirectoryInformation,
	FileBasicInformation,
	FileStandardInformation,
	FileInternalInformation,
	FileEaInformation,
	FileAccessInformation,
	FileNameInformation,
	FileRenameInformation,
	FileLinkInformation,
	FileNamesInformation,
	FileDispositionInformation,
	FilePositionInformation,
	FileFullEaInformation,
	FileModeInformation,
	FileAlignmentInformation,
	FileAllInformation,
	FileAllocationInformation,
	FileEndOfFileInformation,
	FileAlternateNameInformation,
	FileStreamInformation,
	FilePipeInformation,
	FilePipeLocalInformation,
	FilePipeRemoteInformation,
	FileMailslotQueryInformation,
	FileMailslotSetInformation,
	FileCompressionInformation,
	FileCopyOnWriteInformation,
	FileCompletionInformation,
	FileMoveClusterInformation,
	FileQuotaInformation,
	FileReparsePointInformation,
	FileNetworkOpenInformation,
	FileObjectIdInformation,
	FileTrackingInformation,
	FileOleDirectoryInformation,
	FileContentIndexInformation,
	FileInheritContentIndexInformation,
	FileOleInformation,
	FileMaximumInformation
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

typedef struct _IO_STATUS_BLOCK {
	NTSTATUS Status;
	ULONG Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef enum _OBJECT_INFORMATION_CLASS {
	ObjectBasicInformation,
	ObjectNameInformation,
	ObjectTypeInformation,
	ObjectAllInformation,
	ObjectDataInformation
} OBJECT_INFORMATION_CLASS, *POBJECT_INFORMATION_CLASS;

//DLL_API ULONG NTAPI RtlNtStatusToDosError(NTSTATUS);

DLL_API NTSTATUS WINAPI NtTerminateProcess(
								   IN HANDLE ProcessHandle OPTIONAL,
								   IN NTSTATUS ExitStatus);

DLL_API NTSTATUS WINAPI NtQuerySystemInformation (
						  IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
						  OUT PVOID SystemInformation,
						  IN ULONG SystemInformationLength,
						  OUT PULONG ReturnLength OPTIONAL);

DLL_API NTSTATUS WINAPI NtQueryInformationThread(
							IN HANDLE ThreadHandle,
							IN THREADINFOCLASS ThreadInformationClass,
							OUT PVOID ThreadInformation,
							IN ULONG ThreadInformationLength,
							OUT PULONG ReturnLength OPTIONAL) ;

DLL_API NTSTATUS WINAPI NtQueryInformationFile(
						   IN HANDLE  FileHandle,
						   OUT PIO_STATUS_BLOCK  IoStatusBlock,
						   OUT PVOID  FileInformation,
						   IN ULONG  Length,
						   IN FILE_INFORMATION_CLASS  FileInformationClass);

DLL_API NTSTATUS WINAPI NtQueryObject(
							  IN HANDLE               ObjectHandle,
							  IN OBJECT_INFORMATION_CLASS ObjectInformationClass,
							  OUT PVOID               ObjectInformation,
							  IN ULONG                Length,
							  OUT PULONG              ResultLength );

#ifdef __cplusplus
}
#endif
