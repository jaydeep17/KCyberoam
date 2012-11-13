#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KXmlGuiWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSystemTrayIcon>
#include <KStatusNotifierItem>
#include <KAction>
#include <KMenu>
#include <QTimer>
#include <QCloseEvent>
#include <QEvent>
#include <KConfig>
#include <KConfigGroup>

#include "kcyberoam.h"
#include "generalsettings.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void changeEvent(QEvent *ev);
    void closeEvent(QCloseEvent *ev);

signals:
    void loggedin();
    void loggedOff();

private slots:
    void readReply(QNetworkReply *rply);
    void login(bool timer= false);
    void declareLoggedIN();
    void declareLoggedOFF();
    void showTrayMessage();
    void callLogin();
    void checkConnection();
    void showSettings();

private:
    Ui::MainWindow *ui;
    QUrl cyberoamURL;
    QNetworkAccessManager *manager;
    QNetworkRequest req;
    bool isLoggedin, wait4logout, supressMessage, gotReply;

    KStatusNotifierItem *tray;
    KMenu *trayMenu;
    KAction *log_in, *log_out, *quitAction, *showSettingsAction;
    GeneralSettings *gsettings;
    KConfigGroup grp;
    QString inUseID;

    int failAttempts;
    int timeInterval;
    QTimer tm, timeout;
    void createActions();
    void createTrayMenu();
    void loginFailed();

};

#endif // MAINWINDOW_H
