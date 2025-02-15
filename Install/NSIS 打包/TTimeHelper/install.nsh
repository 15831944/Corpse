;!include "nsProcess.nsh"
;!include "ttimehelper.nsh"
;!define UNINSTALL_REG_PATH	"Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_PATHNAME}"


#绑定安装的界面事件 
Function BindUIControls
	# 关闭按扭
	${BindControl} "BtnClose" OnBtnClose
	
	# 最小化按扭
	${BindControl} "BtnMin" OnBtnMin

	# 取消按扭
	${BindControl} "BtnCancel" OnBtnCancel

	# 接受按扭
	${BindControl} "BtnAccept" OnBtnAccept
	
	# 下一步
	${BindControl} "BtnNext" OnBtnNext
	
	# 上一步
	${BindControl} "BtnBack" OnBtnBack
	
	# 开始安装
	${BindControl} "BtnStartInstall" OnStartInstall
	
	# 完成按扭
    ${BindControl} "BtnComplete" OnFinished
FunctionEnd

Function OnStartInstall
	nsTBCIASkinEngine::NextPage "WizardTab"
    GetFunctionAddress $0 OnInstallProgressCallback
	;启动section 区块
    section::call $0
FunctionEnd

Function OnInstallProgressCallback
	; 获取文件释放进度
	Pop $R0	
    nsTBCIASkinEngine::SetProcessValue "InstallProgress" $R0
	${if} $R0 == 100
		nsTBCIASkinEngine::NextPage "WizardTab"
	${endif}
FunctionEnd

Function OnBtnClose
	;MessageBox MB_YESNO|MB_ICONQUESTION "安装尚未结束，您确定要退出${EXE_NAME} 安装程序吗?"
	nsTBCIASkinEngine::ShowMsgBox "    你确定要退出 ${PRODUCT_NAME} 的安装吗？" "${INSTALL_WINDOW_TITLE}"
	Pop $0
	${if} $0 == 1
		SendMessage $hMainWnd ${WM_SYSCOMMAND} 0xF060 0
	${endif}
FunctionEnd

Function OnBtnMin
	; 最小化安装程序
	SendMessage $hMainWnd ${WM_SYSCOMMAND} 0xF020 0
FunctionEnd

Function OnBtnCancel
	Call OnBtnClose	
FunctionEnd

Function OnBtnAccept
	nsTBCIASkinEngine::NextPage "WizardTab"
FunctionEnd

Function OnBtnBack
	nsTBCIASkinEngine::BackPage "WizardTab"
FunctionEnd

Function OnBtnNext
	nsTBCIASkinEngine::NextPage "WizardTab"
FunctionEnd

Function OnFinished	
	nsTBCIASkinEngine::GetCheckStatus "OptionRun"
	Pop $0
	${if} $0 == 1
		Exec "$INSTDIR\${EXE_NAME}"
	${endif}
	
	nsTBCIASkinEngine::TBCIAClose
FunctionEnd