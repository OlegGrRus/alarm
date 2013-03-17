#include <QtGui/QApplication>
#include "alarmclock.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QObject::tr("Systray"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }

    QApplication::setQuitOnLastWindowClosed(false);

    AlarmClock w;
    w.show();
    
    return a.exec();
}
