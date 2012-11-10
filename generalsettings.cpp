#include "generalsettings.h"
#include "ui_generalsettings.h"
#include <KInputDialog>
#include <QDebug>
#include  <QListWidget>

GeneralSettings::GeneralSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GeneralSettings)
{
    ui->setupUi(this);
    ui->tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << i18n("Usernames") << i18n("Passwords"));
    //ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    connect(ui->addButton,SIGNAL(clicked()),SLOT(addAlternative()));
}

GeneralSettings::~GeneralSettings()
{
    delete ui;
}

void GeneralSettings::addAlternative()
{
    //TO-Do: the username & pass are getting into the tablewidget, but not stored in kcfg

    QString alts = KInputDialog::getText("Enter username & password","separate using a semi-colon (no spaces)","username;passoword");
    QString user = alts.left(alts.indexOf(";"));
    qDebug() << alts;
    QString pass = alts.remove(0,alts.indexOf(";")+1);

    ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
    QTableWidgetItem *userItm = new QTableWidgetItem(user);
    ui->tableWidget->setItem(0,0,userItm);
    QTableWidgetItem *passItm = new QTableWidgetItem(pass);
    ui->tableWidget->setItem(0,1,passItm);

}
