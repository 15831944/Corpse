; 选择压缩方式
SetCompressor  LZMA
;!AddPluginDir MyPlugin

# ====================== 自定义宏 产品信息==============================
!define PRODUCT_NAME           		"系统管家助手"
!define PRODUCT_PATHNAME			"PCAssist"
;!define PRODUCT_PATHNAME           "SGLW"     				#安装卸载项用到的KEY
!define INSTALL_APPEND_PATH         "PCHunter"				#安装路径追加的名称 
!define INSTALL_DEFALT_SETUPPATH    "$PROGRAMFILES"       	#默认生成的安装路径 
!define EXE_NAME               		"PCHunter.exe"
!define INSTALL_WINDOW_TITLE		"PCHunter客户端 安装程序"
!define UNINSTALL_WINDOW_TITLE		"PCHunter客户端 卸载程序"
!define PRODUCT_VERSION        		"1.0.0.0"
!define PRODUCT_PUBLISHER      		"四川龙信信息技术有限公司"
!define PRODUCT_LEGAL          		"Copyright (C) 2017 四川龙信信息技术有限公司"
!define OUTPUT_NAME    				"系统管家助手.exe"
!define	SKIN_ZIP					"Engine.zip"			#安装卸载使用的界面资源
!define	INST_FILE					"inst.xml"				#duilib安装界面描述文件
!define	UNINST_FILE					"uninst.xml"			#duilib卸载界面描述文件
# 针对Vista和win7 的UAC进行权限请求.
# RequestExecutionLevel none|user|highest|admin
RequestExecutionLevel admin

# ====================== 自定义宏 安装信息==============================
!define INSTALL_ICO 				"install.ico"
!define UNINSTALL_ICO				"uninst.ico"
!define UNINSTALL_NAME				"uninst.exe"
# ====================== 文件打包脚本内容 ==============================

!include "nsProcess.nsh"
!include "macro.nsh"
!include "main.nsh"
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