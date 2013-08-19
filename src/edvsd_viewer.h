#ifndef EDVSD_VIEWER_H
#define EDVSD_VIEWER_H

#include <QMainWindow>

namespace Ui {
class EDVSD_Viewer;
}

class EDVSD_Viewer : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit EDVSD_Viewer(QWidget *parent = 0);
    ~EDVSD_Viewer();
    
private:
    Ui::EDVSD_Viewer *ui;
};

#endif // EDVSD_VIEWER_H
