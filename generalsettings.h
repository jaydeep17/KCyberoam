#ifndef GENERALSETTINGS_H
#define GENERALSETTINGS_H

#include <QDialog>
#include "kcyberoam.h"

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
    //KCyberoam cy;

private slots:
    void addAlternative();
};

#endif // GENERALSETTINGS_H
