#ifndef WIDGETUI_H
#define WIDGETUI_H

#include <QWidget>

#include "rkn.h"

#include <QChartView>
#include <QLineSeries>
#include <QScatterSeries>
#include <QValueAxis>

#if QT_VERSION < 0x060000
QT_CHARTS_USE_NAMESPACE
#endif

namespace Ui {
class Widgetui;
}

class Widgetui : public QWidget
{
   Q_OBJECT

private:
   double *z, // вектор координат z
       *t,    // вектор координат t
       *a,    // текущая амплитуда
       *eff,  // текущий КПД
       **th, **dth, *ymin, *ymax, xmin_val, *xmin = &xmin_val, *xmax, *xmin_eta = &xmin_val,
                                            *xmax_eta, *ymin_eta, *ymax_eta;
   int nz,                                      // число точек по z
       nt,                                      // число точек по времени
       ne, *it, phase_space, draw_trajectories; // текущая точка времени
   QValueAxis *xAxis, *xAxis_eta;               // Ось X
   QValueAxis *yAxis, *yAxis_eta;               // Ось Y
   double h, L, hth, delta, Ar, Ai;
   int Ne;
   QString sAr, sNth, sdelta, sL, sh;

public:
   explicit Widgetui(Rkn *, QWidget *parent = nullptr);
   ~Widgetui();
   void disable_enable_on_start();
   void disable_enable_on_stop();

private:
   Ui::Widgetui *ui;

private slots:
   void updateUI();

   void on_pushButton_Start_clicked();

   void on_pushButton_Exit_clicked();

   void on_pushButton_Restart_clicked();

   void on_pushButton_Stop_clicked();

signals:
   void start_calc();
   void pause();
   void reboot();

private:
   void connectSignals();
   QChart *createScatterChart_trj();
   QChart *createScatterChart_phs();
   QChart *createLineChart_eta();

private:
   QChart *chart, *chart_eff;
   int m_listCount;
   int m_valueMax;
   int m_valueCount;
   QList<QScatterSeries *> series;
   QList<QLineSeries *> series_eta;
   QList<QChartView *> m_charts;
   Rkn *r;
   //   int pause;
   QWidget *parw;
   int first_time = 0;

private:
   void init_paintGraph();

public slots:
   void paintGraph();
};

#endif // WIDGETUI_H
