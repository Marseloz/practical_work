#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "download.h"
#include "storage_data.h"
#include "QStandardItemModel"
#include "QValueAxis"
#include "QDateTimeAxis"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QXYSeries *thisSeries = nullptr;
    QStandardItemModel *model;
    QDateTimeAxis *axisX;
    QValueAxis *axisY;

    std::vector <DATA*> data_buff;

    void load_tree_view();
    void clear_tree_view();

    void correction(QVector<QPointF>& p);
    void averaging(QVector<QPointF>& p);
    void update_tab(double *min = nullptr, double *max = nullptr);


private slots:
    void open_csv();
    void open_json();
    void close();
    void add_clear_graph();
    void about();

    void blocked_tab();

    void check_tree_view(const QModelIndex &index);
    void click_button();

    void click_check_but(bool n);
    void click_combo_box(int n);
    void click_line_edit(QString n);
    void click_date();

    void text_y(QString n);
    void text_x(QString n);
    void y_max_min(int n);
    void x_max_min();
};
#endif // MAINWINDOW_H
