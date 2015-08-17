/****************************LDevelop********************************
*********Copyright @ 吴平杰 (Pingjie Wu) from China,2011*********
*******************************************************************/
/*
  LDevelop.cpp 最关键的源文件。
  主要实现了LDevelop主程序的框架及主要功能。在逻辑上将各个窗体部分组织起来。
  文件比较长，也许可以考虑分割一下的。
*/
#include "ldevelop.h"
#include "EditArea.cpp"
#include "ToolBars.cpp"
#include "SearchDialog.cpp"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QDirModel>
#include <QTreeView>
#include <QStatusBar>
#include <QMenuBar>
#include <QDockWidget>
#include <QFileDialog>
#include <QProcess>
#include "ConfigDialog.cpp"

#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

//初始化主程序，设置配置选项
LDevelop::LDevelop()
{
	TabsNum=0;					//初始标签数目
	MaxTabsNum=MAXTABSNUM;		//最大标签数目
	curFileName=new QString;		//当前标签页的文件名
	editArea=NULL;				//当前编辑的标签页类

	//读取配置文件
    readConfig(":/config.LDevelop");

	inputDialog=new SearchDialog(this);			//搜索对话框初始化
	configDialog=new ConfigDialog(options,this);		//配置对话框初始化
	parasInput=new QInputDialog(this);			//带参运行对话框初始化

	editTab=new QTabWidget;					//标签页初始化
	editTab->setTabShape(QTabWidget::Triangular);	//标签页形状
    setWindowIcon(QIcon(":/images/applications-development.svgz"));
    setWindowFilePath(tr("Welcome!"));

	setupMenus();				//初始化菜单栏
	setupToolBars();			//初始化工具栏
	setupDirView();			//初始化树形文件视图
	setupFileList();				//初始化文件列表
	lockTheBars(true);
	setupOutput();				//初始化编译输出区域
	setCentralWidget(editTab);	//设置标签页类
	setupSignals();				//初始化信号与槽
	setupOptions();				//设置配置选项
	enableActions(false);
}

//此函数根据所传的字符串来设置相应的配置选项
//顺便说一下，配置文件config.LDevelop的正确格式
//是以“=”为标识的，只要拥有“=”，则LDevelop都会认为
//这是一个配置选项。比如有这样一行“ProgramMode=IDE”
//则ProgramMode是选项名，IDE是选项值。若又有“Test=MID=true”
//则Test是选项名，true是值。至于是否读入此选项，则要视具体
//的选项名称而定。
void LDevelop::setupOption(QString configLine)
{
    if(configLine.indexOf("=")==-1)
        return;
	QStringList args=configLine.split("=");	//根据“=”分割字符串
	QString option=args.first().trimmed();	//取首字符串为选项名
	QString value=args.last().trimmed();	//取末字符串为值
    // QMessageBox::warning(this,tr("读取配置选项"),
    //                   option+tr("==")+value);
    //switch(option.toUtf8().data()){


	//选项值的分情况设置，先判断选项，若格式正确，则赋值。
    if(option=="ProgramMode"){
        if(value==tr("IDE")||value==tr("1"))
            options.ProgramMode=1;
        else
            options.ProgramMode=2;
    }
	if(option==("TextHighlightMode")){
		options.TextHighlightMode=0;
        if(value==tr("C++")||value==tr("C")||value==tr("0"))
			options.TextHighlightMode=0;
        else if(value==tr("Qt")||value==tr("1"))
			options.TextHighlightMode=1;
		// else if()
        else if(value==tr("none")||value==tr("-1"))
			options.TextHighlightMode=-1;
    }
	if(option=="BackgroundColor"){
        options.BackgroundColor=("white");
        QColor color(value);
        if(color.isValid())
            options.BackgroundColor=value;
    }
    if(option==("TextFont")){
        options.TextFont=QFont("monospace",12,24,false);
		options.TextFont.setFamily(value);
    }
    if(option=="TextSize"){
        options.TextSize=12;
        options.TextSize=value.toInt();
		options.TextFont.setPixelSize(options.TextSize);
    }
    if(option=="TextFontStyle"){
        options.TextFontStyle=0;
        if(value==tr("bold")||value==tr("Bold")){
            options.TextFontStyle=1;
            options.TextFont.setBold(true);
            options.TextFont.setItalic(false);
        }
        if(value==tr("italic")||value==tr("Italic")){
            options.TextFontStyle=2;
            options.TextFont.setBold(false);
            options.TextFont.setItalic(true);
        }
        if(value==tr("bold italic")||value==tr("Bold Italic")){
            options.TextFontStyle=3;
            options.TextFont.setBold(true);
            options.TextFont.setItalic(true);
        }
    }
    if(option=="TabWidth"){
        options.TabWidth=40;
        if(!value.isEmpty())
            options.TabWidth=value.toInt()*10;
    }
    if (option=="AutoIndent"){
        if(value==tr("true")||value==tr("1"))
            options.AutoIndent=true;
        else
            options.AutoIndent=false;
    }
    if (option=="SetBackup"){
        if(value==tr("true")||value==tr("1"))
            options.SetBackup=true;
        else
            options.SetBackup=false;
    }
    if (option=="AutoSaveTime"){
        options.AutoSaveTime=10;
        if(!value.isEmpty())
            options.AutoSaveTime=value.toInt();
    }
    if (option=="LockBar"){
        if(value==tr("true")||value==tr("1"))
            options.LockBar=true;
        else
            options.LockBar=false;
    }
    if (option=="OutputAreaVisiable"){
        if(value==tr("true")||value==tr("1"))
            options.OutputAreaVisiable=true;
        else
            options.OutputAreaVisiable=false;
    }
    if (option=="FileTreeVisiable"){
        if(value==tr("true")||value==tr("1"))
            options.FileTreeVisiable=true;
        else
            options.FileTreeVisiable=false;
    }
    if (option=="FileBar"){
        if(value==tr("true")||value==tr("1"))
            options.FileBar=true;
        else
            options.FileBar=false;
    }
    if (option=="EditBar"){
        if(value==tr("true")||value==tr("1"))
            options.EditBar=true;
        else
            options.EditBar=false;
    }
    if (option=="ReUnDoBar"){
        if(value==tr("true")||value==tr("1"))
            options.ReUnDoBar=true;
        else
            options.ReUnDoBar=false;
    }
    if (option=="BuildBar"){
        if(value==tr("true")||value==tr("1"))
            options.BuildBar=true;
        else
            options.BuildBar=false;
    }
    if (option=="StatusBar"){
        if(value==tr("true")||value==tr("1"))
            options.StatusBar=true;
        else
            options.StatusBar=false;
    }
    if (option=="LineNumber"){
        if(value==tr("true")||value==tr("1"))
            options.LineNumber=true;
        else
            options.LineNumber=false;
    }
    if (option=="AutoChangeLine"){
        if(value==tr("true")||value==tr("1"))
            options.AutoChangeLine=true;
        else
            options.AutoChangeLine=false;
    }
    if (option==tr("HighlightCurrentLine")){
        if(value==tr("true")||value==tr("1"))
            options.HighlightCurrentLine=true;
		else{
            options.HighlightCurrentLine=false;
			options.CurrentLineColor=QColor(options.BackgroundColor);
		}
    }
	if (option=="CurrentLineColor"){
		options.CurrentLineColor=QColor("lightGray");
        QColor color(value);
        if(color.isValid())
			options.CurrentLineColor=color;
    }
}

//读取配置文件configFile，并提取字符串
void LDevelop::readConfig(QString configFile)
{
	QString configLine;
	QFile config(configFile);
	//若文件不存在或者为空，则取默认设置并写入文件。
	if(!config.exists()||config.size()==0){
		config.open(QFile::WriteOnly|QFile::Truncate);
		config.write("[Editor Options]\n"
					 "ProgramMode=TextEditor\n"
					 "TextHighlightMode=C++\n"
					 "TextFont=Monospace\n"
					 "TextSize=12\n"
					 "TextFontStyle=Italic\n"
					 "BackgroundColor=white\n"
					 "#Edit Options\n"
					 "TabWidth=4\n"
					 "AutoIndent=true\n"
					 "SetBackup=true\n"
					 "AutoSaveTime=0\n"
					 "[View Options]\n"
					 "LockBar=false\n"
					 "OutputAreaVisiable=true\n"
					 "FileTreeVisiable=true\n"
					 "FileBar=true\n"
					 "EditBar=true\n"
					 "ReUnDoBar=true\n"
					 "BuildBar=true\n"
					 "StatusBar=true\n"
					 "[Doc Options]"
					 "LineNumber=true\n"
					 "AutoChangeLine=true\n"
					 "HighlightCurrentLine=true\n"
					 "CurrentLineColor=#ff0000\n"
					 "[Build Options]\n");
		config.close();
	}
	//读取文件，成功
	if(config.open(QFile::ReadOnly)){
        do{
			configLine=config.readLine();	//读一行
			setupOption(configLine);		//设置之
        }while(!config.atEnd());
        config.close();
		//    configDialog->options=options;
        return;
    }
	//失败
    else{
        int ret=QMessageBox::warning(this,tr("Read Failure"),
                                     tr("Failed to read the configuration file, load the default settings?"),
									 QMessageBox::No,QMessageBox::Ok);
        if(ret==QMessageBox::No)
			exit(0);
        return;
    }
}


//为自动补全而定义，尚未实现
void LDevelop::appendText()
{
	//editArea->document()->
    //editArea->appendPlainText(tr("HaHa"));
}

//编辑首选项对话框
void LDevelop::editPrefOptions()
{
    //	QMessageBox::warning(this,tr(""),editArea->document()->toPlainText());
	//editArea->textChanged();
	//connect(editArea,SIGNAL(textChanged()),this,SLOT(appendText()));
	//puts("configDialog here");
	configDialog->show();
	//	ConfigDialog configDialog;
	//	configDialog.show();
	//printf("configDialog here\n");
}

//将配置写入文件，先新建一个文件，然后根据原配置文件以及configOptions
//写入新文件。最后删除原文件。
void LDevelop::setConfig(configOptions setOptions, QString configFile)
{
	QString optionLine;
	QStringList optionList;
	QString option;
	QFile oldConfig(configFile);
    QFile newConfig(configFile+tr("~"));
	if(oldConfig.open(QFile::ReadOnly)&&newConfig.open(QFile::WriteOnly|QFile::Truncate|QFile::Text)){
		do{
			optionLine=oldConfig.readLine();
            //	QMessageBox::warning(this,tr(""),optionLine.toUtf8());
			//若不是正确的配置选项，则依旧写入原字符串
			if(optionLine.indexOf("=")<0){
				newConfig.write(optionLine.toUtf8().data());
				continue;
			}
			//否则根据内容而定
			else{
				optionList=optionLine.split("=");
				option=optionList.first();
				newConfig.write((setOption(option,setOptions)+'\n').toUtf8().data());
			}
		}while(!oldConfig.atEnd());
		if(oldConfig.isOpen())
			oldConfig.close();
		if(newConfig.isOpen())
			newConfig.close();
		oldConfig.remove();			//删除原文件
		newConfig.rename(configFile);	//新文件更名
	}
	//若写入失败
	else
        QMessageBox::warning(configDialog,tr("Set Configuration Options Failed"),
                             tr("Failed to write configuration settings,it will maintain the previous settings."));
}

//根据配置选项字符串 option和配置选项setOptions的值返回应写入文件的字符串
//其实跟setupOption()是一样的。
QString LDevelop::setOption(QString option,configOptions setOptions)
{
	if(option=="ProgramMode"){
		if(setOptions.ProgramMode==1)
            return option+tr("=IDE");
		else
            return option+tr("=TextEditor");
	}

	if(option==("TextHighlightMode")){
		if(setOptions.TextHighlightMode==0)
            return option+tr("=C++");
		if(setOptions.TextHighlightMode==1)
            return option+tr("=Qt");
		if(setOptions.TextHighlightMode==-1)
            return option+tr("=none");
	}

	if (option=="BackgroundColor"){
        return option+tr("=")+setOptions.BackgroundColor;
	}

	if(option==("TextFont")){
        return option+tr("=")+setOptions.TextFont.toString().split(tr(",")).first();
	}
	if(option=="TextSize"){
		return option+QString("=%1").arg(setOptions.TextSize);
	}
	if(option=="TextFontStyle"){
		if(setOptions.TextFontStyle==0)
            return option+tr("=Regular");
		if(setOptions.TextFontStyle==1)
            return option+tr("=Bold");
		if(setOptions.TextFontStyle==2)
            return option+tr("=Italic");
		if(setOptions.TextFontStyle==3)
            return option+tr("=Bold Italic");
	}
	if(option=="TabWidth"){
        return option+tr("=%1").arg(setOptions.TabWidth/10);
	}
	if (option=="AutoIndent"){
		if(setOptions.AutoIndent==true)
            return option+tr("=true");
		else
            return option+tr("=false");
	}
	if (option=="SetBackup"){
		if(setOptions.SetBackup==true)
            return option+tr("=true");
		else
            return option+tr("=false");
	}

	if (option=="AutoSaveTime"){
        return option+tr("=%1").arg(setOptions.AutoSaveTime);
	}

	if (option=="LockBar"){
		if(setOptions.LockBar==true)
            return option+tr("=true");
		else
            return option+tr("=false");
	}

	if (option=="OutputAreaVisiable"){
		if(setOptions.OutputAreaVisiable==true)
            return option+tr("=true");
		else
            return option+tr("=false");
	}

	if (option=="FileTreeVisiable"){
		if(setOptions.FileTreeVisiable==true)
            return option+tr("=true");
		else
            return option+tr("=false");
	}
	if (option=="FileBar"){
		if(setOptions.FileBar==true)
            return option+tr("=true");
		else
            return option+tr("=false");
	}
	if (option=="EditBar"){
		if(setOptions.EditBar==true)
            return option+tr("=true");
		else
            return option+tr("=false");
	}
	if (option=="ReUnDoBar"){
		if(setOptions.ReUnDoBar==true)
            return option+tr("=true");
		else
            return option+tr("=false");
	}
	if (option=="BuildBar"){
		if(setOptions.BuildBar==true)
            return option+tr("=true");
		else
            return option+tr("=false");
	}
	if (option=="StatusBar"){
		if(setOptions.StatusBar==true)
            return option+tr("=true");
		else
            return option+tr("=false");
	}
	if (option=="LineNumber"){
		if(setOptions.LineNumber==true)
            return option+tr("=true");
		else
            return option+tr("=false");
	}
	if (option=="AutoChangeLine"){
		if(setOptions.AutoChangeLine==true)
            return option+tr("=true");
		else
            return option+tr("=false");
	}
    if (option==tr("HighlightCurrentLine")){
		if(setOptions.HighlightCurrentLine==true)
            return option+tr("=true");
		else
            return option+tr("=false");
	}
	if (option=="CurrentLineColor"){
        return option+tr("=")+setOptions.CurrentLineColor.name();
	}
	return option;
}

//从对话框的当前设置改变options的值为用户自定义值
void LDevelop::readConfigFromTable()
{
	options.ProgramMode=configDialog->editPage->modeCombo->currentIndex()+1;
	options.AutoIndent=configDialog->editPage->autoIndent->isChecked();
	options.AutoSaveTime=((int)configDialog->editPage->autoSave->isChecked()) * configDialog->editPage->autoSaveTime->value();
	options.BackgroundColor=configDialog->editPage->editBackgroundSelecter->palette().background().color().name();
	options.TextFont=configDialog->editPage->textFont->currentFont();
	options.BuildBar=this->acts.showBuildBar->isChecked();
	options.EditBar=this->acts.showEditToolBar->isChecked();
	options.FileBar=this->acts.showFileToolBar->isChecked();
	options.TabWidth=configDialog->editPage->tabWidth->value()*10;
	options.TextSize=configDialog->editPage->textFont->selectedFont().pixelSize();
	//options.TextFontStyle=configDialog->editPage->textFont->style();
	options.TextHighlightMode=configDialog->editPage->highlightCombo->currentIndex();
	options.HighlightCurrentLine=configDialog->lookPage->HighlightCurLine->isChecked();
	options.LineNumber=configDialog->lookPage->LineNumber->isChecked();
}

//写入
void LDevelop::do_writeConfig()
{
	readConfigFromTable();
    setConfig(options,tr(":/config.LDevelop"));
	configDialog->accept();
}

//初始化工具栏
void LDevelop::setupToolBars()
{
	fileToolBar=new FileToolBars;
    fileToolBar->setWindowTitle(tr("File"));
	fileToolBar->saveAct()->setDisabled(true);
	fileToolBar->saveAsAct()->setDisabled(true);

	editToolBar=new EditToolBars;
    editToolBar->setWindowTitle(tr("Edit"));
	editToolBar->addAction(acts.searchAct);
	editToolBar->addAction(acts.replaceAct);
	editToolBar->setDisabled(true);

	ReUnDoBar=new QToolBar;
    ReUnDoBar->setWindowTitle(tr("History"));
	ReUnDoBar->addAction(acts.undoAct);
	ReUnDoBar->addAction(acts.redoAct);

	compileBar=new QToolBar;
    compileBar->setWindowTitle(tr("Build"));
	compileBar->addAction(acts.compileAct);
	compileBar->addAction(acts.runAct);
	compileBar->addAction(acts.runWithParasAct);
	compileBar->addAction(acts.cmplAndRunAct);

	logoLabel=new QLabel;
    QPixmap *logo=new QPixmap(":/images/applications-development.svgz");
	logoLabel->setPixmap(*logo);
	logoLabel->setAlignment(Qt::AlignCenter);
	logoLabel->setUpdatesEnabled(true);
	//logoLabel->setAutoFillBackground(true);
	resize(960,640);

    editTab->addTab(logoLabel,tr("Welcome"));

    statusBar()->showMessage(tr("Welcome to LDevelop, all is ready!"),0);

	addToolBar(fileToolBar);
	addToolBar(ReUnDoBar);
	addToolBar(editToolBar);
	addToolBar(Qt::LeftToolBarArea,compileBar);

	QToolBar *empty=new QToolBar;
	empty->setFixedWidth(24);
	empty->setMovable(false);
	addToolBar(Qt::RightToolBarArea,empty);

}

//初始化菜单
void LDevelop::setupMenus()
{
	//setup menubar and actions
	//Filemenu
	//“文件”下拉菜单
    fileMenu=menuBar()->addMenu(tr("&File"));
	//新建文件
    acts.newAct=new QAction(QIcon(":/images/document-new.svg"),tr("&New File"),this);
	acts.newAct->setShortcuts(QKeySequence::New);		//快捷键
    acts.newAct->setStatusTip(tr("Edit new file"));		//提示
    acts.newAct->setToolTip(tr("Create new file"));
	//打开文件
    acts.openAct=new QAction(QIcon(":/images/document-open.svg"),tr("&Open..."),this);
	acts.openAct->setShortcuts(QKeySequence::Open);
    acts.openAct->setStatusTip(tr("Open file"));
    acts.openAct->setToolTip(tr("Open a file"));
	//保存文件
    acts.saveAct=new QAction(QIcon(":/images/document-save.svg"),tr("&Save"),this);
	acts.saveAct->setShortcuts(QKeySequence::Save);
    acts.saveAct->setStatusTip(tr("Save the file you are editing"));
    acts.saveAct->setToolTip(tr("Save the file you are editing"));
	acts.saveAct->setDisabled(true);
	//保存为
    acts.saveAsAct=new QAction(QIcon(":/images/document-save-as.svg"),tr("Save &As.."),this);
	acts.saveAsAct->setShortcuts(QKeySequence::SaveAs);
    acts.saveAsAct->setStatusTip(tr("Save the file to the specified location"));
    acts.saveAsAct->setToolTip(tr("Save the file to the specified location"));
	acts.saveAsAct->setDisabled(true);
    //退出LDevelop
    acts.exitAct=new QAction(QIcon(":/images/window-close.svg"),tr("&Exit"),this);
	acts.exitAct->setShortcuts(QKeySequence::Close);
    acts.exitAct->setStatusTip(tr("Quit LDevelop"));
    acts.exitAct->setToolTip(tr("Quit LDevelop"));
	//“文件”下拉菜单
	fileMenu->addAction(acts.newAct);
	fileMenu->addAction(acts.openAct);
	fileMenu->addSeparator();
	fileMenu->addAction(acts.saveAct);
	fileMenu->addAction(acts.saveAsAct);
	fileMenu->addSeparator();
	fileMenu->addAction(acts.exitAct);

	//“编辑”下拉菜单
	//重做
    editMenu=menuBar()->addMenu(tr("&Edit"));
    acts.redoAct=new QAction(QIcon(":/images/edit-redo.svg"),tr("&Redo"),this);
    acts.redoAct->setToolTip(tr("Redo"));
    acts.redoAct->setStatusTip(tr("Redo"));
	acts.redoAct->setDisabled(true);
	//撤销
    acts.undoAct=new QAction(QIcon(":/images/edit-undo.svg"),tr("&Undo"),this);
    acts.undoAct->setToolTip(tr("Undo"));
    acts.undoAct->setStatusTip(tr("Undo"));
	acts.undoAct->setDisabled(true);
	//搜索
    acts.searchAct=new QAction(QIcon(":/images/edit-find.svg"),tr("&Search"),this);
    acts.searchAct->setToolTip(tr("Search"));
    acts.searchAct->setStatusTip(tr("Search strings in text"));
	acts.searchAct->setShortcuts(QKeySequence::Find);
	acts.searchAct->setDisabled(true);
	//替换
    acts.replaceAct=new QAction(QIcon(":/images/edit-find-replace.svg"),tr("&Replace"),this);
    acts.replaceAct->setToolTip(tr("Find/Replace"));
    acts.replaceAct->setStatusTip(tr("Search and replace strings in text"));
	acts.replaceAct->setShortcuts(QKeySequence::Replace);
	acts.replaceAct->setDisabled(true);
	//首选项
    acts.preferencesAct=new QAction(QIcon(":/images/preferences.png"),tr("&Preference"),this);
	acts.preferencesAct->setShortcut(Qt::Key_F10);
    acts.preferencesAct->setToolTip(tr("Preference"));
	editMenu->addAction(acts.redoAct);
	editMenu->addAction(acts.undoAct);
	editMenu->addSeparator();
	editMenu->addAction(acts.searchAct);
	editMenu->addAction(acts.replaceAct);
	editMenu->addSeparator();
	editMenu->addAction(acts.preferencesAct);

	//“视图”下拉菜单
    viewMenu=menuBar()->addMenu(tr("&View"));
	//全屏
    acts.showFullScreen=new QAction(tr("&Fulls Screen"),this);
	acts.showFullScreen->setShortcut(Qt::Key_F11);
	acts.showFullScreen->setCheckable(true);
	viewMenu->addAction(acts.showFullScreen);
    acts.showToolBar=viewMenu->addMenu(tr("&Toolbar"));
    acts.showFileToolBar=new QAction(QIcon(":/images/selected.png"),tr("File"),this);

    acts.showEditToolBar=new QAction(QIcon(":/images/selected.png"),tr("Edit"),this);

    acts.showRUToolBar=new QAction(QIcon(":/images/selected.png"),tr("History"),this);

    acts.showBuildBar=new QAction(QIcon(":/images/selected.png"),tr("Build"),this);

	acts.showToolBar->addAction(acts.showFileToolBar);
	acts.showToolBar->addAction(acts.showEditToolBar);
	acts.showToolBar->addAction(acts.showRUToolBar);
	acts.showToolBar->addAction(acts.showBuildBar);

    acts.showMenuBar=new QAction(QIcon(":/images/selected.png"),tr("&Menu Bar"),this);
	acts.showMenuBar->setCheckable(true);
	//acts.showMenuBar-;
	menuBar()->actions().insert(0,acts.showMenuBar);
    acts.lockBars=new QAction(tr("&Lock Toolbar"),this);
    acts.showStatusBar=new QAction(tr("&Status Bar"),this);
	acts.showStatusBar->setCheckable(true);
	acts.showStatusBar->setChecked(true);
	acts.lockBars->setCheckable(true);
	acts.lockBars->setChecked(true);

	viewMenu->addAction(acts.showMenuBar);
	viewMenu->addAction(acts.showStatusBar);
	viewMenu->addAction(acts.lockBars);
	viewMenu->addSeparator();
	
    acts.textFormat=viewMenu->addMenu(tr("Text &Format"));
    acts.font=acts.textFormat->addMenu(tr("Font"));
    acts.size=acts.textFormat->addMenu(tr("Size"));

	fonts=new QActionGroup(this);

	//字体选项菜单
    QAction *monospace=new QAction(tr("Monospace"),this);
	monospace->setCheckable(true);
	connect(monospace,SIGNAL(triggered()),this,SLOT(setFont()));
	
    QAction *jomolhari=new QAction(tr("Jomolhari"),this);
	jomolhari->setCheckable(true);
	connect(jomolhari,SIGNAL(triggered()),this,SLOT(setFont()));

    QAction *console=new QAction(tr("Console"),this);
	console->setCheckable(true);
	connect(console,SIGNAL(triggered()),this,SLOT(setFont()));

    QAction *sans=new QAction(tr("Sans"),this);
	sans->setCheckable(true);
	connect(sans,SIGNAL(triggered()),this,SLOT(setFont()));

	fonts->addAction(jomolhari);
	fonts->addAction(monospace);
	fonts->addAction(console);
	fonts->addAction(sans);
	monospace->setChecked(true);

	acts.font->addAction(jomolhari);
	acts.font->addAction(monospace);
	acts.font->addAction(console);
	acts.font->addAction(sans);

	fontSizes=new QActionGroup(this);

    QAction *size_4=new QAction(tr("4"),this);
	size_4->setCheckable(true);
	connect(size_4,SIGNAL(triggered()),this,SLOT(setFont()));

    QAction *size_8=new QAction(tr("8"),this);
	size_8->setCheckable(true);
	connect(size_8,SIGNAL(triggered()),this,SLOT(setFont()));

    QAction *size_12=new QAction(tr("12"),this);
	size_12->setCheckable(true);
	connect(size_12,SIGNAL(triggered()),this,SLOT(setFont()));

    QAction *size_16=new QAction(tr("16"),this);
	size_16->setCheckable(true);
	connect(size_16,SIGNAL(triggered()),this,SLOT(setFont()));

    QAction *size_20=new QAction(tr("20"),this);
	size_20->setCheckable(true);
	connect(size_20,SIGNAL(triggered()),this,SLOT(setFont()));

    QAction *size_30=new QAction(tr("30"),this);
	size_30->setCheckable(true);
	connect(size_30,SIGNAL(triggered()),this,SLOT(setFont()));

    QAction *size_48=new QAction(tr("48"),this);
	size_48->setCheckable(true);
	connect(size_48,SIGNAL(triggered()),this,SLOT(setFont()));

	fontSizes->addAction(size_4);
	fontSizes->addAction(size_8);
	fontSizes->addAction(size_12);
	fontSizes->addAction(size_16);
	fontSizes->addAction(size_20);
	fontSizes->addAction(size_30);
	fontSizes->addAction(size_48);

	acts.size->addAction(size_4);
	acts.size->addAction(size_8);
	acts.size->addAction(size_12);
	acts.size->addAction(size_16);
	acts.size->addAction(size_20);
	acts.size->addAction(size_30);
	acts.size->addAction(size_48);
	size_12->setChecked(true);


	//“工具”下拉菜单
    toolsMenu=menuBar()->addMenu(tr("&Tools"));

    acts.compileAct=new QAction(QIcon(":/images/debug-run.svg"),tr("&Build"),this);
    acts.compileAct->setStatusTip(tr("Compiles the current file as an executable"));
	acts.compileAct->setShortcut(Qt::Key_F7);
    acts.runAct=new QAction(QIcon(":/images/run-build.svg"),tr("&Run"),this);
    acts.runAct->setStatusTip(tr("Execute the compiled binary file"));
	acts.runAct->setShortcut(Qt::Key_F8);
    acts.cmplAndRunAct=new QAction(QIcon(":/images/run-build-configure.svg"),tr("Buid && Run"),this);
    acts.cmplAndRunAct->setStatusTip(tr("Compile and run"));
	acts.cmplAndRunAct->setShortcut(Qt::Key_F9);
    acts.runWithParasAct=new QAction(QIcon(":/images/run-build-clean.svg"),tr("Run With Parameter"),this);
    acts.runWithParasAct->setStatusTip(tr("Input parameters for the program and execute"));

	toolsMenu->addAction(acts.compileAct);
	toolsMenu->addAction(acts.runAct);
	toolsMenu->addAction(acts.runWithParasAct);
	toolsMenu->addAction(acts.cmplAndRunAct);
	
    helpMenu=menuBar()->addMenu(tr("&Help"));
    acts.calculatAct=new QAction(tr("Word Count"),this);
    acts.aboutAct=new QAction(tr("About"),this);
    acts.aboutQtAct=new QAction(tr("About &Qt"),this);

	helpMenu->addAction(acts.calculatAct);
	helpMenu->addSeparator();
	helpMenu->addAction(acts.aboutAct);
	helpMenu->addAction(acts.aboutQtAct);
	//enableActions(false);
}

//初始化文件列表，实现上尚有点技术问题，故并未开启此项功能
void LDevelop::setupFileList()
{
	fileList=new QListWidget;
	fileList->setSpacing(10);
	// fileList->currentIndex();
	//fileList->setContentsMargins(30,30,30,30);
	// treeDock->
	//QVBoxLayout *treeLayout=new QVBoxLayout;
	//treeLayout->addWidget(treeView);
	//treeLayout->addWidget(fileList);
	//  QTreeView *fileTree=new QTreeView;
	// fileTree->set
	QDockWidget *fileDock=new QDockWidget;
	fileDock->setWidget(fileList);
	//   addDockWidget(Qt::LeftDockWidgetArea,fileDock);
	//       connect(fileList,SIGNAL(currentRowChanged(int)),this,SLOT(onFileListClicked(int)));
}

//单击打开文件列表上的文件
void LDevelop::onFileListClicked(int index)
{
	printf("%d",index);
	// changeCurTab(index);
	//editTab->setCurrentIndex(index);
}

//初始化树形文件视图
void LDevelop::setupDirView()
{
	//文件夹模型
	dirModel=new QDirModel();
	//树形视图
	treeView=new QTreeView;
	treeView->setModel(dirModel);
	treeView->setAnimated(true);

	treeView->setIndentation(24);
	treeView->setSortingEnabled(true);
	//	treeView->setFixedWidth(280);
	//	treeView->setColumnWidth(0,280);
	treeView->setColumnHidden(1,true);
	treeView->setColumnHidden(2,true);
	treeView->setColumnHidden(3,true);

	//树形视图在一个DockWidget中显示
    treeDock=new QDockWidget(tr("Folder View"),this);
	treeDock->setAllowedAreas(Qt::RightDockWidgetArea|Qt::LeftDockWidgetArea);
	treeDock->setWidget(treeView);
	treeDock->setAcceptDrops(false);
	viewMenu->insertAction(acts.lockBars,treeDock->toggleViewAction());
	treeDock->setVisible(false);
	addDockWidget(Qt::LeftDockWidgetArea, treeDock);
}

//初始化构建信息输出区域
void LDevelop::setupOutput()
{
    outputDock=new QDockWidget(tr("Build Output"),this);
	outputArea=new QPlainTextEdit;
	outputArea->setReadOnly(true);
	setOutputDoc();
	outputDock->setAllowedAreas(Qt::BottomDockWidgetArea);
	outputDock->setWidget(outputArea);
	viewMenu->insertAction(acts.lockBars,outputDock->toggleViewAction());
	addDockWidget(Qt::BottomDockWidgetArea,outputDock);
}

//设置选项，理论上可以且应该包含读取配置文件，但目前只实现了工作模式的控制
void LDevelop::setupOptions()
{

	statusBar()->setVisible(options.StatusBar);
    if(options.ProgramMode==2){
        /*QToolBar *empty=new QToolBar;
        empty->setFixedWidth(24);
        empty->setMovable(false);
        addToolBar(Qt::LeftToolBarArea,empty);*/

        compileBar->setVisible(false);
		acts.showBuildBar->setDisabled(true);
		acts.showBuildBar->setVisible(false);

        outputDock->setVisible(false);
		outputDock->setDisabled(true);
		outputDock->toggleViewAction()->setDisabled(true);
		outputDock->toggleViewAction()->setVisible(false);
        toolsMenu->setDisabled(true);
		resize(720,640);
        return;
	}
}

//信号与槽
void LDevelop::setupSignals()
{
	connect(fileToolBar->newAct(),SIGNAL(triggered()),this,SLOT(newFile()));
	connect(fileToolBar->openAct(),SIGNAL(triggered()),this,SLOT(openFile()));
	connect(fileToolBar->saveAct(),SIGNAL(triggered()),this,SLOT(saveFile()));
	connect(fileToolBar->saveAsAct(),SIGNAL(triggered()),this,SLOT(saveAsFile()));
	connect(editToolBar->copyAct(),SIGNAL(triggered()),this,SLOT(copyEdit()));
	connect(editToolBar->pasteAct(),SIGNAL(triggered()),this,SLOT(pasteEdit()));
	connect(editToolBar->cutAct(),SIGNAL(triggered()),this,SLOT(cutEdit()));
	//connect(editArea);


	connect(treeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(openTreeFile()));
	connect(acts.newAct,SIGNAL(triggered()),this,SLOT(newFile()));
	connect(acts.openAct,SIGNAL(triggered()),this,SLOT(openFile()));
	connect(acts.saveAct,SIGNAL(triggered()),this,SLOT(saveFile()));
	connect(acts.saveAsAct,SIGNAL(triggered()),this,SLOT(saveAsFile()));
	connect(acts.undoAct,SIGNAL(triggered()),this,SLOT(handleUndo()));
	connect(acts.redoAct,SIGNAL(triggered()),this,SLOT(handleRedo()));
	connect(acts.searchAct,SIGNAL(triggered()),this,SLOT(searchText()));
	//  connect(inputDialog->input,SIGNAL(textEdited(QString)),this,SLOT(do_search()));
	connect(inputDialog->replaceButton,SIGNAL(clicked()),this,SLOT(do_replace()));
	connect(inputDialog->findNext,SIGNAL(clicked()),this,SLOT(downSearch()));
	connect(parasInput,SIGNAL(accepted()),this,SLOT(run()));
	connect(acts.replaceAct,SIGNAL(triggered()),this,SLOT(replaceText()));
	connect(acts.showFullScreen,SIGNAL(triggered(bool)),this,SLOT(showFullScreen(bool)));
	connect(acts.showFileToolBar,SIGNAL(triggered()),this,SLOT(hideFileBar()));
	connect(acts.showEditToolBar,SIGNAL(triggered()),this,SLOT(hideEditBar()));
	connect(acts.showRUToolBar,SIGNAL(triggered()),this,SLOT(hideRUBar()));
	connect(acts.showBuildBar,SIGNAL(triggered()),this,SLOT(hideBuildBar()));
	connect(acts.compileAct,SIGNAL(triggered()),this,SLOT(build()));
	connect(acts.runAct,SIGNAL(triggered()),this,SLOT(run()));
	connect(acts.cmplAndRunAct,SIGNAL(triggered()),this,SLOT(build_and_run()));
	connect(acts.runWithParasAct,SIGNAL(triggered()),this,SLOT(run_with_paras()));
	connect(acts.showStatusBar,SIGNAL(triggered(bool)),this->statusBar(),SLOT(setVisible(bool)));
	connect(acts.lockBars,SIGNAL(triggered(bool)),this,SLOT(lockTheBars(bool)));
	connect(acts.showMenuBar,SIGNAL(triggered()),this,SLOT(hideMenuBar()));
	connect(acts.calculatAct,SIGNAL(triggered()),this,SLOT(calculatTool()));
	connect(acts.aboutAct,SIGNAL(triggered()),this,SLOT(about()));
	connect(acts.aboutQtAct,SIGNAL(triggered()),qApp,SLOT(aboutQt()));
	connect(acts.exitAct,SIGNAL(triggered()),this,SLOT(close()));
	connect(acts.preferencesAct,SIGNAL(triggered()),this,SLOT(editPrefOptions()));
	connect(editTab,SIGNAL(tabCloseRequested(int)),this,SLOT(tabCloseRequested(int)));
	//	connect(editArea,SIGNAL(textChanged()),this,SLOT(markModified()));
	connect(editTab,SIGNAL(currentChanged(int)),this,SLOT(changeCurTab(int)));
	connect(configDialog->okButton,SIGNAL(clicked()),this,SLOT(do_writeConfig()));
}

//若当前文件被修改
bool LDevelop::handleCurIsModified()
{
	if(editArea->document()->isModified()){
		QMessageBox::StandardButton ret;
        ret=QMessageBox::warning(this,tr("Save changes?"),
                                 tr( "<h2>Do you need to save changes to the following files:<br>%1 ?</h2>"
                                         "If you choose no, the changes you make will be permanently lost.<br>")
								 .arg(editTab->tabText(curIndex)),
								 QMessageBox::Cancel|QMessageBox::Discard|QMessageBox::Save
								 );
		if(ret==QMessageBox::Save){
			saveFile();
			return true;
		}
		else if(ret==QMessageBox::Cancel){
			setWindowModified(true);
			return false;
		}
	}
	setWindowModified(false);
	return true;
}

//新建文件
void LDevelop::newFile()
{
	if(!creatNewTab())
		return;

	QString newFile("~newfile~");
	//   fileList->addItem(new QListWidgetItem(newFile));
	//  fileList->setCurrentRow(curIndex);

	editArea->setCurrentDoc(&newFile);
	*curFileName=newFile;
	if(!TabsNum){
		//resize(800,600);
		editTab->setTabText(curIndex,newFile);
		editArea->setDocumentTitle(newFile);
        //		editTab->setTabIcon(0,QIcon(":/images/editing.svg"));
		//setCentralWidget(editTab);
		enableActions(true);
	}
    editTab->setTabIcon(curIndex,QIcon(":/images/story-editor.svg"));
	//	editTab->setCurrentWidget(editArea);
	//	setCurrentFile("");
	//}
    statusBar()->showMessage(tr("The new tab page has been created"));
    setWindowFilePath(tr("~newfile~"));
	editTab->setTabText(curIndex,newFile);
	editArea->setDocumentTitle(newFile);

}

//加载文件到新标签页
void LDevelop::loadFile(QString filePath)
{
	QString fileName;
	if(filePath.isEmpty()){
		if(!curFileName->isEmpty())
			fileName=QFileDialog::getOpenFileName(this,
                                                  tr("Open"),
												  *curFileName,
                                                  tr("All files (*)"));
		else
			fileName=QFileDialog::getOpenFileName(this);
	}
	else
		fileName=QString(filePath);
	//curFileName=new QString(fileName);
	for(int i=0;i<TabsNum;i++){
		if(editTab->tabText(i)==fileName){
			editTab->setCurrentIndex(i);
			return;
		}
	}
	if(!fileName.isEmpty()){
		if(!creatNewTab())
			return;
		//   fileList->addItem(new QListWidgetItem(fileName));
		//  fileList->setCurrentRow(curIndex);
		editArea->setCurrentDoc(&fileName);
        editTab->setTabIcon(curIndex,QIcon(":/images/story-editor.svg"));
	}
	else return;
	*curFileName=fileName;
    statusBar()->showMessage(fileName+tr(" Opened successfully"),0);
	setWindowFilePath(fileName);
	setWindowModified(false);
	editArea->setDocumentTitle(fileName);
	editTab->setTabText(editTab->currentIndex(),fileName);
	editTab->setTabToolTip(editTab->currentIndex(),fileName);
}

//打开文件
void LDevelop::openFile()
{
	loadFile(QString());
}

//双击打开树形视图中的文件
void LDevelop::openTreeFile()
{
	QString fileName;
	QModelIndex cur=treeView->currentIndex();
	
	if(!cur.child(0,0).data(Qt::DisplayRole).toString().isEmpty())
		return;
	fileName="/"+cur.data(Qt::DisplayRole).toString();
	do{
		cur=cur.parent();
		fileName="/"+cur.data(Qt::DisplayRole).toString()+fileName;
    }while(cur.parent().data(Qt::DisplayRole).toString()!=tr("/"));
	loadFile(fileName);
}
//返回当前文件名，很鸡肋很狗屁的一个函数
QString* LDevelop::getCurFileName()
{
	return curFileName;
}

//使能部分菜单选项及工具栏，在模式转换时很有用处
void LDevelop::enableActions(bool yes)
{
	if(yes){
		fileToolBar->saveAsAct()->setEnabled(true);
		fileToolBar->saveAct()->setEnabled(true);
		editToolBar->setEnabled(true);
		//ReUnDoBar->setEnabled(true);
		compileBar->setEnabled(true);
		//acts.redoAct->setEnabled(true);
		acts.compileAct->setEnabled(true);
		acts.runAct->setEnabled(true);
		acts.runWithParasAct->setEnabled(true);
		acts.cmplAndRunAct->setEnabled(true);
		acts.saveAct->setEnabled(true);
		acts.saveAsAct->setEnabled(true);
		acts.searchAct->setEnabled(true);
		acts.replaceAct->setEnabled(true);
		acts.textFormat->setEnabled(true);
		acts.calculatAct->setEnabled(true);
	}
	else{
		fileToolBar->saveAsAct()->setDisabled(true);
		fileToolBar->saveAct()->setDisabled(true);
		editToolBar->setDisabled(true);
		// ReUnDoBar->setDisabled(true);
		compileBar->setDisabled(true);
		acts.redoAct->setDisabled(true);
		acts.undoAct->setDisabled(true);
		acts.compileAct->setDisabled(true);
		acts.runAct->setDisabled(true);
		acts.runWithParasAct->setDisabled(true);
		acts.cmplAndRunAct->setDisabled(true);
		acts.searchAct->setDisabled(true);
		acts.replaceAct->setDisabled(true);
		acts.saveAct->setDisabled(true);
		acts.saveAsAct->setDisabled(true);
		acts.textFormat->setDisabled(true);
		acts.calculatAct->setDisabled(true);
	}
}

//保存文件
void LDevelop::saveFile()
{
	QString fileName(*curFileName);
	//如果是新文件
    if(fileName==tr("~newfile~")){
		saveAsFile();
        editTab->setTabIcon(curIndex,QIcon(":/images/story-editor.svg"));
		return;
	}
	//如果被修改
	if(!editArea->document()->isModified()){
        statusBar()->showMessage(tr("Sorry, the current file is the latest,\nHave nothing to do"));
		return;
	}
	if(fileName.isEmpty())
		return;
	//写入
	do_SaveFile(&fileName,curIndex);

	return;
}

//另存为
void LDevelop::saveAsFile()
{
	//获取保存位置
	QString fileName(QFileDialog::getSaveFileName(
						 this,
                         tr("Save As.."),
						 *curFileName,
                         tr("All files (*)")));
	//若空
	if(fileName.isEmpty())
		return;
	int index=curIndex;
	QString tmpName=editTab->tabText(index);
	//写入
	do_SaveFile(&fileName,curIndex);
	editArea->setCurrentDoc(&fileName);
	//刷新当前标签页
	changeCurTab(index);
	//如果被修改保存的文件已在当前窗口中打开
	for(int i=0;i<TabsNum;i++)
		if(editTab->tabText(i)==fileName){
			editTab->setCurrentIndex(i);
			editTab->setTabText(index,fileName);
			int ret=QMessageBox::critical(
						this,
                        tr("Load again?"),
                        tr("This document has been changed since it was opened.\n"
                               "Have you decided to change the content from the new load<OK>\n"
                               "Or maintain the current state<NO>?"),
						QMessageBox::No|QMessageBox::Ok
						);
			if(ret==QMessageBox::Ok){
                if(tmpName==tr("~newfile~")||tmpName==tr("~unsaved~")){
					editTab->setCurrentIndex(index);
					editArea->setCurrentDoc(&fileName);
					tabCloseRequested(index);
                    //statusBar()->showMessage(tr("The current file is not saved"));
				}
				if(index<i) i--;

				editTab->setCurrentIndex(i);
				editArea->setCurrentDoc(&fileName);
			}
			else{
				int ti=curIndex;
                editTab->setTabText(curIndex,tr("~unsaved~"));
                editArea->setDocumentTitle(tr("~unsaved~"));
				
				editTab->setCurrentIndex(index);
				editArea->setCurrentDoc(&fileName);
				editTab->setCurrentIndex(ti);
				editArea->document()->setModified(true);
                setWindowFilePath(tr("~unsaved~"));
                statusBar()->showMessage(tr("The current file is not saved"));
			}
			break;
		}
	editTab->setTabText(index,fileName);
	editArea->setDocumentTitle(fileName);
	changeCurTab(curIndex);
}

//鸡肋函数之二，复制文本
void LDevelop::copyEdit()
{
	editArea->copy();
}

//鸡肋函数之三，粘贴文本
void LDevelop::pasteEdit()
{
	editArea->paste();
}

//鸡肋函数之四，剪贴文本
void LDevelop::cutEdit()
{
	editArea->cut();
}

//搜索字符串对话框
void LDevelop::searchText()
{
    inputDialog->setWindowTitle(tr("Search"));
    inputDialog->showExtension(false);
    inputDialog->show();
}

//向下搜索字符串
void LDevelop::downSearch()
{
    //while(editArea->find(tr("text"))>=0){
	/*int n=3;
 while(n--){
  editArea->find(tr("text"));
  QList<QTextEdit::ExtraSelection> selections = editArea->extraSelections();
  QTextEdit::ExtraSelection selection;
  selection.cursor.setPosition(editArea->textCursor().position()-4,QTextCursor::MoveAnchor);
  selection.cursor.setPosition(editArea->textCursor().position(), QTextCursor::KeepAnchor);
  selection.format.setBackground(QBrush(QColor(Qt::red)));
  selections.append(selection);
  editArea->setExtraSelections(selections);
 }*/
	//向前
    if(inputDialog->findBack->isChecked()){
		if(!do_search(QTextDocument::FindBackward))
            QMessageBox::warning(inputDialog,tr("Find failure"),tr("Failed: have gone to file header."));
		return;
    }
	//向后
    else{
		if(!do_search(QTextDocument::FindCaseSensitively)){
            QMessageBox::warning(inputDialog,tr("Find Failure"),tr("Find the failure: the file has been to the end of the file, continue to search from the beginning."));
			QTextCursor tc =editArea->textCursor();

			int position=editArea->document()->findBlockByNumber(0).position();
			tc.setPosition(position,QTextCursor::MoveAnchor);
			editArea->setTextCursor(tc);
		}
		return;
    }
}

//搜索
bool LDevelop::do_search(QTextDocument::FindFlag find)//QString text)
{
	QString text=inputDialog->input->text();
	if(text.isEmpty()){
        statusBar()->showMessage(tr("Warning: no input strings."));
		return false;
	}
	if(!editArea->find(text,find)){
		return false;
	}
	return true;
}

//替换
void LDevelop::do_replace()
{
    int count=0;
    if(inputDialog->replaceAll->isChecked()){
		//  editArea->textCursor().movePosition(QTextCursor::Start);
        while(do_search(QTextDocument::FindCaseSensitively)){
            editArea->insertPlainText(inputDialog->replaceTextEdit->text());
            count++;
        }
        while(do_search(QTextDocument::FindBackward)){
            editArea->insertPlainText(inputDialog->replaceTextEdit->text());
            count++;
        }
        statusBar()->showMessage(tr("Have been found and replaced %1 place(s). ").arg(count));
        return;
    }
    editArea->insertPlainText(inputDialog->replaceTextEdit->text());
}

//搜索并替换字符串
void LDevelop::replaceText()
{
    inputDialog->setWindowTitle(tr("Search And Replace"));
    inputDialog->showExtension(true);
    inputDialog->show();
}

//字数统计小工具
void LDevelop::calculatTool()
{
    QMessageBox::about(this,tr("Word Count"),tr("Number of words：%1").arg(editArea->document()->toPlainText().count()));
}

//关于LDevelop
void LDevelop::about()
{
    QMessageBox::about(this,tr("LDevelop"),
                       tr("A simple code editor from China.\n"));
}

//保存指定标签的文件到指定位置
bool LDevelop::do_SaveFile(QString *fileName,int index)
{
	QFile file(*fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("LDevelop"),
                             tr("Cannot save file %1:\n%2.")
							 .arg(*fileName)
							 .arg(file.errorString()));
		return false;
	}
	QTextStream out(&file);
#ifndef QT_NO_CURSOR
	QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
	out << editTabAreas[index]->toPlainText();
	// out << editArea->toPlainText();
#ifndef QT_NO_CURSOR
	QApplication::restoreOverrideCursor();
#endif
	//QString curName=editTabAreas[index]->documentTitle();
	//  editTabAreas[index]->setDocumentTitle(*fileName);
	// bool ret=editTabAreas[index]->do_save();
	// if(ret){
    statusBar()->showMessage(*fileName+tr(" Saved successfully"),0);
	editTabAreas[index]->document()->setModified(false);
	setWindowModified(false);
    editTab->setTabIcon(index,QIcon(":/images/story-editor.svg"));
	//}

	//      editTab->setWindowFilePath(curName);

	editTabAreas[index]->setDocumentTitle(*fileName);
	return true;//ret;
}

//新建标签页
bool LDevelop::creatNewTab()
{
	//检查是否超过最大标签页数
	if(TabsNum<MaxTabsNum){
		int index=TabsNum;
		TabsNum++;
		if(!index){
			editTab->setTabsClosable(true);
			editTab->removeTab(0);
			enableActions(true);
		}
		//申请一个EditArea
		editTabAreas[index]=new EditArea(options.TextHighlightMode);

		editArea=editTabAreas[index];
		//setWindowModified(false);

		//新建标签，并设置Widget为新申请的EditArea
        editTab->addTab(editArea,tr("~newfile~"));
		editTab->setCurrentIndex(index);

		setWindowModified(false);
		setIndexSignals(index);
        editTab->setTabIcon(index,QIcon(":/images/story-editor.svg"));
		return true;
	}
	//超过最大标签页数
	else{
        QMessageBox::warning(this,tr("Could not create a tab page"),
                             tr(
                                 "Could not continue to create a tab page because"
                                 "The number of your creation has reached an upper limit: 50"));
		return false;
	}
}

//为新申请的EditArea设置信号与槽的连接，以及一些杂碎的配置选项
void LDevelop::setIndexSignals(int index)
{
	editArea->setTabStopWidth(options.TabWidth);
	editArea->viewport()->setStyleSheet(QString("background-color:"+options.BackgroundColor));
	editArea->setFont(options.TextFont);
	//   editArea->document()->setIndentWidth(options.TabWidth);
	editArea->setFrameStyle(0);

    editArea->setDocumentTitle(tr("~newfile~"));

    if(options.AutoSaveTime){
        editTabAreas[index]->editTimer.start(options.AutoSaveTime*60*1000);
        connect(&(editTabAreas[index]->editTimer),SIGNAL(timeout()),editTabAreas[index],SLOT(do_save()));
        connect(&(editTabAreas[index]->editTimer),SIGNAL(timeout()),this,SLOT(autoSave()));
    }

    if(options.HighlightCurrentLine){
        editTabAreas[index]->highlightCurrentLine();
        connect(editTabAreas[index], SIGNAL(cursorPositionChanged()),editTabAreas[index], SLOT(highlightCurrentLine()));
    }
    connect(editTabAreas[index],SIGNAL(textChanged()),this,SLOT(markModified()));
    connect(editTabAreas[index],SIGNAL(undoAvailable(bool)),this,SLOT(undoAvailable(bool)));
    connect(editTabAreas[index],SIGNAL(redoAvailable(bool)),this,SLOT(redoAvailable(bool)));
    connect(editTabAreas[index],SIGNAL(cursorPositionChanged()),this,SLOT(showCurPos()));

    if(options.CurrentLineColor.isValid())
		editTabAreas[index]->setCurLineColor(options.CurrentLineColor);
    editTabAreas[index]->LineNumber=options.LineNumber;
}

//鸡肋函数之五，处理撤销动作的槽
void LDevelop::handleUndo()
{
	editArea=editTabAreas[editTab->currentIndex()];
	editArea->document()->undo();
}

//鸡肋函数之六，处理重做动作的槽
void LDevelop::handleRedo()
{
	editArea=editTabAreas[editTab->currentIndex()];
	editArea->document()->redo();
}

//锁定工具栏
void LDevelop::lockTheBars(bool yes)
{
	if(yes){
		editToolBar->setMovable(false);
		fileToolBar->setMovable(false);
		compileBar->setMovable(false);
		ReUnDoBar->setMovable(false);
		acts.showToolBar->setDisabled(true);
		acts.showMenuBar->setDisabled(true);
	}
	else{
		editToolBar->setMovable(true);
		fileToolBar->setMovable(true);
		compileBar->setMovable(true);
		ReUnDoBar->setMovable(true);
		acts.showToolBar->setEnabled(true);
		acts.showMenuBar->setEnabled(true);
	}
}

//隐藏文件工具栏的槽函数
void LDevelop::hideFileBar()
{
	if(fileToolBar->isHidden()){
        acts.showFileToolBar->setIconVisibleInMenu(true);//setIcon(QIcon(":/images/selected.png"));
		fileToolBar->show();
	}
	else{
		acts.showFileToolBar->setIconVisibleInMenu(false);//setIcon(QIcon(""));
		fileToolBar->hide();
	}
}

//隐藏编辑工具栏的槽
void LDevelop::hideEditBar()
{
	if(editToolBar->isHidden()){
        acts.showEditToolBar->setIconVisibleInMenu(true);//setIcon(QIcon(":/images/selected.png"));
		editToolBar->show();
	}
	else{
		acts.showEditToolBar->setIconVisibleInMenu(false);//setIcon(QIcon(""));
		editToolBar->hide();
	}
}

//隐藏撤销/重做栏
void LDevelop::hideRUBar()
{
	if(ReUnDoBar->isHidden()){
        acts.showRUToolBar->setIconVisibleInMenu(true);//setIcon(QIcon(":/images/selected.png"));
		ReUnDoBar->show();
	}
	else{
		acts.showRUToolBar->setIconVisibleInMenu(false);//setIcon(QIcon(""));
		ReUnDoBar->hide();
	}
}

//隐藏构建栏
void LDevelop::hideBuildBar()
{
	if(compileBar->isHidden()){
		acts.showBuildBar->setIconVisibleInMenu(true);
		compileBar->show();
	}
	else{
		acts.showBuildBar->setIconVisibleInMenu(false);
		compileBar->hide();
	}
}

//处理撤销允许的槽
void LDevelop::undoAvailable(bool yes)
{
	if(yes)
		acts.undoAct->setEnabled(true);
	else{
		acts.undoAct->setDisabled(true);
        editTab->setTabIcon(curIndex,QIcon(":/images/story-editor.svg"));
	}
}

//处理重做允许的槽
void LDevelop::redoAvailable(bool yes)
{
	if(yes)
		acts.redoAct->setEnabled(true);
	else
		acts.redoAct->setDisabled(true);
}

//隐藏菜单栏
void LDevelop::hideMenuBar()
{
	if(menuBar()->isHidden()){
		acts.showMenuBar->setIconVisibleInMenu(true);
		menuBar()->show();
	}
	else{
		acts.showMenuBar->setIconVisibleInMenu(false);
		menuBar()->hide();
	}
}

//LDevelop被关闭时采取的动作，用依次关闭标签的形式来实现
void LDevelop::closeEvent(QCloseEvent *event)
{
	while(TabsNum>0){
		if(editTabAreas[curIndex])
			editArea=editTabAreas[curIndex];
		editTab->setCurrentIndex(curIndex);
		if(tabCloseRequested(curIndex))
			event->accept();
		else{
			event->ignore();
			return;
		}
        //		remove(":/output.LDevelop");
	}
    remove(":/output.LDevelop");
}

//标签被要求关闭时采取的动作
bool LDevelop::tabCloseRequested(int index)
{
	//先转换当前标签页至要关闭的标签页
	changeCurTab(index);
	//editTab->setCurrentIndex(index);
	//editArea=editTabAreas[index];
	//若曾被修改，询问是否保存
	if(handleCurIsModified()){
		//if(!(index==TabsNum-1))
		for(int i=index;i<TabsNum;i++)
			editTabAreas[i]=editTabAreas[i+1];
		//printf("tabsNum:%d\n",TabsNum);
		TabsNum--;
		//fileList->setCurrentRow(index);
		//delete fileList->item(index);
		delete editArea;
		//delete editTimer[index];
        *curFileName=tr("");
		editArea=NULL;
		//curFileName=NULL;
		//若当前标签页数为零，转至欢迎页面
		if(!TabsNum){
			// delete fileList->item(0);
            editTab->addTab(logoLabel,tr("Welcome"));
			editTab->setTabsClosable(false);
			enableActions(false);
            setWindowFilePath(tr("Welcome"));
			outputArea->clear();
            remove(":/output.LDevelop");
		}
		//否则按照下列规则转移视图：
		//若被删除者为最后一个标签页，则转移至前一标签页
		//否则转移至后一个标签页
		else{
			if(index==TabsNum)
				// editTab->setCurrentIndex(index-1);
				changeCurTab(index-1);
			else
				// editTab->setCurrentIndex(index);
				changeCurTab(index);
		}
		//  fileList()
		//fileList->removeItemWidget(fileList->currentItem());
		//fileList->setRowHidden(index,true);
		// fileList->row();
		return true;
	}
	return false;
}

//设置编辑区的字体
void LDevelop::setFont()
{
	int size=fontSizes->checkedAction()->text().toInt();
	QString fontFam=fonts->checkedAction()->text();
	for(int i=0;i<TabsNum;i++){
		if(!editTabAreas[i])
			continue;
		editArea=editTabAreas[i];
		editArea->setFont(QFont(fontFam,size,24,false));
	}
	editArea=editTabAreas[curIndex];
}

//改变当前标签页，处理由此产生的一系列问题
void LDevelop::changeCurTab(int index)
{
	if(editTab->currentIndex()<0||index<0){
		setWindowModified(false);
		return;
	}
	//  fileList->setCurrentRow(index);
	curIndex=index;
	*curFileName=editTab->tabText(index);//editTabAreas[index]->documentTitle();
	editArea=editTabAreas[index];
	// fileList->setCurrentRow(index);
	
	setWindowFilePath(*curFileName);
	if(editArea&&TabsNum){
        //  QMessageBox::warning(this,tr("title"),editArea->documentTitle());
		if(editArea->document()->isModified())//{
			markModified();
		//		setWindowModified(true);
        //		editTab->setTabIcon(index,QIcon("~/.config/LDevelop/new.svg"));
		//	}
		else{
			setWindowModified(false);
            editTab->setTabIcon(index,QIcon(":/images/story-editor.svg"));
		}
	}
}

//显示当前光标位置
void LDevelop::showCurPos()
{
	QTextCursor cursor = editArea->textCursor();
	int col = cursor.columnNumber();
	int row = cursor.blockNumber();
    QString pos(tr("\t\t\t\t\t\trow%1, column%2").arg(row+1).arg(col+1));
	statusBar()->showMessage(pos);
}

//标识被修改
void LDevelop::markModified()
{
	setWindowModified(editArea->document()->isModified());
    editTab->setTabIcon(curIndex,QIcon(":/images/document-new.svg"));
}

//全屏显示
void LDevelop::showFullScreen(bool yes)
{
	if(yes)
		QMainWindow::showFullScreen();
	else
		showNormal();
}

int closeFile(int fd)
{
	return close(fd);
}

void LDevelop::autoSave()
{
	//    do_SaveFile(editTab->tabText());
    statusBar()->showMessage(tr("The document is automatically saving...Success"));
	// if(!edit)
    setWindowModified(false);
}

//编译C/C++程序
void LDevelop::build()
{
	//若未保存
	if(!handleCurIsModified()){
        statusBar()->showMessage(tr("File not saved, nothing to do"));
		return;
	}

    if(!curFileName->isEmpty()){//&&curFileName!=tr("~newfile~")){
		//若为未修改的新文件
        if(curFileName==tr("~newfile~")){
            QMessageBox::warning(this,tr("Error"),
                                 tr("Error: new file not changed ~newfile~, nothing to do."));
			return;
		}
		QString ext,elf; //文件名、扩展名
		QStringList ret=splitFilePath(*curFileName);
		elf=ret.first();
		ext=ret.last();
		int fd;
		//打开编译输出文件并清空
        if((fd=open(":/output.LDevelop",O_CREAT|O_RDWR,0777))==-1){
            QMessageBox::warning(this,tr("Cannot build"),
                                 tr("Cannot open the output file, the compiler is terminated."));
			return;
		}
		//重定向标准输出，使编译消息输出至文件中
		if(dup2(fd,STDOUT_FILENO)==-1||dup2(fd,STDERR_FILENO)==-1){
            QMessageBox::warning(this,tr("Cannot build"),
                                 tr("Unable to redirect standard output, compile terminated."));
			return;
		}
		//清空
		if(ftruncate(fd,0)){}

        outputArea->setPlainText(tr("***Build Starting***\n"));
        //QMessageBox::warning(this,tr("df"),elf);
        //QMessageBox::warning(this,tr("df"),ext);
		//构建开始
        if(ext==tr("cpp"))		//C++程序
            QProcess::execute(tr("c++ -Wall ")+curFileName->toUtf8()+tr(" -o ")+elf);
        else if(ext==tr("c"))		//C程序
            QProcess::execute(tr("gcc -Wall ")+curFileName->toUtf8()+tr(" -o ")+elf);
		else{					//文件后缀错误
            outputArea->appendPlainText(tr("***Compile error, give up.***"));
            QMessageBox::warning(this,tr("Error"),
                                 tr("Error: file format error, compile terminated."));
		}
		if(closeFile(fd)==-1)
            QMessageBox::warning(this,tr("Warning"),
                                 tr("Warning: File closing error."));
	}
	else
        QMessageBox::warning(this,tr("File is empty"),
                             tr("File name is empty, the compiler is terminated."));
	setOutputDoc();
    outputArea->appendPlainText(tr("****Finished****\n"));
}

//设置构建输出区域显示的消息
void LDevelop::setOutputDoc()
{
    QFile file(":/output.LDevelop");
	if(file.open(QFile::ReadOnly|QFile::Text)){
		QTextStream in(&file);
#ifndef QT_NO_CURSOR
		QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
		outputArea->appendPlainText(in.readAll());
#ifndef QT_NO_CURSOR
		QApplication::restoreOverrideCursor();
#endif
	}

}

//运行程序
void LDevelop::run()
{
	QString ext,elf;
	QStringList ret=splitFilePath(*curFileName);
	elf=ret.first();
	ext=ret.last();

	QFile file(elf);

	if(!file.exists()){
        QMessageBox::warning(this,tr("Error"),
                             tr("Error:file %1 does not exist，Cannot be executed.\nYou might have to compile this file first.").arg(elf));
		return;
	}

    int fd=open(":/output.LDevelop",O_CREAT|O_RDWR,0777);
	if(fd==-1){
        QMessageBox::warning(this,tr("Cannot run"),
                             tr("Cannot create file, run failed."));
		return;
	}
	//重定向标准输出
	if(dup2(fd,STDOUT_FILENO)==-1||dup2(fd,STDERR_FILENO)==-1){
        QMessageBox::warning(this,tr("Cannot be executed"),
                             tr("Unable to redirect standard output to perform termination."));
		return;
	}
//	if(ftruncate(fd,0)){}

    outputArea->setPlainText(tr("***Run Starting***\n"));
	QString paras=parasInput->textValue();
    if(QProcess::execute(tr("xterm -e ")+elf.toUtf8()+tr(" ")+paras)<0){
        QMessageBox::critical(this,tr("Error"),
                              tr("Program execution error, exception exit."));
        outputArea->appendPlainText(tr("***Exception Exit***\n"));
	}

	setOutputDoc();
    outputArea->appendPlainText(tr("****Finished****\n"));
}

//将所给的字符串分割为文件名与后缀名并以QStringList的形式返回
QStringList LDevelop::splitFilePath(QString filePath)
{
    char elfFile[120];
    char extName[8];
    strcpy(elfFile,filePath.toUtf8().data());
    for(int i=strlen(elfFile)-1;i>=0;i--)
		if(elfFile[i]=='.'){
			strcpy(extName,&elfFile[i+1]);
			elfFile[i]='\0';
			break;
		}
    QStringList ret;
    ret<<elfFile<<extName;
    //QMessageBox::warning(this,tr("df"),ret.first());
    //QMessageBox::warning(this,tr("df"),ret.last());
    return ret;
}

//带参运行
void LDevelop::run_with_paras()
{
    parasInput->setWindowTitle(tr("Run with parameters"));
    parasInput->setLabelText(tr("Input parameters:"));
    parasInput->setOkButtonText(tr("Run"));
    parasInput->setCancelButtonText(tr("Close"));
    parasInput->resize(400,32);
    parasInput->show();
}

//构建并运行
void LDevelop::build_and_run()
{
	build();
	run();
}
