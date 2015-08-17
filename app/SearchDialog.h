/****************************LDevelop********************************
*********Copyright @ 吴平杰 (Pingjie Wu) from China,2011*********
*******************************************************************/
/*
  搜索与替换对话框的类定义。继承自QDialog
*/
#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include<QDialog>
#include<QPushButton>

class SearchDialog:public QDialog
{
	private:
                QWidget *extension;

                QHBoxLayout *hLayout;
                QVBoxLayout *extVLayout;
               // QDialogButtonBox *buttonBox;
                void setupSignals();

        public:
                SearchDialog(QWidget *);
                QLineEdit *input;
                QLineEdit *replaceTextEdit;
                QPushButton *findNext;
                QCheckBox *findBack;
                QPushButton *exitButton;

                QCheckBox *wholeWord;
                QCheckBox *replaceAll;
                QPushButton *replaceButton;

                void showExtension(bool);
};

#endif
