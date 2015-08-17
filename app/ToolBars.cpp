/****************************LDevelop********************************
*********Copyright @ 吴平杰 (Pingjie Wu) from China,2011*********
*******************************************************************/
/*
  编辑工具栏与文件工具栏的单独实现，这是一大败笔。因为刚开始写没有考虑周全，
  轻率地将其单独实现。要改也未尝不可，只是牵一发而动全身，
  有点麻烦，所以只好将就一下了。
*/

#include"ToolBars.h"

FileToolBars::FileToolBars()
{
    newFileAct=new QAction(QIcon(":/images/document-new.svg"),tr("New File"),this);
    newFileAct->setStatusTip(tr("Create a new file"));
	
    openFileAct=new QAction(QIcon(":/images/document-open.svg"),tr("Open"),this);
    openFileAct->setStatusTip(tr("Open a file"));

    saveFileAct=new QAction(QIcon(":/images/document-save.svg"),tr("Save"),this);
    saveFileAct->setStatusTip(tr("Save current file"));

    saveAsFileAct=new QAction(QIcon(":/images/document-save-as.svg"),tr("Save As.."),this);
    saveAsFileAct->setStatusTip(tr("Save the current file to the specified location"));

	this->addAction(newFileAct);
	this->addAction(openFileAct);
	this->addAction(saveFileAct);
	this->addAction(saveAsFileAct);
}

QAction* FileToolBars::newAct()
{
	return newFileAct;
}

QAction* FileToolBars::openAct()
{
	return openFileAct;
}

QAction* FileToolBars::saveAct()
{
	return saveFileAct;
}

QAction* FileToolBars::saveAsAct()
{
	return saveAsFileAct;
}



EditToolBars::EditToolBars()
{
        copyEditAct=new QAction(QIcon(":/images/edit-copy.svg"),tr("Copy"),NULL);
        copyEditAct->setStatusTip(tr("Copy selected text"));
		
        pasteEditAct=new QAction(QIcon(":/images/edit-paste.svg"),tr("Paste"),this);
        pasteEditAct->setStatusTip(tr("Paste at current position"));

        cutEditAct=new QAction(QIcon(":/images/edit-cut.svg"),tr("Cut"),this);
        cutEditAct->setStatusTip(tr("Cut selected text"));

		this->addAction(copyEditAct);
		this->addAction(pasteEditAct);
		this->addAction(cutEditAct);
}

QAction* EditToolBars::copyAct()
{
	return copyEditAct;
}

QAction* EditToolBars::pasteAct()
{
	return pasteEditAct;
}

QAction* EditToolBars::cutAct()
{
	return cutEditAct;
}
