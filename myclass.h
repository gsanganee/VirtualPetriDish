#ifndef MYCLASS_H
#define MYCLASS_H

#include <QWidget>
#include <QMessageBox>

class MyClass : public QWidget
{
    Q_OBJECT

public:
    MyClass(QWidget* parent=0)
        :QWidget(parent){}

    ~MyClass(){}

public slots:
    void showModified(const QString& str)
    {
        Q_UNUSED(str)
        QMessageBox::information(this,"Directory Modified", "Your Directory is modified");
    }
};

#endif // MYCLASS_H
