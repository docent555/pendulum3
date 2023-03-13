/***************************************************************************************************************\
*   Моделирование системы уравнений:                                                                            *
*                                                                                                               *
*   i\frac{\partial^2 A}{\partial x^2} + \frac{\partial A}{\partial\tau} + \frac{\partial A}{\partial z} = J    *
*   \frac{\partial^2\theta}{\partial z^2} = Re\left(A e^{i\theta}\right)                                        *
*   A(\tau)|_{z=0} = KA(\tau)|_{z=L}                                                                            *
*   \theta|_{z=0} = \theta_0\in [0, 2\pi]                                                                       *
*   \frac{\partial\theta}{\partial z} = \Delta                                                                  *
*                                                                                                               *
*   методом Рунге-Кутты-Нистрема                                                                                *
\***************************************************************************************************************/

#include <fstream>
#include <iostream>
#include <QDebug>

#include "dialog.h"
#include "rkn.h"
#include <QDir>
#include <QMutex>
#include <QMutexLocker>

QMutex mx;

Rkn::Rkn(QObject *parent) : QObject(parent), ic(0.0, 1.0)
{
   Dialog *d = new Dialog(geth(), getL(), getNe(), getAr(), getAi(), getdelta());

   d->exec();

   ifstream in("input.dat");
   if (!in) {
      qDebug() << "Error opening file input.dat";
      exit(1);
   }

   //****************************************************************************************//
   //							 Читаем данные из файла input.dat
   //****************************************************************************************//
   in >> h >> L >> Ne >> Ar >> delta >> phase_space >> draw_trajectories;
   //========================================================================================//
   //						   / Читаем данные из файла input.dat
   //========================================================================================//

   //****************************************************************************************//
   //							 Печатаем в консоли данные из файла input.dat
   //****************************************************************************************//
   cout << "h = " << h << "\nL = " << L << "\nNe = " << Ne << "\ndelta = " << delta << endl << endl;
   //****************************************************************************************//
   //							 Печатаем в консоли данные из файла input.dat
   //****************************************************************************************//

   A = complex<double>(Ar, Ai);
   NZ = int(L / h) + 1;
   hth = 2 * M_PI / Ne;

   cout << "NZ = " << NZ << "\nh = " << hth << endl;

   //****************************************************************************************//
   //							 Массивы для A(z), th(z,th0), dthdz(z,th0)
   //****************************************************************************************//
   th = new double *[NZ];
   dthdz = new double *[NZ];
   for (int i = 0; i < NZ; i++) {
      th[i] = new double[Ne];
      dthdz[i] = new double[Ne];
   }
   F0 = new double[Ne];
   z = new double[NZ];
   eff = new double[NZ];

   //========================================================================================//
   //						   / Массивы для A(z), th(z,th0), dthdz(z,th0)
   //========================================================================================//

   for (int i = 0; i < NZ; i++) // формируем вектор пространственных координат
   {
      z[i] = i * h;
   }
}

bool Rkn::calculating() const
{
   return m_calculating;
}

void Rkn::setCalculating(bool calculating)
{
   if (m_calculating == calculating)
      return;

   m_calculating = calculating;
   emit calculatingChanged(calculating);

   //    qDebug() << "m_calculating = " << m_calculating;
}

bool Rkn::stop() const
{
   return m_calculating;
}

void Rkn::setStop(bool stop)
{
   if (m_stop == stop)
      return;

   m_stop = stop;
   emit stopChanged(stop);

   //    qDebug() << "m_calculating = " << m_calculating;
}

void Rkn::calculate()
{
   //   QMutexLocker ml(&mx);
   //****************************************************************************************//
   //						     Начальные условия
   //****************************************************************************************//

   for (int k = 0; k < Ne; k++) {
      th[0][k] = hth * k;
      //      th[0][k] = hth * k - M_PI / 2.0;
      dthdz[0][k] = delta;
      for (int i = 1; i < NZ; i++) {
         th[i][k] = 0.0;
         dthdz[i][k] = 0.0;
      }
   }

   for (int k = 0; k < Ne; k++) {
      qDebug() << th[0][k];
   }
   qDebug() << "h = " << h;

   //   ofstream f;
   //   f.open("test.dat");
   //   for (int i = 0; i < Ne; i++) {
   //      f << i << ' ' << th[0][i] << '\n';
   //   }
   //   f.close();

   //========================================================================================//
   //						   / Начальные условия
   //========================================================================================//

   for (int i = 0; i < NZ - 1; i++) {
      eff[i + 1] = 0.0;
      for (int k = 0; k < Ne; k++) {
         F0[k] = F(th[i][k]);

         //Предиктор th
         th[i + 1][k] = th[i][k] + dthdz[i][k] * h + h / 2.0 * F0[k] * h;
         //Предиктор dthdz
         dthdz[i + 1][k] = dthdz[i][k] + h * F0[k];
         //Корректор th
         th[i + 1][k] = th[i][k] + dthdz[i][k] * h + h / 6.0 * F0[k] * h
                        + h / 3.0 * F(th[i + 1][k]) * h;
         //Корректор dthdz
         dthdz[i + 1][k] = dthdz[i][k] + h / 2.0 * (F0[k] + F(th[i + 1][k]));

         if (th[i + 1][k] < thmin)
            thmin = th[i + 1][k];
         if (th[i + 1][k] > thmax)
            thmax = th[i + 1][k];
         if (dthdz[i + 1][k] < dthmin)
            dthmin = dthdz[i + 1][k];
         if (dthdz[i + 1][k] > dthmax)
            dthmax = dthdz[i + 1][k];

         eff[i + 1] = eff[i + 1] + (dthdz[i + 1][k] - delta);
      }

      eff[i + 1] = eff[i + 1] / double(Ne);

      if (eff[i + 1] < effmin)
         effmin = eff[i + 1];
      if (eff[i + 1] > effmax)
         effmax = eff[i + 1];

      //      qDebug() << z[i] << eff[i];

      it = i + 1;
      emit paintSignal();
      QMutexLocker ml(&mx);
      //      mx.lock();
      //      mx.unlock();
   }

   if (m_stop) {
      while (m_calculating) {
      }
      emit finished();
   }  

   cout << endl;

   //========================================================================================//
   //						   / Main part
   //========================================================================================//

   while (m_calculating) {
   }
   emit finished();
}

//****************************************************************************************//
//							 Functions
//****************************************************************************************//

inline double Rkn::F(double th)
{
   //   return real(A * exp(ic * th));
   return imag(A * exp(ic * th));
}

//========================================================================================//
//						   / Functions
//========================================================================================//
