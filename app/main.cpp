#include"ldevelop.h"
#include <QApplication>
#include <QMainWindow>
#include <QTranslator>

int main(int argc,char **argv)
{
	QApplication app(argc,argv);
    //QTextCodec::setCodecForTr(QTextCodec::codecForName("gb18030"));
	QTranslator translator;
	translator.load("/usr/share/qt4/translations/qt_zh_CN.qm");
	app.installTranslator(&translator); 
	LDevelop main;
	QString file;
	for(int i=1;i<argc;i++){
		file=QString(argv[i]);
		main.loadFile(file);
	}
	main.show();
	return app.exec();
}
