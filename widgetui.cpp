#include "widgetui.h"
#include "ui_widgetui.h"

#include <QDebug>
#include <QGridLayout>

Widgetui::Widgetui(Rkn *r, QWidget *parent)
    : QWidget(parent), ui(new Ui::Widgetui), m_listCount(3), m_valueMax(10), m_valueCount(7)
{
   ui->setupUi(this);

   z = r->getz();
   nz = r->getnz();
   th = r->get_theta();
   dth = r->get_dtheta();
   it = r->getit();
   ne = r->getNe();
   phase_space = r->get_phase_space();
   draw_trajectories = r->get_draw_trajectories();

   //   QGridLayout *baseLayout = new QGridLayout();
   //   QChartView *chartView;

   QSize min(300, 300);
   QSize max(900, 900);
   if (phase_space == 0) {
      ymin = r->get_thmin();
      ymax = r->get_thmax();
      *xmin = 0;
      xmax = &z[nz - 1];
      chart = new QChart();
      chart = createScatterChart_trj();
      ui->psView->setChart(chart);
      //      ui->psView->setChart(createScatterChart_trj());
      ui->psView->setMinimumSize(min);
      ui->psView->setMaximumSize(max);

      //      chartView = new QChartView(createScatterChart_trj());

      //      chartView->setMinimumSize(min);
      //      chartView->setMaximumSize(max);
   } else {
      ymin = r->get_dthmin();
      ymax = r->get_dthmax();
      xmin = r->get_thmin();
      xmax = r->get_thmax();
      chart = new QChart();
      chart = createScatterChart_phs();
      ui->psView->setChart(chart);
      //      ui->psView->setChart(createScatterChart_phs());
      ui->psView->setMinimumSize(min);
      ui->psView->setMaximumSize(max);

      //      chartView = new QChartView(createScatterChart_phs());

      //      chartView->setMinimumSize(min);
      //      chartView->setMaximumSize(max);
   }

   m_charts << ui->psView;
   updateUI();

   //   baseLayout->addWidget(chartView, 0, 0);
   //   m_charts << chartView;

   //   setLayout(baseLayout);
   //   updateUI();

   connect(r, SIGNAL(paintSignal()), this, SLOT(paintGraph()), Qt::BlockingQueuedConnection);
}

Widgetui::~Widgetui()
{
   delete ui;
}

QChart *Widgetui::createScatterChart_trj()
{
   QChart *chart = new QChart();
   chart->setTitle("Траектории");
   chart->setTheme(QChart::ChartThemeDark);
   QFont font = chart->titleFont();
   font.setPointSize(16);
   chart->setTitleFont(font);

   for (int i = 0; i < ne; i++) {
      series.append(new QScatterSeries());
      series[i]->setUseOpenGL(true);
   }

   xAxis = new QValueAxis; // Ось X
                           //    xAxis->setRange(0, z[nz - 1]);
   xAxis->setRange(*xmin, *xmax);
   xAxis->setTitleText(tr("z")); // Название оси X
   //    xAxis->setTitleBrush(Qt::black); // Цвет названия
   //    xAxis->setLabelsColor(Qt::black); // Цвет элементов оси
   font = xAxis->titleFont();
   font.setPointSize(16);
   xAxis->setTitleFont(font);
   font = xAxis->labelsFont();
   font.setPointSize(12);
   xAxis->setLabelsFont(font);

   yAxis = new QValueAxis;          // Ось Y
   yAxis->setRange(*ymin, *ymax);   // Диапазон от -20 до +20 Вольт
   yAxis->setTitleText(tr("Фаза")); // Название оси Y
   //    yAxis->setTitleBrush(Qt::black); // Цвет названия
   //    yAxis->setLabelsColor(Qt::black); // Цвет элементов оси
   font = xAxis->titleFont();
   font.setPointSize(16);
   yAxis->setTitleFont(font);
   font = xAxis->labelsFont();
   font.setPointSize(12);
   yAxis->setLabelsFont(font);

   chart->addAxis(xAxis, Qt::AlignBottom);
   chart->addAxis(yAxis, Qt::AlignLeft);

   for (int i = 0; i < ne; ++i) {
      chart->addSeries(series[i]);
      //    chart->setAxisX(xAxis, serieskpd); // Назначить ось xAxis, осью X для diagramA
      series[i]->attachAxis(xAxis);
      //    chart->setAxisY(yAxis, serieskpd); // Назначить ось yAxis, осью Y для diagramA
      series[i]->attachAxis(yAxis);
   }

   return chart;
}

QChart *Widgetui::createScatterChart_phs()
{
   QChart *chart = new QChart();
   chart->setTitle("Фазовая плоскость");
   chart->setTheme(QChart::ChartThemeDark);
   QFont font = chart->titleFont();
   font.setPointSize(16);
   chart->setTitleFont(font);

   for (int i = 0; i < ne; i++) {
      series.append(new QScatterSeries());
      series[i]->setUseOpenGL(true);
   }

   xAxis = new QValueAxis; // Ось X
   xAxis->setRange(*xmin, *xmax);
   xAxis->setTitleText(tr("Theta")); // Название оси X
   //    xAxis->setTitleBrush(Qt::black); // Цвет названия
   //    xAxis->setLabelsColor(Qt::black); // Цвет элементов оси
   font = xAxis->titleFont();
   font.setPointSize(16);
   xAxis->setTitleFont(font);
   font = xAxis->labelsFont();
   font.setPointSize(12);
   xAxis->setLabelsFont(font);

   yAxis = new QValueAxis;               // Ось Y
   yAxis->setRange(*ymin, *ymax);        // Диапазон от -20 до +20 Вольт
   yAxis->setTitleText(tr("dTheta/dz")); // Название оси Y
   //    yAxis->setTitleBrush(Qt::black); // Цвет названия
   //    yAxis->setLabelsColor(Qt::black); // Цвет элементов оси
   font = xAxis->titleFont();
   font.setPointSize(16);
   yAxis->setTitleFont(font);
   font = xAxis->labelsFont();
   font.setPointSize(12);
   yAxis->setLabelsFont(font);

   chart->addAxis(xAxis, Qt::AlignBottom);
   chart->addAxis(yAxis, Qt::AlignLeft);

   for (int i = 0; i < ne; ++i) {
      chart->addSeries(series[i]);
      //    chart->setAxisX(xAxis, serieskpd); // Назначить ось xAxis, осью X для diagramA
      series[i]->attachAxis(xAxis);
      //    chart->setAxisY(yAxis, serieskpd); // Назначить ось yAxis, осью Y для diagramA
      series[i]->attachAxis(yAxis);
   }

   return chart;
}

QChart *Widgetui::createLineChart_eta()
{
   QChart *chart_eta = new QChart();
   chart_eta->setTitle("КПД");
   chart_eta->setTheme(QChart::ChartThemeDark);
   QFont font = chart_eta->titleFont();
   font.setPointSize(10);
   chart_eta->setTitleFont(font);

   for (int i = 0; i < ne; i++) {
      series_eta.append(new QLineSeries());
      series_eta[i]->setUseOpenGL(true);
   }

   xAxis_eta = new QValueAxis; // Ось X
                               //    xAxis_eta->setRange(0, z[nz - 1]);
   xAxis_eta->setRange(*xmin_eta, *xmax_eta);
   xAxis_eta->setTitleText(tr("z")); // Название оси X
   //    xAxis_eta->setTitleBrush(Qt::black); // Цвет названия
   //    xAxis_eta->setLabelsColor(Qt::black); // Цвет элементов оси
   font = xAxis_eta->titleFont();
   font.setPointSize(10);
   xAxis_eta->setTitleFont(font);
   font = xAxis_eta->labelsFont();
   font.setPointSize(8);
   xAxis_eta->setLabelsFont(font);

   yAxis_eta = new QValueAxis; // Ось Y
   yAxis_eta->setRange(0, 1);
   //   yAxis_eta->setTitleText(tr("КПД")); // Название оси Y
   //    yAxis_eta->setTitleBrush(Qt::black); // Цвет названия
   //    yAxis_eta->setLabelsColor(Qt::black); // Цвет элементов оси
   font = xAxis_eta->titleFont();
   font.setPointSize(10);
   yAxis_eta->setTitleFont(font);
   font = xAxis_eta->labelsFont();
   font.setPointSize(8);
   yAxis_eta->setLabelsFont(font);

   chart_eta->addAxis(xAxis_eta, Qt::AlignBottom);
   chart_eta->addAxis(yAxis_eta, Qt::AlignLeft);

   for (int i = 0; i < ne; ++i) {
      chart_eta->addSeries(series_eta[i]);
      //    chart->setAxisX(xAxis_eta, serieskpd); // Назначить ось xAxis_eta, осью X для diagramA
      series_eta[i]->attachAxis(xAxis_eta);
      //    chart->setAxisY(yAxis_eta, serieskpd); // Назначить ось yAxis_eta, осью Y для diagramA
      series_eta[i]->attachAxis(yAxis_eta);
   }

   return chart_eta;
}

void Widgetui::updateUI()
{
   const auto charts = m_charts;

   bool checked = true;
   for (QChartView *chart : charts)
      chart->setRenderHint(QPainter::Antialiasing, checked);

   for (QChartView *chartView : charts) {
      chartView->chart()->legend()->hide();
   }
}

void Widgetui::paintGraph()
{
   static int j;
   j = *it;
   QColor green(Qt::green);

   if (phase_space == 0) {
      yAxis->setRange((*ymin) - 0.2, (*ymax) + 0.2);

      for (int i = 0; i < ne; i++) {
         if (draw_trajectories == 0)
            series[i]->clear();
         series[i]->setBrush(green);
         series[i]->append(z[j], th[j][i]);
      }
   } else {
      xAxis->setRange((*xmin) - 0.2, (*xmax) + 0.2);
      yAxis->setRange((*ymin) - 0.2, (*ymax) + 0.2);

      for (int i = 0; i < ne; i++) {
         if (draw_trajectories == 0)
            series[i]->clear();
         series[i]->setBrush(green);
         series[i]->append(th[j][i], dth[j][i]);
      }
   }
}