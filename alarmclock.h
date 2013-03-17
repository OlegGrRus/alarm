#ifndef ALARMCLOCK_H
#define ALARMCLOCK_H

#include <QtGui>

#include <QWidget>
#include <QSystemTrayIcon>
#include <QDateTime>
#include <QTimer>
#include <QMessageBox>
#include <QMap>
#include <QFile>
#include <phonon/MediaObject>
#include <phonon/AudioOutput>
#include "DialogAddAlarm.h"
/////////////////////////////
#include "messagedialog.h"

namespace Ui {
class AlarmClock;
}

class AlarmClock : public QWidget
{
    Q_OBJECT
    
public:
    explicit AlarmClock(QWidget *parent = 0);
    ~AlarmClock();
    void setLabelText();
    void openDataFile();
    void saveDataFile();
    void setVisible(bool visible);
protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::AlarmClock *ui;

    enum {myMagicNumber = 0xA7B7C7D7};
    QTimer *myTimer;
    QMap<QDateTime, QString> myData;
    quint32 count;
    QString filePath;

    void createTrayIcon(); //создаем иконку в трее
    void createActions();
    void showMainWindow(); //показать окно программы
    void updateInfo() const;
    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *maximizeAction;
    QAction *newTask;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    Phonon::MediaObject *music;
    Phonon::AudioOutput *output;
    bool onCloseMessage;
    DialogAddAlarm *dialogAlarm;
    QMessageBox *messageBox;
    //////////////////////////////
    MessageDialog *myDialog;

public slots:
    void addAlarm(); //добавляем событие
    void getAlarm(); //получаем все события
    void deleteAlarm(); //удаляем выбранное событие
    void showDateTime();

private slots:
     void setMyIcon(); //установить картинку иконки
     void iconActivated(QSystemTrayIcon::ActivationReason reason); // обрабока событий иконки в трее
     void showMessage(); //всплывающие подсказки в трее
     void messageClicked(); //событие при клике на всплывающие подсказки
     void setTextShowButton();

     void on_openDialogButton_clicked();
};


#endif // ALARMCLOCK_H
