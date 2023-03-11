#ifndef RKN_H
#define RKN_H

#include <complex>
#include <QObject>
#include <QString>

using namespace std;

class Rkn : public QObject
{
   Q_OBJECT

   // Свойства, управляющее работой потока
   Q_PROPERTY(bool calculating READ calculating WRITE setCalculating NOTIFY calculatingChanged)
   Q_PROPERTY(bool stop READ stop WRITE setStop NOTIFY stopChanged)

   bool m_calculating;
   bool m_stop;

   //****************************************************************************************//
   //							 Параметры счета и начальные условия
   //****************************************************************************************//
   complex<double> ic, A;
   double h, L, hth, delta, Ar, Ai;
   int NZ, Ne, phase_space, draw_trajectories;
   int it; // текущий момент времени
   complex<double> *A0, *A1, *J0, *Az0, *AzL;
   double **th, **dthdz, thmin = 1000, thmax = -1000, dthmin = 1000, dthmax = -1000;
   double *F0;
   double *z;
   QString *dir;

signals:
   void paintSignal(); // сигнал для прорисовки графика на очередной итерации по времени (соединение в конструкторе themewidget)
   void calculatingChanged(bool calculating);
   void stopChanged(bool calculating);
   void finished(); // Сигнал, по которому будем завершать поток, после завершения метода calculate

public:
   explicit Rkn(QObject *parent = nullptr);
   bool calculating() const;
   bool stop() const;
   //    QEventLoop loop;
   double *getz() { return z; }
   int getnz() { return NZ; }
   int *getit() { return &it; }
   double **get_theta() { return th; }
   double **get_dtheta() { return dthdz; }
   double *get_thmin() { return &thmin; }
   double *get_thmax() { return &thmax; }
   double *get_dthmin() { return &dthmin; }
   double *get_dthmax() { return &dthmax; }
   int get_phase_space() { return phase_space; }
   int get_draw_trajectories() { return draw_trajectories; }

   // Получение париметров счета для инициализации в классе Dialog
   double &geth() { return h; }
   double &getL() { return L; }  
   int &getNe() { return Ne; }
   double &getAr() { return Ar; }
   double &getAi() { return Ai; }
   double &getdelta() { return delta; }        

public slots:
   void calculate();   
   void setCalculating(bool calculating);
   void setStop(bool calculating);

private:
   inline double F(double th);
};

#endif // RKN_H
