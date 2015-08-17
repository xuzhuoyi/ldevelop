/****************************LDevelop********************************
*********Copyright @ 吴平杰 (Pingjie Wu) from China,2011*********
*******************************************************************/

/*
编辑区 类EditArea的定义在EditArea.h中，主要实现以下功能：
 1、语法、关键字高亮;
 2、显示编辑区行号;
 3、突出显示当前行;
 4、保存当前文档到指定位置;
 都是可选的，用户可以配置成不使用这些功能。
 功能4是额外的，主要用于实现文档的自动保存功能，但这个功能有点鸡肋，所以默认并不开启。
*/

#include"EditArea.h" 

#include <QTextStream>
#include <QApplication>
#include <QPainter>
#include <QMessageBox>


//语法高亮类，从QSyntaxHighlighter继承而来
//主要是C/C++语法的高亮，还有Qt风格的类型高亮
//如果要扩充额外支持的语法，如JAVA、HTML、makefile、C#，
//可以通过修改、扩充此类来完成。

//Highlighter构造函数传递的mode参数来自于配置文件，即语法高亮模式
Highlighter::Highlighter(QTextDocument *parent,int mode)
	: QSyntaxHighlighter(parent)
{
	HighlightRule rule;						//高亮规则

	QTextCharFormat sharpFormat;			//预定义与宏语法的高亮
	sharpFormat.setForeground(Qt::darkBlue);	//设置为深蓝
	rule.pattern=QRegExp("#[^\n]*");			//用正则表达式来提取字符串
	rule.format=sharpFormat;
	highlightRules.append(rule);

	QTextCharFormat keywordFormat;			//C/C++关键字高亮
	keywordFormat.setForeground(Qt::darkGreen);//深绿色
	keywordFormat.setFontWeight(QFont::Bold);	//粗体
	QStringList keywordPatterns;
	keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
					<< "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
					<< "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
					<< "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
					<< "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
					<< "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
					<< "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
					<< "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
					<< "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
					<< "\\bvoid\\b" << "\\bvolatile\\b" <<"\\bbool\\b"
					<< "\\binline\\b" << "\\bnew\\b" <<"\\bnew\\b" <<"\\bdelete\\b"
					<< "\\btrue\\b" <<"\\bfalse\\b"<<"\\bthis\\b";
	foreach(const QString &pattern,keywordPatterns){
		rule.pattern=QRegExp(pattern);
		rule.format=keywordFormat;
		highlightRules.append(rule);
	}

	QTextCharFormat functionFormat;				//函数式的高光
	functionFormat.setFontItalic(false);
	functionFormat.setForeground(Qt::darkGray);		//深灰
	functionFormat.setFontWeight(QFont::Bold);		//粗体
	rule.pattern=QRegExp("\\b[A-Za-z0-9_]+(?=\\()");	//也是用正则表达式
	rule.format=functionFormat;
	highlightRules.append(rule);


	QTextCharFormat logicKeywordFormat;			//逻辑链接词
	logicKeywordFormat.setForeground(Qt::darkMagenta);
	logicKeywordFormat.setFontWeight(QFont::Bold);
	QStringList logicKeywordPatterns;
	logicKeywordPatterns<< "\\bif\\b" <<"\\belse\\b" <<"\\bwhile\\b"
						<< "\\bdo\\b" <<"\\bfor\\b" <<"\\bforeach\\b"
						<< "\\bgoto\\b" <<"\\breturn\\b" <<"\\bsizeof\\b"
						<< "\\bcontinue\\b" <<"\\bbreak\\b" <<"\\bor\\b"
						<< "\\bconst\\b" <<"\\bcase\\b"<<"\\bswitch\\b";
	foreach(const QString &pattern,logicKeywordPatterns){
		rule.pattern=QRegExp(pattern);
		rule.format=logicKeywordFormat;
		highlightRules.append(rule);
	}

	if(mode==1){		//这个判断条件用来区分高亮语法模式，C/C++或者Qt，可以扩充以支持其它语法
		QTextCharFormat classFormat;
		//classFormat.setFontWeight(QFont::Bold);
		classFormat.setForeground(Qt::darkMagenta);
		rule.pattern=QRegExp("\\bQ[A-Za-z]+\\b");
		rule.format=classFormat;
		highlightRules.append(rule);
	}

	/*singleLineCommentFormat.setForeground(Qt::darkBlue);
 rule.pattern=QRegExp("//[^\n]*");
 rule.format=singleLineCommentFormat;
 highlightRules.append(rule);
*/
	//just a test for the "/* */" 
	//QTextCharFormat testFormat;
	//testFormat.setForeground(Qt::darkBlue);
	//rule.pattern=QRegExp("/*[^*]*/");
	//rule.format=testFormat;
	//highlightRules.append(rule);


	//multiLineCommentFormat.setForeground(Qt::blue);

	QTextCharFormat quotationFormat;				//字符串语法
	quotationFormat.setForeground(Qt::darkGreen);
	rule.pattern=QRegExp("\".*\"");
	rule.format=quotationFormat;
	highlightRules.append(rule);

	//  if(mode==0){
	QTextCharFormat sharpKuoFormat;				//头文件包含语法
	sharpKuoFormat.setForeground(Qt::darkGreen);
	rule.pattern=QRegExp("<.*>");
	rule.format=sharpKuoFormat;
	highlightRules.append(rule);
	// }

	/*QTextCharFormat functionFormat;
 functionFormat.setFontItalic(true);
 functionFormat.setForeground(Qt::darkGray);
 rule.pattern=QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
 rule.format=functionFormat;
 highlightRules.append(rule);*/


	QTextCharFormat singleLineCommentFormat;		//单行注释语法
	singleLineCommentFormat.setFontItalic(true);
	singleLineCommentFormat.setForeground(Qt::darkBlue);//深蓝色
	rule.pattern=QRegExp("//[^\n]*");
	rule.format=singleLineCommentFormat;
	highlightRules.append(rule);



	commentStartExpression=QRegExp("/\\*");		//多行注释
	commentEndExpression=QRegExp("\\*/");

	multiLineCommentFormat.setForeground(Qt::darkBlue);//也是深蓝色
	multiLineCommentFormat.setFontItalic(true);
}

//QSyntaxHighlighter类的虚函数，用以实现将匹配的
//字符串用匹配的高亮规则实现高亮
void Highlighter::highlightBlock(const QString &text)
{
	foreach(const HighlightRule &rule,highlightRules){
		QRegExp expression(rule.pattern);
		int index=expression.indexIn(text);
		while(index>=0){
			int length=expression.matchedLength();
			setFormat(index,length,rule.format);
			index=expression.indexIn(text,index+length);
		}
	}
	setCurrentBlockState(0);

	int startIndex=0;
	if(previousBlockState()!=1)
		startIndex=commentStartExpression.indexIn(text);
	while(startIndex>=0){
		int endIndex=commentEndExpression.indexIn(text,startIndex);
		int commentLenth;
		if(endIndex==-1){
			setCurrentBlockState(1);
			commentLenth=text.length()-startIndex;//+commentEndExpression.matchedLength();
		}
		else
			commentLenth=endIndex-startIndex+commentEndExpression.matchedLength();

		setFormat(startIndex,commentLenth,multiLineCommentFormat);
		startIndex=commentStartExpression.indexIn(text,startIndex+commentLenth);
	}
}


//EditArea构造函数实现，mode参数是传递给Hightlighter用的
//区别高亮语法
EditArea::EditArea(int mode)
{
	QFont font("Monospace",12,24,false);			//默认字体
	setFont(font);
	lineNumberArea=new LineNumberArea(this);		//显示行号用的Widget
	if(mode>=0)								//也是判别高亮语法模式
		highlighter=new Highlighter(this->document(),mode);
	//QFile file("EditArea.cpp");
	//if (file.open(QFile::ReadOnly | QFile::Text))
	//	this->setPlainText(file.readAll());
	curLineColor=QColor(Qt::lightGray).light(120);	//突出显示当前行的默认颜色
	setViewportMargins(38,0,0,0);					//给行号区域让出地方
	setWordWrapMode(QTextOption::WrapAnywhere);
	//     highlightCurrentLine();
    //    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
	//当窗体更新时，更新行号区域
}

//setCurrentDoc函数，从给定的路径设置当前文档
//若给出空路径，则生成一个名为~newfile~的新文档
bool EditArea::setCurrentDoc(QString *orgName)
{
	QString *name=new QString(*orgName);
	setDocumentTitle(*name);
    if(*name==tr("~newfile~"))
		return false;
	QFile file(*name);
	if(file.open(QFile::ReadOnly|QFile::Text)){

		QTextStream in(&file);
		//if(in.codec()->name())
		//QMessageBox::warning(this,0,in.codec()->name());
#ifndef QT_NO_CURSOR
		QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
		this->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
		QApplication::restoreOverrideCursor();
#endif
		//		this->setPlainText(file.readAll());
		return true;
	}
	return false;
}

//突出显示当前行
void EditArea::highlightCurrentLine()
{
    //setCurLineColor(curLineColor);
	QList<QTextEdit::ExtraSelection> extraSelections;
	if (!isReadOnly()) {
		QTextEdit::ExtraSelection selection;
		QColor lineColor = QColor(curLineColor);
		selection.cursor = textCursor();
		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		//selection.cursor.clearSelection();
		extraSelections.append(selection);
	}
	setExtraSelections(extraSelections);
}

//设置突出显示当前行用的颜色
void EditArea::setCurLineColor(QColor color)
{
	curLineColor=color.light(180);
	highlightCurrentLine();
}

//更新行号区域，用于窗体发生变化时
void EditArea::updateLineNumberArea(const QRect &rect,int dy)
{
	if(dy)
		lineNumberArea->scroll(0,dy);
	else
		lineNumberArea->update(0,rect.y(),lineNumberArea->width(),rect.height());
	//	if(rect.contains(viewport()->rect()))
	///		;//updateLineNumberArea
}

//调整行号区域Widget的位置
void EditArea::resizeEvent(QResizeEvent *event)
{
	QPlainTextEdit::resizeEvent(event);
	QRect cr = contentsRect();
	lineNumberArea->setGeometry(QRect(cr.left(), cr.top(),36, cr.height()));

}

//重写行号区
void EditArea::drawLineNumber(QPaintEvent *event)
{
    if(LineNumber){
		//	QPlainTextEdit plainEdit(this);
        //	setPlainText(tr("1"));
		//	QTextBlock block=firstVisibleBlock();
		//	printf("%d",block.blockNumber());
		QPainter painter(lineNumberArea);
		painter.fillRect(event->rect(), QColor(Qt::lightGray).light(125));

		QTextBlock block = firstVisibleBlock();
		int blockNumber = block.blockNumber();
		int top=(int)blockBoundingGeometry(block).translated(contentOffset()).top();
		int bottom=top + (int) blockBoundingRect(block).height();

		while (block.isValid() && top <= event->rect().bottom()) {
			if (block.isVisible() && bottom >= event->rect().top()) {
				QString number = QString::number(blockNumber + 1);
				painter.setPen(QColor(Qt::black).light(120));
                painter.setFont(QFont(tr("Monospace")));
				painter.drawText(0, top, lineNumberArea->width(),fontMetrics().height(),Qt::AlignRight, number);
			}
            block = block.next();
            top = bottom;
            bottom = top + (int) blockBoundingRect(block).height();
            ++blockNumber;
        }
    }
}


//保存当前文档到原来位置，主要用于自动保存功能
bool EditArea::do_save()
{
    QString fileName(documentTitle());
    if(fileName==tr("~newfile~")||!document()->isModified())
        return false;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("LDevelop"),
                             tr("Cannot save file %1:\n%2.")
							 .arg(fileName)
							 .arg(file.errorString()));
		return false;
    }
    QTextStream out(&file);

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << this->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    this->document()->setModified(false);
    return true;
}
