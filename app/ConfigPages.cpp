/****************************LDevelop********************************
*********Copyright @ 吴平杰 (Pingjie Wu) from China,2011*********
*******************************************************************/
/*
  配置对话框里面的三个分类页面，editConfigPage,viewConfigPage,lookConfigPage
  的定义。在初始阶段接受用configOptions传递的参数。
*/

#include "ConfigPages.h"

#include <QDate>
#include <QDateTimeEdit>


editConfigPage::editConfigPage(configOptions options,QWidget *parent)
    : QWidget(parent)
{
    QGroupBox *configGroup = new QGroupBox(tr("LDevelop Mode and Text Options"));

    QLabel *modeLabel = new QLabel(tr("LDevelop mode:"));
	modeCombo = new QComboBox;
    modeCombo->addItem(tr("IDE"));
    modeCombo->addItem(tr("TextEditor"));
	modeCombo->setCurrentIndex(options.ProgramMode-1);

    QLabel *highlightLabel = new QLabel(tr("Syntax highlighting mode:"));
	highlightCombo = new QComboBox;
    highlightCombo->addItem(tr("C/C++"));
    highlightCombo->addItem(tr("Qt"));
    highlightCombo->addItem(tr("C#"));
    highlightCombo->addItem(tr("Java"));
    highlightCombo->addItem(tr("Makefile"));
    highlightCombo->addItem(tr("Linux Shell"));
    highlightCombo->addItem(tr("Plain Text"));
	highlightCombo->setCurrentIndex(options.TextHighlightMode);

    QLabel *editBackgroundLabel=new QLabel(tr("Edit area background color:"));
	editBackgroundSelecter=new QPushButton;
	editBackgroundSelecter->setFlat(true);
	editBackgroundSelecter->setAutoFillBackground(true);
	editBackgroundSelecter->setPalette(QPalette(QColor(options.BackgroundColor)));

	selectColor=new QColorDialog(this);
    selectColor->setWindowTitle(tr("Select Edit Area Background Color"));
	connect(editBackgroundSelecter,SIGNAL(clicked()),selectColor,SLOT(show()));
	connect(selectColor,SIGNAL(colorSelected(QColor)),this,SLOT(selectBackgroundColor(QColor)));

    QLabel *editFontLabel=new QLabel(tr("Font:"));
    editFontSelecter=new QPushButton(tr(options.TextFont.toString().toUtf8()).split(",").first());
	editFontSelecter->setFont(options.TextFont);
	editFontSelecter->setFlat(true);
	textFont=new QFontDialog(options.TextFont);
    textFont->setWindowTitle(tr("Select Text Font"));
	connect(editFontSelecter,SIGNAL(clicked()),textFont,SLOT(show()));
	connect(textFont,SIGNAL(fontSelected(QFont)),this,SLOT(selectTextFont(QFont)));

    QLabel *tabLabel=new QLabel(tr("Tab width:"));
	tabWidth=new QSpinBox;
	//tabWidth->setSuffix("  sec");
	tabWidth->setMaximum(16);
	tabWidth->setMinimum(1);
	tabWidth->setValue(options.TabWidth/10);

    autoIndent=new QCheckBox(tr("C-style auto indent"));
	autoIndent->setChecked(false);

    autoBackup=new QCheckBox(tr("Automatic backup"));
	autoIndent->setChecked(false);

    autoSave=new QCheckBox(tr("Automatic save"));
	autoSave->setChecked(options.AutoSaveTime);

    QLabel *autoSaveLabel=new QLabel(tr("Automatic save interval"));
	autoSaveTime=new QSpinBox;
    autoSaveTime->setSuffix(tr("  Minutes"));
	autoSaveTime->setMaximum(360);
	autoSaveTime->setMinimum(0);
	autoSaveTime->setValue(options.AutoSaveTime);
	autoSaveTime->setEnabled(options.AutoSaveTime);
	connect(autoSave,SIGNAL(toggled(bool)),autoSaveTime,SLOT(setEnabled(bool)));

	QHBoxLayout *modeLayout = new QHBoxLayout;
	modeLayout->addWidget(modeLabel);
	modeLayout->addWidget(modeCombo);

	QHBoxLayout *highlightLayout = new QHBoxLayout;
	highlightLayout->addWidget(highlightLabel);
	highlightLayout->addWidget(highlightCombo);

	QHBoxLayout *editBackgroundColorLayout = new QHBoxLayout;
	editBackgroundColorLayout->addWidget(editBackgroundLabel);
	editBackgroundColorLayout->addWidget(editBackgroundSelecter);

	QHBoxLayout *editFontLayout = new QHBoxLayout;
	editFontLayout->addWidget(editFontLabel);
//	editFontLayout->addWidget(fontTestLabel);
	editFontLayout->addWidget(editFontSelecter);

	QHBoxLayout *tabWidthLayout = new QHBoxLayout;
	tabWidthLayout->addWidget(tabLabel);
//	editFontLayout->addWidget(fontTestLabel);
	tabWidthLayout->addWidget(tabWidth);

	QHBoxLayout *autoSaveLayout = new QHBoxLayout;
	autoSaveLayout->addWidget(autoSaveLabel);
	autoSaveLayout->addWidget(autoSaveTime);

	QVBoxLayout *configLayout = new QVBoxLayout;
	configLayout->addLayout(modeLayout);
	configLayout->addLayout(highlightLayout);
	configLayout->addLayout(editBackgroundColorLayout);
	configLayout->addLayout(editFontLayout);
	configLayout->addLayout(tabWidthLayout);
	configLayout->addWidget(autoIndent);
	configLayout->addWidget(autoSave);
	configLayout->addLayout(autoSaveLayout);
	configLayout->setAlignment(Qt::AlignVCenter);

    configGroup->setLayout(configLayout);

	QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(configGroup);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

void editConfigPage::selectBackgroundColor(QColor color)
{
	//editBackgroundSelecter->de
	editBackgroundSelecter->setPalette(QColor(color));
}

void editConfigPage::selectTextFont(QFont font)
{
	editFontSelecter->setText(font.toString().split(",").first());
	editFontSelecter->setFont(font);
}

viewConfigPage::viewConfigPage(configOptions options,QWidget *parent)
    : QWidget(parent)
{
    QGroupBox *updateGroup = new QGroupBox(tr("Package selection"));
    QCheckBox *systemCheckBox = new QCheckBox(tr("Update system"));
    QCheckBox *appsCheckBox = new QCheckBox(tr("Update applications"));
    QCheckBox *docsCheckBox = new QCheckBox(tr("Update documentation"));

    QGroupBox *packageGroup = new QGroupBox(tr("Existing packages"));

    QListWidget *packageList = new QListWidget;
    QListWidgetItem *qtItem = new QListWidgetItem(packageList);
    qtItem->setText(tr("Qt"));
    QListWidgetItem *qsaItem = new QListWidgetItem(packageList);
    qsaItem->setText(tr("QSA"));
    QListWidgetItem *teamBuilderItem = new QListWidgetItem(packageList);
    teamBuilderItem->setText(tr("Teambuilder"));

    QPushButton *startUpdateButton = new QPushButton(tr("Start update"));

    QVBoxLayout *updateLayout = new QVBoxLayout;
    updateLayout->addWidget(systemCheckBox);
    updateLayout->addWidget(appsCheckBox);
    updateLayout->addWidget(docsCheckBox);
    updateGroup->setLayout(updateLayout);

    QVBoxLayout *packageLayout = new QVBoxLayout;
    packageLayout->addWidget(packageList);
    packageGroup->setLayout(packageLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(updateGroup);
    mainLayout->addWidget(packageGroup);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(startUpdateButton);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

lookConfigPage::lookConfigPage(configOptions options,QWidget *parent)
    : QWidget(parent)
{
    QGroupBox *packagesGroup = new QGroupBox(tr("Look for packages"));

    QLabel *nameLabel = new QLabel(tr("Name:"));
    QLineEdit *nameEdit = new QLineEdit;

    QLabel *dateLabel = new QLabel(tr("Released after:"));
    QDateTimeEdit *dateEdit = new QDateTimeEdit(QDate::currentDate());

    LineNumber = new QCheckBox(tr("Display line numbers"));
	LineNumber->setChecked(options.LineNumber);
    HighlightCurLine = new QCheckBox(tr("Highlight the current line"));
	HighlightCurLine->setChecked(options.HighlightCurrentLine);

    QSpinBox *hitsSpinBox = new QSpinBox;
    hitsSpinBox->setPrefix(tr("Return up to "));
    hitsSpinBox->setSuffix(tr(" results"));
    hitsSpinBox->setSpecialValueText(tr("Return only the first result"));
    hitsSpinBox->setMinimum(1);
    hitsSpinBox->setMaximum(100);
    hitsSpinBox->setSingleStep(10);

    QPushButton *startQueryButton = new QPushButton(tr("Start query"));

    QGridLayout *packagesLayout = new QGridLayout;
    packagesLayout->addWidget(nameLabel, 0, 0);
    packagesLayout->addWidget(nameEdit, 0, 1);
    packagesLayout->addWidget(dateLabel, 1, 0);
    packagesLayout->addWidget(dateEdit, 1, 1);
	packagesLayout->addWidget(LineNumber, 2, 0);
	packagesLayout->addWidget(HighlightCurLine, 3, 0);
    packagesLayout->addWidget(hitsSpinBox, 4, 0, 1, 2);
    packagesGroup->setLayout(packagesLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(packagesGroup);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(startQueryButton);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}
