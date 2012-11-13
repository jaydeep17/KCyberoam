#ifndef GENERALSETTINGS_H
#define GENERALSETTINGS_H

#include <QDialog>
#include <QList>
#include <QTableWidgetItem>
#include "kcyberoam.h"
#include <KConfig>
#include <KConfigGroup>

namespace Ui {
class GeneralSettings;
}

class GeneralSettings : public QDialog
{
    Q_OBJECT
    
public:
    explicit GeneralSettings(QWidget *parent = 0);

    ~GeneralSettings();
    
private:
    Ui::GeneralSettings *ui;
    KConfigGroup grp;
    void readAlternatives();
    void removeAltEntry(QList<QTableWidgetItem*> selectedItems);

private slots:
    void addAlternative();
    void removeAlternative();
    void altEdited(QTableWidgetItem *itm);
    void clearAlternatives();
};

#endif // GENERALSETTINGS_H
