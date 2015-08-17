/****************************LDevelop********************************
*********Copyright @ 吴平杰 (Pingjie Wu) from China,2011*********
*******************************************************************/
/*
  配置对话框的头文件ConfigDialog.h，包含类ConfigDialog的声明
*/

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include"Structs.h"		//包含actions 和configOptions结构体的定义

#include<QDialog>
#include<QStackedWidget>
#include<QListWidgetItem>
#include"ConfigPages.h"

class ConfigDialog:public QDialog
{
	Q_OBJECT
	private:
		QTabWidget *configTab;

		QListWidget *contentsWidget;
		QStackedWidget *pagesWidget;
		void createIcons();

	public:
		ConfigDialog(configOptions,QWidget*);
		configOptions options;
		QPushButton *okButton;
		editConfigPage *editPage;//“编辑器”选项卡页面
		viewConfigPage *viewPage;//“视图”选项卡页面
		lookConfigPage *lookPage;//“查看”选项卡页面

	private slots:
		void changePage(QListWidgetItem*,QListWidgetItem*);
};




#endif

