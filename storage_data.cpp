#include "storage_data.h"


DATA::~DATA(){
    sensName.clear();
    for(size_t i=0; i < sensData.size(); i++){
        if(sensData.at(i) != nullptr){
            delete sensData.at(i);
        }
    }
    sensData.clear();
}

bool DATA::setName(QString _name){
    if(name != _name){
        name = _name;
        return true;
    }
    return false;
}

QString& DATA::getName(){ return name; }

bool DATA::setId(QString _id){
    if(id != _id){
        id = _id;
        return true;
    }
    return false;
}

QString& DATA::getId(){ return id; }

int DATA::setSensName(QString _name, bool addNewNames){
    //проверка наличия имени датчика в массиве с именами датчиков
    if(!sensName.empty()){
        for(size_t i=0; i<sensName.size(); i++){
            if(sensName.at(i)==_name){ return i; }
        }
    }
    if(addNewNames){
        //добавление в массив, если имя не найдено
        sensName.push_back(_name);
        sensData.push_back(new std::vector <double>);
        return sensName.size()-1;
    }
    else{ return -1; }
}

std::vector<QString>& DATA::getSensName(){
    return sensName;
}


bool isNum(QString str){
    bool dot=false, minus=false;
    for(qsizetype i = 0; i<str.size(); i++){
        if(((str.at(i)<'0') || (str.at(i)>'9')) && ((str.at(i)!='.') && (str.at(i)!='-'))){
            return false;
        }
        else if((str.at(i)=='.') && dot){return false;}
        else if((str.at(i)=='-') && minus){return false;}

        if(str.at(i)=='.'){dot = true;}
        else if(str.at(i)=='-'){minus = true;}
    }
    return true;
}

bool DATA::setSensNameData(QString _name, QString _data, QString _dtime){
    //разделение полученной строки на дату и время //2021-07-11 02:05:07
    QStringList values_data = _dtime.split(' ', Qt::SkipEmptyParts)[0].split('-', Qt::SkipEmptyParts);
    QStringList values_time = _dtime.split(' ', Qt::SkipEmptyParts)[1].split(':', Qt::SkipEmptyParts);

    //преобразование даты и времени в нужный формат
    QDateTime momentInTime;
    momentInTime.setDate(QDate(values_data[0].toInt(), values_data[1].toInt(), values_data[2].toInt()));
    momentInTime.setTime(QTime(values_time[0].toInt(), values_time[1].toInt(), values_time[2].toInt()));

    //проверка введённой даты на соответствие с последней записаной датой в массиве
    if(!senDate.empty()){
        if(senDate.back() < momentInTime){ senDate.push_back(momentInTime); }
    }
    else{
        senDate.push_back(momentInTime);
    }

    //получение позиции имени датчика в массиве имён датчиков
    int k = setSensName(_name);

    //добавление данных
    if(sensData[k] != nullptr){
        if(isNum(_data)){
            sensData[k]->push_back(_data.toDouble());
        }
        else if((_data == "none")||(_data == "")){
            sensData[k]->push_back(0);
        }
        else{
            delete sensData[k];
            sensData[k] = nullptr;
        }
    }
    return true;
}

std::vector<std::vector<double>*>& DATA::getData(){
    return sensData;
}


QVector <QPointF>* DATA::getSeries(QString _name, double *max, double *min){
    return getSeries(setSensName(_name, false), max, min);
}

QVector <QPointF>* DATA::getSeries(int num, double *max, double *min){
    if(num < 0){return nullptr;}
    QVector <QPointF>* p = new QVector <QPointF>;

    if(max != nullptr){
        *max = sensData.at(num)->at(0);
        if(min != nullptr){ *min = sensData.at(num)->at(0); }
    }

    //заполняем данными
    for(size_t i=0; i<sensData.at(num)->size(); i++){
        p->push_back(QPointF(senDate.at(i).toMSecsSinceEpoch(), sensData.at(num)->at(i)));
        if(max != nullptr){
            if(*max < sensData.at(num)->at(i)){ *max = sensData.at(num)->at(i); }
            if(min != nullptr){ if(*min > sensData.at(num)->at(i)){ *min = sensData.at(num)->at(i); } }
        }
    }
    return p;
}

std::vector<QDateTime>* DATA::getDate(){return &senDate;}
