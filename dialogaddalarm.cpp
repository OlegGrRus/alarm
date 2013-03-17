#include <QtGui>
#include "DialogAddAlarm.h"

DialogAddAlarm::DialogAddAlarm(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    timeEdit->setDate(calendarWidget->selectedDate().currentDate());
    timeEdit->setTime(QTime::currentTime());
    calendarWidget->setGridVisible(true);
    connect(calendarWidget, SIGNAL(clicked(QDate)), timeEdit, SLOT(setDate(QDate)));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(closeAndClear()));

}

void DialogAddAlarm::closeAndClear()
{
    textEdit->clear();
    this->close();
}
