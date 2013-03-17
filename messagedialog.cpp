#include "messagedialog.h"
#include "ui_messagedialog.h"

MessageDialog::MessageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageDialog)
{
    ui->setupUi(this);
    ui->cbRepeatTask->setChecked(false);
    /*if(ui->cbRepeatTask->isChecked())
        ui->groupBox->setVisible(true);*/
}

MessageDialog::~MessageDialog()
{
    delete ui;
}

void MessageDialog::setText(QString s)
{
    ui->labelAlarm->setText("<h2><font color=darkred>" + s + "</font></h2>");
}
