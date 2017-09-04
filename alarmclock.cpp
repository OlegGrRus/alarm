#include "alarmclock.h"
#include "ui_alarmclock.h"
#include <QDebug>

AlarmClock::AlarmClock(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlarmClock)
{
    ui->setupUi(this);
    ui->textEdit->setReadOnly(true);//список заданий не редактируемый!
    dialogAlarm = new DialogAddAlarm(this);//диалог для добавления заданий

    //phonon
    music = new Phonon::MediaObject(this);
    output = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    music->setCurrentSource(Phonon::MediaSource("alarm.wav")); //звук будильника
    Phonon::createPath(music, output);
    //
    onCloseMessage = true; //перменная чтобы не вызывался повторно MessageBox при сворачивании в трей
    count = 0; //счетчик заданий
    filePath = "mytask.dat"; //путь к файлу с заданиями
    messageBox = new QMessageBox(this); //окно будильника с текстом задния
    messageBox->setModal(false); //если оставить модальным то звук не проигрывается
    messageBox->setMinimumSize(280, 320); //это не работает. Почему???

    ui->groupBox->hide();//прячем список задач
    layout()->setSizeConstraint(QLayout::SetFixedSize);//размер окна по содержимому

    createActions();
    createTrayIcon();
    setMyIcon();
    trayIcon->setVisible(true);
    connect(trayIcon,SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    trayIcon->show();

    myTimer = new QTimer(this);//запускаем таймер
    myTimer->start(1000);//период 1 Гц
    openDataFile();//открываем файл с заданиями
    setLabelText();//обнавляем информацию по колличеству заданий
    updateInfo();//обновляем список заданий
    connect(myTimer, SIGNAL(timeout()), this, SLOT(getAlarm()));// срабатывание будильника
    connect(myTimer, SIGNAL(timeout()), this, SLOT(showDateTime()));//ежесекундно обновляем текущее время
    connect(ui->deleteEventButton, SIGNAL(clicked()), this, SLOT(deleteAlarm()));//удаляем выбранное задание
    connect(dialogAlarm->addButton, SIGNAL(clicked()), this, SLOT(addAlarm()));//добавляем новое задание
    connect(ui->showButton, SIGNAL(toggled(bool)), this, SLOT(setTextShowButton()));

    /////////////////////////////
    myDialog = new MessageDialog(this);
    myDialog->show();
    myDialog->setText("Hello");
    /////////////////////////////

}

AlarmClock::~AlarmClock()
{
    myTimer->stop();//останавливаем таймер
    delete ui;
}

void AlarmClock::setLabelText()
{
    if(count !=0)
        ui->label->setText(trUtf8("Колличество заданий: ") + QString::number(count));
    else if(count == 0){
        ui->label->setText(trUtf8("Заданий нет"));
    }
}

void AlarmClock::openDataFile()
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "File not open";
        return;
    }
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_7);
    quint32 magic;
    in >> magic;
    if(magic != myMagicNumber){
        qDebug() << "Not support file format";
        return;
    }

    in >> myData ;
    foreach(const QDateTime &dt, myData.keys()){
        if(dt < QDateTime::currentDateTime())
            myData.remove(dt);
    }
    count = myData.size();
    foreach(const QDateTime &dt, myData.keys())
        qDebug()<< dt.toString(Qt::ISODate)<< ": " << myData.value(dt) << endl;
    file.close();
    setLabelText();
}

void AlarmClock::saveDataFile()
{
    QFile file;
    file.setFileName(filePath);
    if(file.exists())
        file.remove();
    // если лог есть то дописываем в него сообщения
    if (!file.open(QIODevice::Append | QIODevice::WriteOnly |  QIODevice::Text)){
            qDebug() << "File not found";
            return;
        }
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_7);
    out << myMagicNumber;
    if(!myData.isEmpty())
        out << myData;
    else
        qDebug() << "Map is empty";
        ui->textEdit->show();

    file.flush();
    file.close();
}

void AlarmClock::setVisible(bool visible)
{
    newTask->setEnabled(true);
    minimizeAction->setEnabled(visible);
    restoreAction->setEnabled(!visible);
    QWidget::setVisible(visible);
}

void AlarmClock::closeEvent(QCloseEvent *event)
{
    if (onCloseMessage) {

        QMessageBox::information(this, trUtf8("Alarm"),
                                 trUtf8("Программа будет помещена в трей. "
                                    "Для прекращения работы проложения выбирете "
                                    "<b>Quit</b> в контекстном меню программы "
                                        "(вызывается правым кликом мышки)."));
        hide();
        showMessage();
        event->ignore();
        onCloseMessage = false;
    }
}

void AlarmClock::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(newTask);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void AlarmClock::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    newTask = new QAction(tr("&Add task"), this);
    connect(newTask, SIGNAL(triggered()), ui->openDialogButton, SIGNAL(clicked()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void AlarmClock::showMainWindow()
{
    restoreAction->setEnabled(true);
    emit setVisible(restoreAction->isVisible());
}

void AlarmClock::updateInfo() const
{
    ui->textEdit->clear();
    ui->comboBox->clear();

    foreach(const QDateTime &dt, myData.keys()){
        ui->textEdit->append(dt.date().toString(Qt::ISODate) + " "+ dt.time().toString() +
                             ": " + myData.value(dt));
        ui->comboBox->addItem(myData.value(dt));
    }
}


void AlarmClock::addAlarm()
{
    if(dialogAlarm->textEdit->toPlainText() == ""){
        QMessageBox::warning(this, trUtf8("Ошибка"), trUtf8("Введите текст сообщения"));
    }
    else {
        myData.insert(dialogAlarm->timeEdit->dateTime(), dialogAlarm->textEdit->toPlainText());
        count = myData.size();
    }
    setLabelText();
    saveDataFile();
    updateInfo();
    showMessage();
    dialogAlarm->textEdit->clear();
}

void AlarmClock::getAlarm()
{
    foreach(const QDateTime &dt, myData.keys()){
        if(dt <= QDateTime::currentDateTime()){
            music->play(); //сначало играем звук
            messageBox->setText("<h2><font color=darkred>"+myData.value(dt)+"</font></h2>");
            messageBox->show();
            myData.remove(dt);
            count = myData.size();
            showMessage();//потом показываем сообщение в трее
            updateInfo();
        }
        setLabelText();       
    }
}

void AlarmClock::deleteAlarm()
{
    int index = ui->comboBox->currentIndex();
    QMapIterator<QDateTime, QString> it(myData);
    while(it.findNext(ui->comboBox->itemText(index))){
        myData.remove(it.key());
        count = myData.size();
    }
    setLabelText();
    saveDataFile();
    updateInfo();
    showMessage();
}

void AlarmClock::showDateTime()
{
    ui->timeLabel->setText(QDate::currentDate().toString(Qt::ISODate) + " " + QTime::currentTime().toString());
}

void AlarmClock::setMyIcon()
{
    QIcon icon(":/Icons/alarm_icon.png");
    trayIcon->setIcon(icon);
    trayIcon->setToolTip(ui->label->text());
}

void AlarmClock::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason){
    case QSystemTrayIcon::Trigger:
        showMessage();
        break;
    case QSystemTrayIcon::DoubleClick:
        showMainWindow();
        break;
    case QSystemTrayIcon::MiddleClick:
        showMessage();
        break;
    default:
        ;
    }
}

void AlarmClock::showMessage()
{
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(count);
    trayIcon->showMessage(trUtf8("Будильник"),QString(trUtf8("Колличество заданий: ")+
                                                      QString::number(count)), icon, 3000);
}

void AlarmClock::messageClicked()
{
    showMainWindow();
}

void AlarmClock::setTextShowButton()
{
    if(ui->showButton->isChecked())
        ui->showButton->setText(trUtf8("Скрыть активные задания"));
    else
        ui->showButton->setText(trUtf8("Показать активные задания"));
}


void AlarmClock::on_openDialogButton_clicked()
{
    dialogAlarm->show();
}
