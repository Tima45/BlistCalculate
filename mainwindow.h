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


namespace Ui {
class MainWindow;
}
using namespace cv;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void processPictures(QString path);
    void calculateDifferense(QDir dir);
    double calculateIntegralBright(Mat pic);
    QFileInfo findFirstFile(QDir dir);

    double diffPicPic(Mat pic1, Mat pic2);
    ~MainWindow();
    void initPlot();
    void drawDifference();
private slots:
    void on_selectFolderButton_clicked();

private:
    Ui::MainWindow *ui;

    QRegExp tampleFileName;
    Mat startPicture;

    QVector<double> pictureDifference;
    QCPGraph *diffGraph;
};

#endif // MAINWINDOW_H
