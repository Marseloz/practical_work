#ifndef DOWNLOAD_H
#define DOWNLOAD_H
#include <QString>
#include <QFile>
#include "storage_data.h"

namespace download{
    void csv(QString path, std::vector <DATA*> &buff_data);
    void json(QString path, std::vector <DATA*> &buff_data);
}

#endif //DOWNLOAD_H
