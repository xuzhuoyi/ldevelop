/****************************LDevelop********************************
*********Copyright @ 吴平杰 (Pingjie Wu) from China,2011*********
*******************************************************************/
/*
  EditArea.h头文件，包含Highlighter、EditArea、LineNumberArea类的定义
  这些类综合起来实现了一个语法高亮、行号、突出当前行等功能的文本编辑区域
  本来还希望实现自动缩进、自动补全等高级一点的功能，可惜目前能力尚有点欠缺
  所以只是给了一个框架，留待以后慢慢实现吧。
*/
#ifndef EDITAREA_H 
#define EDITAREA_H

#include <QTextEdit>
#include <QFile>
#include <QSyntaxHighlighter>
#include <QHash>
#include <QTextCharFormat>
#include <QPlainTextEdit>
#include <QTimer>


class Highlighter:public QSyntaxHighlighter
{
	Q_OBJECT
	public:
		Highlighter(QTextDocument *parent=0,int mode=0);
	protected:
		void highlightBlock(const QString &);
	private:
		struct HighlightRule{
			QRegExp pattern;
			QTextCharFormat format;
		};
		QVector<HighlightRule> highlightRules;
		QRegExp commentStartExpression;
		QRegExp commentEndExpression;

		QTextCharFormat multiLineCommentFormat;

};


//EditArea类继承自QPlainTextEdit，而非QTextEdit
//这样可以更方便地控制编辑区的一些特殊功能，比如
//显示行号。
class EditArea:public QPlainTextEdit
{
	Q_OBJECT 
	public:
                EditArea(int=0);
                bool LineNumber;
                QTimer editTimer;
		bool setCurrentDoc(QString *);
		void setCurLineColor(QColor=QColor(Qt::lightGray).light(120));
	public slots:
		void highlightCurrentLine();
		void drawLineNumber(QPaintEvent *);
		void resizeEvent(QResizeEvent *);
		void updateLineNumberArea(const QRect &,int);
                bool do_save();
	private:
		Highlighter *highlighter;
		QColor curLineColor;
		QWidget *lineNumberArea;
	//public:
		//void drawLineNumber();
};


//显示行号区域的Widget
class LineNumberArea : public QWidget
{
	public:
    		LineNumberArea(EditArea *editor) : QWidget(editor) {
				codeEditor = editor;
			}

	protected:
		void paintEvent(QPaintEvent *event) {
        		codeEditor->drawLineNumber(event);
			}

	private:
    		EditArea *codeEditor;
};

#endif
