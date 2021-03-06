#ifndef EDVSD_VISUALIZER_H
#define EDVSD_VISUALIZER_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>

#include "edvsd.h"

enum EDVS_Visualization_Mode{
	EDVS_Visualization_Mode_Green_Red = 0,
	EDVS_Visualization_Mode_White_Black = 1
};

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
	 * @brief set scale factor of visualization (default is 1.0)
	 */
	void setScaler(double p_scaler);

	/**
	 * @brief set visualization mode, see enum EDVS_Visualization_Mode
	 */
	void setMode(EDVS_Visualization_Mode p_mode);

	void start();

	void setPaused(bool p_paused);

	QPainter *getDebugPainter();

private:
	int m_size_x, m_size_y;
	double m_scaler;
	EDVS_Visualization_Mode m_mode;
	int m_fade;

	QWidget *m_parent;
	QImage *m_image;
	QTimer *m_timer;

	QPixmap *m_debug_pixmap;
	QPainter *m_debug_painter;

	bool m_paused;


	void fadeImage();

	static const quint32 m_colors[2][3];

protected:
	/**
	 * @brief overrides paintEvent of QWidget
	 */
	virtual void paintEvent(QPaintEvent *p_paintevent);

signals:
	void loadEventData();

private slots:
	void displayFrame();

public slots:
	void drawEvents(EDVS_Event *p_buffer, int p_n);

};

#endif // EDVSD_VISUALIZER_H
