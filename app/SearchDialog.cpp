/****************************LDevelop********************************
*********Copyright @ 吴平杰 (Pingjie Wu) from China,2011*********
*******************************************************************/
/*
  搜索与替换对话框的成员函数实现。
*/

#include "SearchDialog.h"
#include <QHBoxLayout>
#include <QLineEdit>

SearchDialog::SearchDialog(QWidget *parent)
    :QDialog(parent)
{
    setWindowTitle(tr("Find"));
    input=new QLineEdit;
    input->setFixedSize(320,28);
    replaceTextEdit=new QLineEdit;
    replaceTextEdit->setFixedSize(320,28);
    findBack=new QCheckBox(tr(" Find Backward"));
    findNext=new QPushButton(tr("Find"));

   // buttonBox=new QDialogButtonBox(Qt::Vertical);
    //buttonBox->addButton(findBack, QDialogButtonBox::ActionRole);
    //buttonBox->addButton(findNext, QDialogButtonBox::ActionRole);

    wholeWord=new QCheckBox(tr("Whole words only"));
    exitButton=new QPushButton(tr("Close"));
    replaceButton=new QPushButton(tr("Replace"));
    replaceAll=new QCheckBox(tr("Replace all"));


    QHBoxLayout *repLayout=new QHBoxLayout;
    repLayout->addWidget(new QLabel(tr("Replace as:")));
    repLayout->addWidget(replaceTextEdit);
    repLayout->addWidget(findBack);

    QHBoxLayout *extLayout=new QHBoxLayout;
    extLayout->addWidget(wholeWord);
    extLayout->addWidget(replaceAll);
    extLayout->addWidget(exitButton);
    extLayout->addWidget(replaceButton);

    extension=new QWidget;
    extVLayout=new QVBoxLayout;
    extVLayout->addLayout(repLayout);
    extVLayout->addLayout(extLayout);
    extVLayout->setAlignment(Qt::AlignLeft);
    extension->setLayout(extVLayout);



    hLayout=new QHBoxLayout;
    hLayout->addWidget(new QLabel(tr("Find string:")));
    hLayout->addWidget(input);
    hLayout->addWidget(findNext);

    QVBoxLayout *main=new QVBoxLayout;
    main->addLayout(hLayout);
    main->addWidget(extension);

    setLayout(main);

    setupSignals();
}

//信号与槽
void SearchDialog::setupSignals()
{
    connect(exitButton,SIGNAL(clicked()),this,SLOT(close()));
}

//相对于搜索对话框，替换对话框需要额外的控制按钮
void SearchDialog::showExtension(bool yes)
{
    if(yes)
        extension->setVisible(true);
    else
        extension->setVisible(false);
}
