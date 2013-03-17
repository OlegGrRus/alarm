#ifndef DIALOGADDALARM_H
#define DIALOGADDALARM_H
#include <QDialog>
#include "ui_dialogaddalarm.h"

class DialogAddAlarm : public QDialog, public Ui_Dialog
{
    Q_OBJECT
public:
    DialogAddAlarm(QWidget *parent = 0);
private slots:
    void closeAndClear();
};

#endif // DIALOGADDALARM_H
