/****************************LDevelop********************************
*********Copyright @ 吴平杰 (Pingjie Wu) from China,2011*********
*******************************************************************/
/*
配置首选项对话框，ConfigDialog.cpp
*/

#include"ConfigDialog.h"
#include"ConfigPages.cpp" 

//构造函数，需要传递一个 configOptions 参数，用以标识初始配置选项
//这个 configOptions 一般是程序启动时从配置文件(config.LDevelop)所读取的
ConfigDialog::ConfigDialog(configOptions options,QWidget *parent)
    :QDialog(parent)
{
	//选项卡，选择当前配置页面。这是由一个QListWidget对象实现的
    contentsWidget = new QListWidget;
    contentsWidget->setViewMode(QListView::IconMode);
    contentsWidget->setIconSize(QSize(96, 84));
    contentsWidget->setMovement(QListView::Static);
    contentsWidget->setMaximumWidth(128);
    contentsWidget->setSpacing(12);

    pagesWidget = new QStackedWidget;

	//添加配置页面，“编辑器”、“视图”以及“查看”，以对配置选项进行分类
	//editConfigPage viewConfigPage以及lookConfigPage的定义在ConfigPages.h中
	editPage=new editConfigPage(options);
	viewPage=new viewConfigPage(options);
	lookPage=new lookConfigPage(options);

	pagesWidget->addWidget(editPage);
	pagesWidget->addWidget(viewPage);
	pagesWidget->addWidget(lookPage);

	//添加关闭与接受配置按钮
    QPushButton *closeButton = new QPushButton(tr("Close"));
    okButton=new QPushButton(tr("OK"));

	//创建目录图标
    createIcons();
	contentsWidget->setCurrentRow(0);//当前选项卡

    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

	//对话框布局
    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(contentsWidget);
    horizontalLayout->addWidget(pagesWidget, 1);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(closeButton);
	buttonsLayout->addWidget(okButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Preference"));
}

//createIcons函数，创建图标式目录
void ConfigDialog::createIcons()
{
    QListWidgetItem *configButton = new QListWidgetItem(contentsWidget);
    configButton->setIcon(QIcon(":/images/configure.svg"));
    configButton->setText(tr("Editor"));
    configButton->setTextAlignment(Qt::AlignHCenter);
    configButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *updateButton = new QListWidgetItem(contentsWidget);
    updateButton->setIcon(QIcon(":/images/update.png"));
    updateButton->setText(tr("Packages"));
    updateButton->setTextAlignment(Qt::AlignHCenter);
    updateButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *queryButton = new QListWidgetItem(contentsWidget);
    queryButton->setIcon(QIcon(":/images/query.png"));
    queryButton->setText(tr("View"));
    queryButton->setTextAlignment(Qt::AlignHCenter);
    queryButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	//点击图标转换视图
    connect(contentsWidget,
            SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
}

//转换选项卡页面SLOT槽
void ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    pagesWidget->setCurrentIndex(contentsWidget->row(current));
}

