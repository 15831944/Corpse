; 选择压缩方式
SetCompressor  LZMA
;!AddPluginDir MyPlugin

# ====================== 自定义宏 产品信息==============================
!define PRODUCT_NAME           		"时间助手"
!define PRODUCT_PATHNAME			"TTimeHelperAssist"
;!define PRODUCT_PATHNAME           "SGLW"     				#安装卸载项用到的KEY
!define INSTALL_APPEND_PATH         "TTimeHelper"			#安装路径追加的名称 
!define INSTALL_DEFALT_SETUPPATH    "$PROGRAMFILES"       	#默认生成的安装路径 
!define EXE_NAME               		"TTimeHelper.exe"
!define INSTALL_WINDOW_TITLE		"TTimeHelper客户端 安装程序"
!define UNINSTALL_WINDOW_TITLE		"TTimeHelper客户端 卸载程序"
!define PRODUCT_VERSION        		"1.0.0.0"
!define PRODUCT_PUBLISHER      		"四川龙信信息技术有限公司"
!define PRODUCT_LEGAL          		"Copyright (C) 2017 四川龙信信息技术有限公司"
!define OUTPUT_NAME    				"时间助手.exe"

# 针对Vista和win7 的UAC进行权限请求.
# RequestExecutionLevel none|user|highest|admin
RequestExecutionLevel admin

# ====================== 自定义宏 安装信息==============================
!define INSTALL_ICO 				"TTimeHelper.ico"
!define UNINSTALL_ICO				"uninst.ico"
!define UNINSTALL_NAME				"uninst.exe"
# ====================== 文件打包脚本内容 ==============================

!include "nsProcess.nsh"
!include "macro.nsh"
!include "ttimehelper.nsh"
!include "install.nsh"
!include "uninstall.nsh"
!include "callback.nsh"
# ====================== NSIS属性 ======================================

#SetCompressor zlib

; 安装包标题显示名字.
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"

# 安装程序文件名.
OutFile "Output\${OUTPUT_NAME}"

InstallDir "${INSTALL_DEFALT_SETUPPATH}\${INSTALL_APPEND_PATH}"

# 安装和卸载程序图标
Icon              "${INSTALL_ICO}"
UninstallIcon     "${UNINSTALL_ICO}"