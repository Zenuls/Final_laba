#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include <QCheckBox>
#include <QSpinBox>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void click();

private:
    void load();
    void update();

    QLineEdit *user;
    QTableWidget *products;
    QPushButton *button;
    QVector<QSpinBox*> spin_boxes;
};

#endif
