#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <KMessageBox>
#include <KApplication>
#include <KStandardAction>
#include <KActionCollection>
#include <KConfigDialog>

#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    KXmlGuiWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->cyb_image->setPixmap(QPixmap::fromImage(QImage(":/cyb/webclientportallogo.png")));
    this->setWindowTitle("Cyberoam AutoLogin");
    this->setWindowIcon(QIcon(":/cyb/icon.png"));
    this->setFixedSize(this->width(),this->height());

    cyberoamURL.setUrl("http://10.100.56.55:8090/httpclient.html");

    manager = new QNetworkAccessManager(this);
    req.setUrl(cyberoamURL);
    connect(ui->login_b,SIGNAL(clicked()),this,SLOT(login()));

    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(readReply(QNetworkReply*)));
    connect(this,SIGNAL(loggedin()),this,SLOT(declareLoggedIN()));
    connect(this,SIGNAL(loggedOff()),this,SLOT(declareLoggedOFF()));
    connect(ui->user_field,SIGNAL(returnPressed()),this,SLOT(login()));
    connect(ui->pass_field,SIGNAL(returnPressed()),this,SLOT(login()));

    isLoggedin = false;

    grp = KConfigGroup(KGlobal::config(),"credentials");
    QString uname = grp.readEntry("uname","");

    if(!uname.isEmpty()){
        ui->remember->setChecked(true);
        ui->user_field->setText(uname);
        ui->pass_field->setText(grp.readEntry("pass",""));
    }

    tray = new KStatusNotifierItem(this);
    tray->setStatus(KStatusNotifierItem::Active);
    tray->setIconByPixmap(QIcon(":/cyb/icon.png"));
    tray->setToolTip(QIcon(":/cyb/icon.png"), "Cyberoam AutoLogin Client", "");
    wait4logout = supressMessage = gotReply = false;

    createActions();
    createTrayMenu();

    timeInterval = 179;

    tm.setInterval(179*60*1000);
    connect(&tm,SIGNAL(timeout()),this,SLOT(callLogin()));

    timeout.setInterval(5000);
    connect(&timeout,SIGNAL(timeout()),this,SLOT(checkConnection()));

    setupGUI(Default,"/home/jaydeep/projects/KCyberoam/debug/share/apps/kcyberoam/kcyberoamui.rc");

    if(KCyberoam::msts())
        this->setWindowState(Qt::WindowMinimized);

    if(KCyberoam::alos())
        login();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *ev)
{
    if(ev->type() == QEvent::WindowStateChange){
        if(this->windowState() & Qt::WindowMinimized){
            QTimer::singleShot(250, this, SLOT(hide()));
            QTimer::singleShot(600,this,SLOT(showTrayMessage()));
        }
    }
    QMainWindow::changeEvent(ev);
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
    if(KCyberoam::loe() && ev->type() == QEvent::Close){
        if(isLoggedin && gotReply){
            wait4logout = true;
            login(false);
            ev->ignore();
        } else {
            QMainWindow::closeEvent(ev);
        }
    }
}

void MainWindow::readReply(QNetworkReply *rply)
{
    gotReply = true;
    if(rply->error() == QNetworkReply::NoError){
        QString response = QString::fromUtf8(rply->readAll());
        qDebug() << response;
        if(response.contains("Make sure your password is correct")){
            if(this->isMinimized()){
                tray->showMessage("Login failed","The system could not log you on.\nMake sure your password is correct.","error");
            } else {
                KMessageBox::error(this,"The system could not log you on.\nMake sure your password is correct.","Login failed");
            }
            loginFailed();
        } else if(response.contains("Maximum",Qt::CaseInsensitive)){
            if(this->isMinimized()){
                tray->showMessage("Login failed","You have reached Maximum Login Limit.","error");
            } else {
                KMessageBox::error(this,"You have reached Maximum Login Limit.","Login failed");
            }
            loginFailed();
        } else if(response.contains("You have successfully logged in")){
            emit loggedin();
        } else if(response.contains("You have successfully logged off")){
            emit loggedOff();
            if(wait4logout)
                this->close();
        }
    } else {
        qDebug() << "error";
        KMessageBox::error(this, rply->errorString(),"Error Occured");
        isLoggedin = false;
    }
}

void MainWindow::login(bool timer)
{
    supressMessage = timer;
    if(ui->remember->isChecked()){
        grp.writeEntry("uname",ui->user_field->text());
        grp.writeEntry("pass",ui->pass_field->text());
        grp.sync();
    } else {
        grp.writeEntry("uname","");
        grp.writeEntry("pass","");
        grp.sync();
    }
    if(ui->user_field->text() == "" || ui->pass_field->text() == ""){
        if(this->isMinimized())
            tray->showMessage("Cannot Login","Incomplete credentials provided","error");
        else{
            this->show();
            KMessageBox::error(this,"Incomplete credentials provided","Cannot Login");
        }
        return;
    }

    inUseID = ui->user_field->text();

    if(isLoggedin && !timer){
        QUrl credentials;
        credentials.addQueryItem("mode","193");
        credentials.addQueryItem("username",ui->user_field->text());

        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        gotReply = false;
        manager->post(req,credentials.encodedQuery());
        timeout.start();
    } else {
        QUrl credentials;
        credentials.addQueryItem("mode","191");
        credentials.addQueryItem("username",ui->user_field->text());
        credentials.addQueryItem("password",ui->pass_field->text());

        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        gotReply = false;
        manager->post(req,credentials.encodedQuery());
        timeout.start();
        tm.stop();
        tm.start();
    }
}

void MainWindow::declareLoggedIN()
{
    if(this->isMinimized() && !supressMessage){
        tray->showMessage("Notification" , ui->user_field->text() + " successfully logged in","info");
    }
    ui->login_b->setText("Logout");
    log_in->setText(inUseID);
    isLoggedin = true;
    log_out->setEnabled(true);
    log_in->setEnabled(false);

    ui->user_field->setEnabled(false);
    ui->pass_field->setEnabled(false);
}

void MainWindow::declareLoggedOFF()
{
    if(this->isMinimized()){
        tray->showMessage("Notification", ui->user_field->text() + " successfully logged off", "info");
    }
    ui->login_b->setText("Login");
    log_in->setText("Login");
    isLoggedin = false;
    log_out->setEnabled(false);
    log_in->setEnabled(true);

    ui->user_field->setEnabled(true);
    ui->pass_field->setEnabled(true);
}

void MainWindow::createActions()
{
    log_in = new KAction(KIcon("preferences-system-login"), "Login",this);
    connect(log_in,SIGNAL(triggered()),this,SLOT(login()));
    actionCollection()->addAction("login",log_in);

    log_out = new KAction(KIcon("gnome-log-out"),"Logout",this);
    log_out->setEnabled(false);
    connect(log_out,SIGNAL(triggered()),this,SLOT(login()));
    actionCollection()->addAction("logout",log_out);

    quitAction = new KAction(KIcon("gtk-quit"),"Quit",this);
    connect(quitAction,SIGNAL(triggered()),kapp,SLOT(quit()));
    quitAction->setShortcut(KStandardShortcut::quit());
    actionCollection()->addAction("quitAction",quitAction);

    showSettingsAction = new KAction(KIcon("configure"),"Preferences",this);
    connect(showSettingsAction,SIGNAL(triggered()),this,SLOT(showSettings()));
    actionCollection()->addAction("showsettings",showSettingsAction);
}

void MainWindow::createTrayMenu()
{
    trayMenu = new KMenu("KCyberoam",this);
    trayMenu->addTitle(QIcon(":/cyb/icon.png"),"KCyberoam");
    trayMenu->addAction(log_in);
    trayMenu->addAction(log_out);
    tray->setContextMenu(trayMenu);
}

void MainWindow::loginFailed()
{
    QStringList usernames = grp.keyList();
    if(failAttempts > grp.keyList().count()+1){
        KMessageBox::information(this,"None of the A/Cs logged in successfully :(","Login Failure");
        return;
    }
    int index = 0;
    if(usernames.contains(inUseID)){
        index = usernames.indexOf(inUseID);
    }

    //replace current id with an alternative
    QString usr = ui->user_field->text();
    QString pass = ui->pass_field->text();

    QString altUser = usernames.at(index);
    QString altPass = grp.readEntry(altUser);

    grp.deleteEntry(altUser);
    grp.writeEntry(usr,pass);

    ui->user_field->setText(altUser);
    ui->pass_field->setText(altPass);

    failAttempts++;
    login();
}

void MainWindow::showTrayMessage()
{
    if(KCyberoam::snom())
        tray->showMessage("Notification","I'm alive :), sitting here..","info");
}

void MainWindow::callLogin()
{
    login(true);
}

void MainWindow::checkConnection()
{
    if(!gotReply){
        if(this->isMinimized()){
            tray->showMessage("Connection Error","Cyberoam didn't respond to the request","error");
        } else {
            KMessageBox::error(this,"Cyberoam didn't respond to the request","Connection Error");
        }
        isLoggedin = false;
    }
    timeout.stop();

}

void MainWindow::showSettings()
{
    if(KConfigDialog::showDialog("kcyberoam")){
        return;
    }
    KConfigDialog *dialog = new KConfigDialog(this,"kcyberoam",KCyberoam::self());
    gsettings = new GeneralSettings(this);
    dialog->addPage(gsettings,i18n("General Settings"),"preferences-desktop");
    dialog->show();
}
