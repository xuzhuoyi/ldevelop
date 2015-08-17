/****************************LDevelop********************************
*********Copyright @ 吴平杰 (Pingjie Wu) from China,2011*********
*******************************************************************/
/*
  编辑工具栏与文件工具栏单独实现的头文件。包含EditToolBars与FileToolBars类定义
*/

#ifndef TOOLBAR_H
#define TOOLBAR_H

#include<QToolBar>
#include<QAction>

class FileToolBars:public QToolBar
{
	Q_OBJECT
	public:
		FileToolBars();
		QAction* newAct();
		QAction* openAct();
		QAction* saveAct();
		QAction* saveAsAct();
	private:
		QAction *newFileAct;
		QAction *openFileAct;
		QAction *saveFileAct;
		QAction *saveAsFileAct;
};


class EditToolBars:public QToolBar
{
	Q_OBJECT
	public:
		EditToolBars();
		QAction* copyAct();
		QAction* pasteAct();
		QAction* cutAct();
	private:
		QAction *copyEditAct;
		QAction *pasteEditAct;
		QAction *cutEditAct;
};

#endif 
