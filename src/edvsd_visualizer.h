#ifndef EDVSD_VISUALIZER_H
#define EDVSD_VISUALIZER_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>

/**
 * @brief EDVSD_Visualizer
 */
class EDVSD_Visualizer : public QWidget
{
    Q_OBJECT
public:
    explicit EDVSD_Visualizer(QWidget *p_parent, int p_size_x, int p_size_y);
    ~EDVSD_Visualizer();

    /**
     * @brief overrides paintEvent of QWidget
     */
    void paintEvent(QPaintEvent *p_paintevent);

    /**
     * @brief set scale factor of visualization (default is 1.0)
     */
    void setScaler(double p_scaler);

private:
    int m_size_x, m_size_y;
    double m_scaler;
    QWidget *m_parent;
    QPainter m_painter;
    QImage *m_image;
    
signals:
    
public slots:
    
};

#endif // EDVSD_VISUALIZER_H
