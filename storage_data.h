#ifndef STORAGE_DATA_H
#define STORAGE_DATA_H
#include <QString>
#include <QDateTime>
#include <vector>
#include <QXYSeries>
#include <QLineSeries>

struct paramSetting{
    bool edit_name;
    QString name;
    bool correctArtefact;//коррекция артефактов
    bool typeGraph;//тип графика
    int averagingNum;//осреднение
    int colorNum;//цвет
    bool edit_date;
    QDateTime datStart;//начальная дата
    QDateTime datEnd;//конечная дата
};

class DATA{
private:
    QString name;                               //имя устройства
    QString id;                                 //id устройства
    std::vector <QString> sensName;             //имена сенсоров
    std::vector <std::vector<double>*> sensData;//данные сенсоров
    std::vector <QDateTime> senDate;            //время
public:
    ~DATA();
    bool setId(QString _id);
    QString& getId();
    bool setName(QString _name);
    QString& getName();
    int setSensName(QString _name, bool addNewNames = true);
    std::vector <QString>& getSensName();
    bool setSensNameData(QString _name, QString _data, QString _dtime);
    std::vector <std::vector<double>*>& getData();
    QVector <QPointF>* getSeries(QString _name, double *max = nullptr, double *min = nullptr);
    QVector <QPointF>* getSeries(int num, double *max = nullptr, double *min = nullptr);
    std::vector <QDateTime>* getDate();
};

#endif // STORAGE_DATA_H
