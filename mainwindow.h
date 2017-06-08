#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include "plot/qcustomplot.h"
#include <cv.hpp>
#include <QTime>
#include <QRegExp>
#include "blistpicture.h"
#include <QAxObject>
#include <QThread>
#include "loadingthread.h"



namespace Ui {
class MainWindow;
}
using namespace cv;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void processPictures(QString path);

    double calculateIncfluensTo(QTime time);
    void deletePictures();

    void initPlot();
    void drawDifference();
private slots:
    void on_selectFolderButton_clicked();


    void loadingStatusChanged(QString text);
    void loadingEnded();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    QVector<BlistPicture*> blistPictures;
    QVector<QTime> times;
    QVector<double> current;

    LoadingThread *loadingThread = nullptr;

    QRegExp tampleFileName;
    QSharedPointer<QCPAxisTicker> standartTicker;
};

#endif // MAINWINDOW_H
