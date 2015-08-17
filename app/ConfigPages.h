/****************************LDevelop********************************
*********Copyright @ 吴平杰 (Pingjie Wu) from China,2011*********
*******************************************************************/
/*
  ConfigPages.h 包含配置选项三个页面的类声明，这三个类拥有共同的父类QWidget。
  成员定义在ConfigPages.cpp中。
*/

#ifndef CONFIGPAGES_H
#define CONFIGPAGES_H

#include"Structs.h"
#include<QWidget>
#include<QFile>
#include<QLabel>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QCheckBox>
#include<QGroupBox>
#include<QPushButton>
#include<QColorDialog>
#include<QFontDialog>
#include<QComboBox>
#include<QSpinBox>

class editConfigPage : public QWidget
{
    Q_OBJECT
    private:
		QColorDialog *selectColor;
		QLabel *fontTestLabel;
    public:
        editConfigPage(configOptions,QWidget *parent = 0);
		QComboBox *modeCombo,*highlightCombo;
		QPushButton *editBackgroundSelecter,*editFontSelecter;
		QFontDialog *textFont;
		QSpinBox *tabWidth;
		QCheckBox *autoIndent;
		QCheckBox *autoBackup;
		QCheckBox *autoSave;
		QSpinBox *autoSaveTime;

private slots:
		void selectBackgroundColor(QColor);
		void selectTextFont(QFont);
};

class viewConfigPage : public QWidget
{
    Q_OBJECT
    public:
        viewConfigPage(configOptions,QWidget *parent = 0);
		QCheckBox *ShowEditBar;
		QCheckBox *ShowFileBar;
		QCheckBox *ShowBuildBar;
		QCheckBox *ShowRUDoBar;
		QCheckBox *ShowFileTree;
		QCheckBox *ShowOutput;
		QCheckBox *ShowStatusBar;
		QCheckBox *ShowMenuBar;
		QCheckBox *LockToolBars;
};

class lookConfigPage : public QWidget
{
    Q_OBJECT
    public:
       lookConfigPage(configOptions,QWidget *parent = 0);
	   QCheckBox *LineNumber;
	   QCheckBox *HighlightCurLine;
	   QColor *CurLineColor;
};

#endif
