/****************************LDevelop********************************
*********Copyright @ 吴平杰 (Pingjie Wu) from China,2011*********
*******************************************************************/
/*
  LDevelop.h 定义了LDevelop类。继承自QMainWindow。
  主程序的框架。
*/

#ifndef LDevelop_H
#define LDevelop_H

#include<QMainWindow>
#include<QWidget>
#include<QTabWidget>

#include"Structs.h"
#include"EditArea.h"
#include"ToolBars.h"
#include"ConfigDialog.h"
#include"SearchDialog.h"

#define MAXTABSNUM 50

class QInputDialog;
class QDirModel;
class QTreeView;


class LDevelop:public QMainWindow
{
	Q_OBJECT
	public:
		LDevelop();
		QString* getCurFileName();
		bool do_SaveFile(QString *,int);
		void loadFile(QString);
		void setupOption(QString);
		void readConfig(QString);
		void setConfig(configOptions,QString);
		QString setOption(QString,configOptions);
	private:
	//	void loadFile(QString);
		bool do_search(QTextDocument::FindFlag);
		void setupOptions();
		void setupToolBars();
		void setupMenus();
		void setupFileList();
		void setupDirView();
		void setupSignals();
		void setIndexSignals(int);
		void setupOutput();
		void setOutputDoc();
		bool handleCurIsModified();
		bool creatNewTab();
		void closeEvent(QCloseEvent *);
		void enableActions(bool);
		QStringList splitFilePath(QString);
		void readConfigFromTable();
		int TabsNum;
		int MaxTabsNum;
		int curIndex;
		
		QString *curFileName;
		QTabWidget *editTab;
		EditArea *editTabAreas[MAXTABSNUM];
		EditArea *editArea;
		FileToolBars *fileToolBar;
		EditToolBars *editToolBar;
		QToolBar *ReUnDoBar;
		QToolBar *compileBar;
		QLabel *logoLabel;

		QActionGroup *fonts;
		QActionGroup *fontSizes;
		QMenu *fileMenu;
		QMenu *editMenu;
		QMenu *viewMenu;
		QMenu *toolsMenu;
		QMenu *helpMenu;
		menuActions acts;

		QInputDialog *parasInput;
		configOptions options;
		ConfigDialog *configDialog;
		SearchDialog *inputDialog;

		QPlainTextEdit *outputArea;
		QDirModel *dirModel;
		QTreeView *treeView;
		QListWidget *fileList;
		QDockWidget *treeDock;
		QDockWidget *outputDock;

	private slots:
		void newFile();
		void saveFile();
		void openFile();
		void openTreeFile();
		void saveAsFile();
		void copyEdit();
		void pasteEdit();
		void cutEdit();
		//  void upSearch();
		void downSearch();
		void handleUndo();
		void handleRedo();
		void searchText();
		void replaceText();
		void do_replace();
		void autoSave();

		void do_writeConfig();

		void hideFileBar();
		void hideEditBar();
		void hideRUBar();
		void hideBuildBar();
		void hideMenuBar();
		void showFullScreen(bool);
		void lockTheBars(bool);
		void undoAvailable(bool);
		void redoAvailable(bool);
		void showCurPos();
		void editPrefOptions();
		void setFont();
		bool tabCloseRequested(int);
		void markModified();
		void appendText();
		void changeCurTab(int);
		void onFileListClicked(int);
		void build();
		void run();
		void build_and_run();
		void run_with_paras();
		void calculatTool();
		void about();
};

#endif
