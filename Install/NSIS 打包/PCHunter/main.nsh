
# ===================== 外部插件以及宏 =============================
!include "StrFunc.nsh"
!include "WordFunc.nsh"
!include "LogicLib.nsh"
!include "nsDialogs.nsh"
;!include "common.nsh"
!include "x64.nsh"
!include "MUI.nsh"
!include "WinVer.nsh" 
;!include "notify.nsh"
;!include "..\commonfunc.nsh"


!insertmacro MUI_LANGUAGE "SimpChinese"
# ===================== 安装包版本 =============================
VIProductVersion             		"${PRODUCT_VERSION}"
VIAddVersionKey "ProductVersion"    "${PRODUCT_VERSION}"
VIAddVersionKey "ProductName"       "${PRODUCT_NAME}"
VIAddVersionKey "CompanyName"       "${PRODUCT_PUBLISHER}"
VIAddVersionKey "FileVersion"       "${PRODUCT_VERSION}"
VIAddVersionKey "InternalName"      "${EXE_NAME}"
VIAddVersionKey "FileDescription"   "${PRODUCT_NAME}"
VIAddVersionKey "LegalCopyright"    "${PRODUCT_LEGAL}"

;!define INSTALL_PAGE_CONFIG 			0
;!define INSTALL_PAGE_LICENSE 			1
;!define INSTALL_PAGE_PROCESSING 		1
;!define INSTALL_PAGE_FINISH 			2
;!define INSTALL_PAGE_UNISTCONFIG 		3
;!define INSTALL_PAGE_UNISTPROCESSING 	4
;!define INSTALL_PAGE_UNISTFINISH 		5

!define UNINSTALL_REG_PATH	"Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_PATHNAME}"

# 自定义页面
Page custom DUIPage
;Page instfiles  "" InstallShow
;!insertmacro MUI_PAGE_INSTFILES
# 卸载程序显示进度
UninstPage custom un.DUIPage un.CreateUI

# ======================= DUILIB 自定义页面 =========================
Var hMainWnd

Function DUIPage
	; 拼凑安装目录
	StrCpy $INSTDIR "${INSTALL_DEFALT_SETUPPATH}\${INSTALL_APPEND_PATH}"
	; 设置安装目录
	SetOutPath "$TEMP"

	; 将APP 目录下所有文件打包
	File /r "APP\${SKIN_ZIP}"
	;File /r "APP\*"
	; 初始化Duilib界面库
	nsTBCIASkinEngine::InitTBCIASkinEngine "$TEMP" "${SKIN_ZIP}" "${INST_FILE}" #指定资源路径及资源文件
	Pop $hMainWnd
	
	; 设置安装程序窗口名称
	nsTBCIASkinEngine::SetWindowTitle "${INSTALL_WINDOW_TITLE}"

	Call BindUIControls
	; 显示Duilib界面
	nsTBCIASkinEngine::ShowPage 
FunctionEnd


Function un.DUIPage
	#此处检测当前是否有程序正在运行，如果正在运行，提示先卸载再安装 
	StrCpy $INSTDIR "${INSTALL_DEFALT_SETUPPATH}\${INSTALL_APPEND_PATH}"
	; 设置输出目录，主要解决不能删除卸载程序所在目录问题
	SetOutPath $TEMP
	;MessageBox MB_OK $TEMP
	
	; 将APP 目录下所有文件打包
	File /r "APP\${SKIN_ZIP}"
	
	nsTBCIASkinEngine::InitTBCIASkinEngine /NOUNLOAD "$TEMP" "${SKIN_ZIP}" "${UNINST_FILE}"#指定资源路径及资源文件
	Pop $hMainWnd
	
	nsTBCIASkinEngine::SetWindowTitle /NOUNLOAD "${UNINSTALL_WINDOW_TITLE}"
	Call un.BindUIControls
	;MessageBox MB_OK "SHOW"
	nsTBCIASkinEngine::ShowPage /NOUNLOAD
		
	DeleteRegKey HKLM "${UNINSTALL_REG_PATH}" 
	
	Delete "$INSTDIR\${SKIN_ZIP}"
	RMDir /r /REBOOTOK $INSTDIR
FunctionEnd

# 区段1
# 区段名为空、遗漏或者以一个 "-" 开头，那么它将是一个隐藏的区段，用户也不能选择禁止它.
/* Section "-Necessary"

	# 生成卸载程序
	WriteUninstaller "$INSTDIR\uninst.exe"
	
	# 添加卸载信息到控制面板
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "DisplayName" "${PRODUCT_NAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "UninstallString" "$INSTDIR\uninst.exe"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "DisplayIcon" "$INSTDIR\${EXE_NAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "Publisher" "$INSTDIR\${PRODUCT_PUBLISHER}"
SectionEnd */

;本文来自：读书人网(http://www.reader8.cn/)原文链接：http://www.reader8.cn/jiaocheng/20120913/2118443.html