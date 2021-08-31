#include "download.h"
#include <QTextCodec>
#include <QJsonObject>
#include <QJsonDocument>

void download::csv(QString path, std::vector <DATA*> &buff_data){

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)){return;}
    buff_data.push_back(new DATA);

    int i = 0;

    QTextCodec* codec1251 = QTextCodec::codecForName("Windows-1251");
    while(!file.atEnd()){
        // читаем строку
        QByteArray bstr = file.readLine();
        // меняем кодировку
        QString str = codec1251->toUnicode(bstr);
        // удаляем ";\n" из конца
        str.chop(2);
        // делим строку на отдельные части
        QStringList lst = str.split(";");
        // загружаем
        if(!lst.empty()){
            if(i==0){
                buff_data.back()->setName(lst.at(1).split(" (").at(0));
                buff_data.back()->setId("("+lst.at(1).split(" (").at(1));
            }
            else if(i==1){
                for(int k = 1; k < lst.size(); k++){
                   buff_data.back()->setSensName(lst.at(k));
                }
            }
            else{
                for(size_t k = 0; k < buff_data.back()->getSensName().size(); k++){
                   buff_data.back()->setSensNameData(buff_data.back()->getSensName().at(k), lst.at(k+1), lst.at(0));
                }
            }
        }
        i++;
    }
    file.close();
}

void download::json(QString path, std::vector <DATA*> &buff_data){
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)){return;}
    QJsonDocument json = QJsonDocument::fromJson(file.readAll());
    QJsonObject jsonObj = json.object();

    for(const auto& i : jsonObj.keys()){
        QJsonValue value = jsonObj.value(QString(i));
        QJsonObject item = value.toObject();

        QString date = item.value(QString("Date")).toString();
        QString uName = item.value(QString("uName")).toString();
        QString serial = item.value(QString("serial")).toString();

        item = item.value(QString("data")).toObject();
        DATA *buffElem = nullptr;
        for(size_t k=0; k <buff_data.size(); k++){
            if((buff_data[k]->getName()==uName)&&(buff_data[k]->getId()==serial)){buffElem = buff_data[k];}
        }
        if(buffElem == nullptr){
            buff_data.push_back(new DATA);
            buff_data.back()->setName(uName);
            buff_data.back()->setId(serial);
            buffElem = buff_data.back();
        }

        for(const auto& n : item.keys()){
            buffElem->setSensNameData(n, item.value(n).toString(), date);
        }
    }
    //buff_data.push_back(new DATA);
}
