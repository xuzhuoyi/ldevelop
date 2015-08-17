/****************************LDevelop********************************
*********Copyright @ 吴平杰 (Pingjie Wu) from China,2011*********
*******************************************************************/
/*
  几个关键数据结构的定义。有菜单选项的结构体muneActions，用来
  更有条理地组织菜单里的各个动作。configOptions则是配置选项的集合，
  是提供自定义配置功能的保障。
*/

#ifndef STRUCT_H
#define STRUCT_H
#include<QAction>
#include<QColor>
#include<QMenu>
#include<QString>


typedef struct menuActions{
		QAction *newAct;			//新建文件
		QAction *openAct;			//打开文件
		QAction *saveAct;			//保存文件
		QAction *saveAsAct;		//保存到
		QAction *exitAct;			//退出

		QAction *undoAct;			//撤销
		QAction *redoAct;			//重做
		QAction *searchAct;			//搜索
		QAction *replaceAct;		//替换
		QAction *preferencesAct;		//首选项

		QAction *compileAct;		//编译
		QAction *runAct;			//运行
		QAction *runWithParasAct;	//带参运行
		QAction *cmplAndRunAct;	//编译并运行

		QAction *showFullScreen;	//全屏显示
		QMenu *showToolBar;		//显示工具栏
		QAction *showEditToolBar;	//显示编辑工具栏
		QAction *showFileToolBar;	//显示文件工具栏
		QAction *showRUToolBar;	//显示撤销/重做栏
		QAction *showMenuBar;		//显示菜单栏
		QAction *showBuildBar;		//显示构建工具栏
		QAction *showStatusBar;	//显示状态栏
		QAction *lockBars;			//锁定工具栏

		QMenu *textFormat;		//文本格式
		QMenu *font;				//字体
		QMenu *size;				//字号

		QAction *calculatAct;		//字数统计
		QAction *aboutAct;			//关于
		QAction *aboutQtAct;		//关于Qt

}menuActions;

typedef struct configOptions{
	int ProgramMode;				//工作模式
	int TextHighlightMode;			//高亮显示模式
	QString BackgroundColor;		//背景颜色
	QFont TextFont;				//文本字体
	int TextFontStyle;				//字体风格
	int TextSize;					//字体大小
	int TabWidth;					//Tab键
	bool AutoIndent;				//自动缩进（未实现）
	bool SetBackup;				//自动备份（未实现）
	int AutoSaveTime;				//自动保存时间
	bool LockBar;					//锁定工具栏
	bool OutputAreaVisiable;			//输出面板可见
	bool FileTreeVisiable;			//树形文件可见
	bool FileBar;					//文件工具栏
	bool EditBar;					//编辑工具栏
	bool ReUnDoBar;				//撤销/重做工具栏
	bool BuildBar;					//构建工具栏
	bool StatusBar;				//状态栏
	bool AutoChangeLine;			//自动换行
	bool LineNumber;				//显示行号
	bool HighlightCurrentLine;		//突出当前行
	QColor CurrentLineColor;		//当前行颜色
}configOptions;

#endif
