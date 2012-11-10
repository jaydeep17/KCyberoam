#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KXmlGuiWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSystemTrayIcon>
#include <KAction>
#include <QMenu>
#include <QTimer>
#include <QCloseEvent>
#include <QEvent>

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
    void showDialog();
    void showTrayMessage();
    void callLogin();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void checkConnection();
    void showSettings();

private:
    Ui::MainWindow *ui;
    QUrl cyberoamURL;
    QNetworkAccessManager *manager;
    QNetworkRequest req;
    bool isLoggedin, traymode, wait4logout, supressMessage, gotReply;

    QSystemTrayIcon *tray;
    QMenu *trayMenu;
    KAction *shw, *log_in, *log_out, *quitAction, *showSettingsAction;
    GeneralSettings *gsettings;

    int timeInterval;
    //Dialog about_dialog;
    QTimer tm, timeout;
    void createActions();
    void createTrayMenu();
};

#endif // MAINWINDOW_H
