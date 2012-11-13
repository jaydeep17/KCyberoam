#include "generalsettings.h"
#include "ui_generalsettings.h"
#include <KInputDialog>
#include <QDebug>
#include <KMessageBox>
#include <KComponentData>

GeneralSettings::GeneralSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GeneralSettings)
{
    ui->setupUi(this);
    ui->tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << i18n("Usernames") << i18n("Passwords"));
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    connect(ui->tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),SLOT(altEdited(QTableWidgetItem*)));
    connect(ui->addButton,SIGNAL(clicked()),this,SLOT(addAlternative()));
    connect(ui->removeButton,SIGNAL(clicked()),SLOT(removeAlternative()));
    connect(ui->clearAltButton,SIGNAL(clicked()),SLOT(clearAlternatives()));

    grp = KConfigGroup(KGlobal::config(),"alternatives");
    readAlternatives();
}

GeneralSettings::~GeneralSettings()
{
    delete ui;
}

void GeneralSettings::readAlternatives()
{
    foreach(QString usr, grp.keyList()){
        int rowCount = ui->tableWidget->rowCount();
        ui->tableWidget->setRowCount(rowCount+1);
        ui->tableWidget->setItem(rowCount,0,new QTableWidgetItem(usr));
        ui->tableWidget->setItem(rowCount,1,new QTableWidgetItem(grp.readEntry(usr,"#No Password Found")));
    }
}

void GeneralSettings::addAlternative()
{
    QString alts = KInputDialog::getText("Enter username & password","separate using a semi-colon (no spaces)","username;passoword");
    QString user = alts.left(alts.indexOf(";"));
    qDebug() << alts;
    QString pass = alts.remove(0,alts.indexOf(";")+1);

    int rowCount = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(rowCount+1);
    QTableWidgetItem *userItm = new QTableWidgetItem(user);
    ui->tableWidget->setItem(rowCount,0,userItm);
    QTableWidgetItem *passItm = new QTableWidgetItem(pass);
    ui->tableWidget->setItem(rowCount,1,passItm);

    if(!user.isEmpty()){
        grp.writeEntry(user,pass);
        grp.sync();
    }
}

void GeneralSettings::removeAlternative()
{
    removeAltEntry(ui->tableWidget->selectedItems());
}

void GeneralSettings::altEdited(QTableWidgetItem *itm)
{
    qDebug() << itm->text() + " has been editem";
}

void GeneralSettings::removeAltEntry(QList<QTableWidgetItem *> selectedItems)
{
    QSet<int> selectedRows;
    foreach(QTableWidgetItem *itm, selectedItems){
        selectedRows.insert(itm->row());
    }
    QList<int> selectedList = selectedRows.toList();
    qSort(selectedList.begin(),selectedList.end(),qGreater<int>());

    foreach(int row, selectedList){
        grp.deleteEntry(ui->tableWidget->item(row,0)->text());
        ui->tableWidget->removeRow(row);;
    }
    grp.sync();
}

void GeneralSettings::clearAlternatives()
{
    ui->tableWidget->selectAll();
    removeAltEntry(ui->tableWidget->selectedItems());
}
