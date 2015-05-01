#include <QCoreApplication>
#include "qstringtokenizer.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringTokenizer tokenizer("ABCD\tEFG\fHIJKLM PQR");
    while (tokenizer.hasMoreTokens()) {
        qDebug()<<tokenizer.nextToken();
    }
    return a.exec();
}
