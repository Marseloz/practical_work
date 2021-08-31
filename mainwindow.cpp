#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "download.h"
#include <QDialog>
#include <QDebug>
#include <QtCharts>
#include <math.h>


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);

    model = new QStandardItemModel;
    model->setHorizontalHeaderLabels(QStringList("Устройства"));
    ui->treeView->setModel(model);

    QChart* chart = new QChart;

    axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("yyyy-MM-dd hh:mm:ss");
    axisX->setTitleText("Date");
    QDateTime* xValue = new QDateTime();
    QDateTime* xValue2 = new QDateTime;
    xValue->setDate(QDate(2021, 1, 1));
    xValue2->setDate(QDate(2021, 12, 31));
    axisX->setRange(*xValue, *xValue2);
    axisX->setLabelsAngle(65);
    chart->addAxis(axisX, Qt::AlignBottom);

    axisY = new QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTitleText("Value");
    axisY->setMax(30);
    axisY->setMin(0);
    chart->addAxis(axisY, Qt::AlignLeft);

    chart->setTitle("NAME");
    chart->setBackgroundRoundness(0);
    chart->setContentsMargins(-11,-11,-11,-11);

    ui->graphicsView->setChart(chart);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    //включение горизонтального масштабирования
    ui->graphicsView->setRubberBand(QChartView::HorizontalRubberBand);

    connect(ui->actionOpen_CSV, &QAction::triggered, this, &MainWindow::open_csv);
    connect(ui->action_JSON, &QAction::triggered, this, &MainWindow::open_json);
    connect(ui->action_2, &QAction::triggered, this, &MainWindow::close);
    connect(ui->action, &QAction::triggered, this, &MainWindow::add_clear_graph);
    connect(ui->action_4, &QAction::triggered, this, &MainWindow::about);

    connect(ui->treeView, &QTreeView::doubleClicked, this, &MainWindow::check_tree_view);

    connect(ui->checkBox_name, &QCheckBox::clicked, ui->lineEdit, &QLineEdit::setEnabled);
    connect(ui->checkBox_name, &QCheckBox::clicked, this, &MainWindow::click_check_but);
    connect(ui->checkBox, &QCheckBox::clicked, this, &MainWindow::click_check_but);
    connect(ui->comboBox_3, &QComboBox::activated, this, &MainWindow::click_combo_box);
    connect(ui->comboBox_2, &QComboBox::activated, this, &MainWindow::click_combo_box);
    connect(ui->comboBox, &QComboBox::activated, this, &MainWindow::click_combo_box);
    connect(ui->checkBox_date, &QCheckBox::clicked, this, &MainWindow::click_check_but);
    connect(ui->checkBox_date, &QCheckBox::clicked, ui->dateTimeEdit, &QDateTimeEdit::setEnabled);
    connect(ui->checkBox_date, &QCheckBox::clicked, ui->dateTimeEdit_2, &QDateTimeEdit::setEnabled);
    connect(ui->dateTimeEdit, &QDateTimeEdit::editingFinished, this, &MainWindow::click_date);
    connect(ui->dateTimeEdit_2, &QDateTimeEdit::editingFinished, this, &MainWindow::click_date);

    connect(ui->checkBox_edit, &QCheckBox::clicked, ui->lineEdit_3, &QLineEdit::setEnabled);
    connect(ui->checkBox_edit, &QCheckBox::clicked, ui->lineEdit_2, &QLineEdit::setEnabled);
    connect(ui->checkBox_edit, &QCheckBox::clicked, ui->spinBox, &QSpinBox::setEnabled);
    connect(ui->checkBox_edit, &QCheckBox::clicked, ui->spinBox_2, &QSpinBox::setEnabled);
    connect(ui->checkBox_edit, &QCheckBox::clicked, ui->dateTimeEdit_3, &QDateTimeEdit::setEnabled);
    connect(ui->checkBox_edit, &QCheckBox::clicked, ui->dateTimeEdit_4, &QDateTimeEdit::setEnabled);

    connect(ui->lineEdit, &QLineEdit::textEdited, this, &MainWindow::click_line_edit);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::click_button);

    connect(ui->dateTimeEdit_3, &QDateTimeEdit::editingFinished, this, &MainWindow::x_max_min);
    connect(ui->dateTimeEdit_4, &QDateTimeEdit::editingFinished, this, &MainWindow::x_max_min);
    connect(ui->spinBox, &QSpinBox::valueChanged, this, &MainWindow::y_max_min);
    connect(ui->spinBox_2, &QSpinBox::valueChanged, this, &MainWindow::y_max_min);
    connect(ui->lineEdit_2, &QLineEdit::textChanged, this, &MainWindow::text_x);
    connect(ui->lineEdit_3, &QLineEdit::textChanged, this, &MainWindow::text_y);
}

MainWindow::~MainWindow(){delete ui;}

void MainWindow::load_tree_view(){
    for(size_t num = 0; num < data_buff.size(); num++){
        QStandardItem *parentItem = model->invisibleRootItem();
        QStandardItem *item = new QStandardItem(data_buff.at(num)->getName());
        item->setData(0);
        parentItem->appendRow(item);
        parentItem = item;
        for (size_t i = 0; i < data_buff.at(num)->getSensName().size(); i++) {
            if(data_buff.at(num)->getData().at(i) != nullptr){
                item = new QStandardItem(data_buff.at(num)->getSensName().at(i));
                item->setData(1);
                parentItem->appendRow(item);
            }
        }
    }
}

void MainWindow::clear_tree_view(){
    //чистим память программы
    for(size_t i = 0; i<data_buff.size(); i++){
        if(data_buff.at(i) != nullptr){
            delete data_buff.at(i);
        }
    }
    data_buff.clear();
    //удаляем элементы из treeView
    model->removeRow(0, model->invisibleRootItem()->index());

    ui->tabWidget->setCurrentIndex(0);
    ui->treeView->clearFocus();
    blocked_tab();
    if(thisSeries != nullptr) {
        ui->graphicsView->chart()->removeSeries(thisSeries);
        thisSeries = nullptr;
    }
}

void MainWindow::update_tab(double *min, double *max){
    //удаляем старую серию
    if(thisSeries!=nullptr){ ui->graphicsView->chart()->removeSeries(thisSeries); }

    std::vector <double>* dat = data_buff.at(ui->treeView->selectionModel()->currentIndex().parent().row())->getData().at(ui->treeView->selectionModel()->currentIndex().row());
    std::vector <QDateTime>* time = data_buff.at(ui->treeView->selectionModel()->currentIndex().parent().row())->getDate();
    QVector<QPointF> p;
    //заполняем массив p
    for(size_t i = 0; i < dat->size(); i++){
        if(!ui->checkBox_date->isChecked()||((time->at(i)>=ui->dateTimeEdit_2->dateTime())&&(time->at(i)<=ui->dateTimeEdit->dateTime()))){
            p.push_back(QPointF(time->at(i).toMSecsSinceEpoch(), dat->at(i)));
        }
    }
    //производим коррекцию если поставленна галка на удаление артефактов
    if(ui->checkBox->isChecked()){correction(p);}
    //производим осреднение если поставленна галка на осреднение
    if(ui->comboBox->currentIndex()){averaging(p);}
    //создаём серию в зависимости от настроек
    if(ui->comboBox_3->currentIndex()){thisSeries = new QSplineSeries; }
    else{ thisSeries = new QLineSeries; }
    //добавляем в неё обработанный массив точек
    thisSeries->append(p);
    //добавляем цвета
    if(ui->comboBox_2->currentIndex()==1){thisSeries->setColor(QColor(70,121,229));}
    else if(ui->comboBox_2->currentIndex()==2){thisSeries->setColor(QColor(220,59,59));}
    else if(ui->comboBox_2->currentIndex()==3){thisSeries->setColor(QColor(50,226,146));}
    else if(ui->comboBox_2->currentIndex()==4){thisSeries->setColor(QColor(185,107,222));}
    else if(ui->comboBox_2->currentIndex()==5){thisSeries->setColor(QColor(64,60,60));}
    //подключаем
    ui->graphicsView->chart()->addSeries(thisSeries);
    thisSeries->attachAxis(axisY);
    thisSeries->attachAxis(axisX);
    thisSeries->setOpacity(0.2);
    //добавляем текст
    if( ui->checkBox_name->isChecked() ){ thisSeries->setName(ui->lineEdit->text()); }
    else{ thisSeries->setName(model->itemFromIndex(ui->treeView->selectionModel()->currentIndex())->text()); }
    //ищем масксимум и минимум
    if(max!=nullptr){
        if(p.size()){*min = *max = p[0].y();}
        for(qsizetype i = 0; i<p.size(); i++){
            if(*min>p[i].y()){*min=p[i].y();}
            if(*max<p[i].y()){*max=p[i].y();}
        }
    }
}

void MainWindow::open_csv(){
    QString path = QFileDialog::getOpenFileName(this, tr("Открыть файл"), "", tr("CSV (*.csv)"));
    if(!path.isEmpty()){
        clear_tree_view();
        download::csv(path, data_buff);
        load_tree_view();
    }
}

void MainWindow::open_json(){
    QString path = QFileDialog::getOpenFileName(this, tr("Открыть файл"), "", tr("JSON (*.json)"));
    if(!path.isEmpty()){
        clear_tree_view();
        download::json(path, data_buff);
        load_tree_view();
    }
}

void MainWindow::close(){
    QWidget::close();
}

void MainWindow::add_clear_graph(){
    ui->graphicsView->chart()->removeAllSeries();//удаляем все графики
    ui->graphicsView->chart()->setTitle("NAME");//устанавливаем старый заголовок
    thisSeries = nullptr;
    blocked_tab();                      //блокируем содержимое второй вкладки
    ui->tabWidget->setCurrentIndex(0);  //переключаемся на первую вкладку
    ui->treeView->clearFocus();         //удаляем фокус с treeView
}

void MainWindow::about(){
    QMessageBox::about(this, "О программе", "");
}

void MainWindow::blocked_tab(){
    //возвращение параметров вкладки к изначальным значениям
    ui->tab_2->setEnabled(false);
    ui->checkBox_name->setChecked(false);
    ui->lineEdit->setText("По умолчанию");
    ui->lineEdit->setEnabled(false);
    ui->checkBox_date->setChecked(false);
    ui->checkBox->setChecked(false);
    ui->dateTimeEdit->setEnabled(false);
    ui->dateTimeEdit_2->setEnabled(false);
    ui->comboBox->setCurrentIndex(0);
    ui->comboBox_2->setCurrentIndex(0);
    ui->comboBox_3->setCurrentIndex(0);
}

void MainWindow::check_tree_view(const QModelIndex &index){
    if(index.data(Qt::UserRole+1).toBool()){
        ui->tab_2->setEnabled(true);
        double max, min;
        update_tab(&min, &max);

        std::vector <QDateTime>* dat = data_buff.at(index.parent().row())->getDate();
        axisX->setRange(dat->at(0), dat->at(dat->size()-1));
        //устанавливаем заголовок графика
        if(ui->graphicsView->chart()->title()=="NAME"){ui->graphicsView->chart()->setTitle(data_buff.at(index.parent().row())->getName());}
        //устанавливаем даты
        ui->dateTimeEdit_2->setDateTime(dat->at(0));//стартовое значение
        ui->dateTimeEdit->setDateTime(dat->at(dat->size()-1)); //конечное значение
        ui->dateTimeEdit_3->setDateTime(dat->at(0));//стартовое значение
        ui->dateTimeEdit_4->setDateTime(dat->at(dat->size()-1));//конечное значение
        //устанавливаем значения
        ui->spinBox->setValue(min); //минимум
        ui->spinBox_2->setValue(max); //максимум
        //переключаемся на вторую вкладку
        ui->tabWidget->setCurrentIndex(1);
        //устанавливаем максимумы/минимумы графика
        axisY->setRange(min, max);
    }
}

void MainWindow::click_button(){
    ui->tabWidget->setCurrentIndex(0);
    ui->treeView->clearFocus();
    double max, min;
    update_tab(&min, &max);
    blocked_tab();

    axisY->setRange(min, max);
    qDebug()<<min<<max;
    thisSeries->setOpacity(1);
    thisSeries = nullptr;
}

void MainWindow::click_check_but(bool n){ update_tab(); }

void MainWindow::click_combo_box(int n){ update_tab(); }

void MainWindow::click_line_edit(QString n){ update_tab(); }

void MainWindow::click_date(){ update_tab(); }

void MainWindow::text_y(QString n){axisY->setTitleText(n);}

void MainWindow::text_x(QString n){axisX->setTitleText(n);}

void MainWindow::y_max_min(int n){axisY->setRange(ui->spinBox->value(), ui->spinBox_2->value());}

void MainWindow::x_max_min(){axisX->setRange(ui->dateTimeEdit_3->dateTime(), ui->dateTimeEdit_4->dateTime());}

void MainWindow::correction(QVector<QPointF>& p){
    // min=p[0].y(), max=p[0].y();
    double C = 50;
    std::vector <double> buff;

    for(qsizetype i = 0; i < p.size(); i++){
        buff.push_back(p[i].y());

        if(buff.size()==3){
            double corner = abs(buff[1]-buff[0]); //вычисляем насколько отличается вторая точка от первой

            if(abs(abs((buff[2]-buff[1]))-corner) >= C){ //если отклонение выше определённого порога, то производим коррекцию
                p[i].setY(buff[1]+((buff[1]-buff[0])/2));
                buff[2] = buff[1]+((buff[1]-buff[0])/2);
            }
            buff.erase(buff.begin());
        }
    }
}

void MainWindow::averaging(QVector<QPointF>& p){
    if(p.size()){ //если массив не пустой
        QVector<QPointF> buff;
        qreal time = p.at(0).x(), summ_buff=0;
        qreal tik_time; //округление в миллисикундах
        if(ui->comboBox->currentIndex()==1){tik_time = 3600000;} //1 час
        else if(ui->comboBox->currentIndex()==2){tik_time = 10800000;} //3 часа
        else if(ui->comboBox->currentIndex()==3){tik_time = 86400000;} //сутки
        int num = 1;
        for(qsizetype i = 0; i < p.size(); i++, num++){
            summ_buff += p[i].y();
            if(((p[i].x()-time) >= tik_time) || ((i+1) == p.size())){
                buff.push_back(QPointF(((p[i].x()-time)/2)+time, summ_buff/num));
                num = summ_buff = 0;
                time = p[i].x();
            }
        }
        p = buff;
    }
}

