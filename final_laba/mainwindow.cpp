#include "mainwindow.h"
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QMessageBox>
#include <QHeaderView>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    user = new QLineEdit(this);
    products = new QTableWidget(this);
    resize(440, 230);
    products->setColumnCount(4);
    products->setHorizontalHeaderLabels(QStringList() << "Название" << "Количество" << "Цена" << "Заказать");

    button = new QPushButton("Заказ", this);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(user);
    layout->addWidget(products);
    layout->addWidget(button);
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    connect(button, &QPushButton::clicked, this, &MainWindow::click);


    QFile chequeFile("C:/Users/Zenuls/Documents/final_laba/cheque.txt");
    if (chequeFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        chequeFile.close();
    } else {
        qDebug() << "Не удалось очистить файл.";
    }
    load();
}

void MainWindow::update() {
    QFile file("C:/Users/Zenuls/Documents/final_laba/products.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл.";
        return;
    }
    QTextStream out(&file);

    products->clearContents();
    products->setRowCount(0);
    qDeleteAll(spin_boxes);
    spin_boxes.clear();

    for (int row = 0; row < products->rowCount(); ++row) {
        QString name = products->item(row, 0)->text();
        QString quantity = products->item(row, 1)->text();
        QString price = products->item(row, 2)->text();
        out << name << " - " << quantity << " | " << price << "\n";
    }

    file.close();
}


void MainWindow::load() {
    QFile file("C:/Users/Zenuls/Documents/final_laba/products.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл.";
        return;
    }
    QTextStream in(&file);
    products->setRowCount(0);
    int row = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(" - ");
        if (parts.size() == 2) {
            QStringList secondPart = parts[1].split(" | ");
            if (secondPart.size() == 2) {
                products->insertRow(row);
                products->setItem(row, 0, new QTableWidgetItem(parts[0].trimmed()));
                int quantity = secondPart[0].toInt();
                products->setItem(row, 1, new QTableWidgetItem(QString::number(quantity)));
                products->setItem(row, 2, new QTableWidgetItem(secondPart[1].trimmed()));
                QSpinBox *spinBox = new QSpinBox(this);
                spinBox->setMinimum(0);
                spinBox->setMaximum(quantity);
                spin_boxes.append(spinBox);
                products->setCellWidget(row, 3, spinBox);
                row++;
            }
        }
    }
    file.close();
}

void MainWindow::click() {
    struct OrderInfo {
        QString name;
        int quantityOrdered;
        int price;
    };
    QList<OrderInfo> orders;

    for (int row = 0; row < products->rowCount(); ++row) {
        QString name = products->item(row, 0)->text();
        int quantityAvailable = products->item(row, 1)->text().toInt();
        int price = products->item(row, 2)->text().toInt();
        int quantityOrdered = spin_boxes[row]->value();

        if (quantityOrdered > 0 && quantityOrdered <= quantityAvailable) {
            quantityAvailable -= quantityOrdered;
            products->item(row, 1)->setText(QString::number(quantityAvailable));
            orders.append({name, quantityOrdered, price});
        }
    }

    QFile file("C:/Users/Zenuls/Documents/final_laba/products.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл.";
        return;
    }
    QTextStream out(&file);
    for (int row = 0; row < products->rowCount(); ++row) {
        QString name = products->item(row, 0)->text();
        QString quantity = products->item(row, 1)->text();
        QString price = products->item(row, 2)->text();
        out << name << " - " << quantity << " | " << price << "\n";
    }
    file.close();

    QFile chequeFile("C:/Users/Zenuls/Documents/final_laba/cheque.txt");
        if (!chequeFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
         qDebug() << "Не удалось очиститть файл.";
        return;
    }
    QTextStream chequeOut(&chequeFile);
    chequeOut << "Пользователь: " << user->text() << "\n";
    int totalSum = 0;
    for (const OrderInfo &order : orders) {
        int sum = order.quantityOrdered * order.price;
        totalSum += sum;
        chequeOut << order.name << ": " << order.quantityOrdered <<" шт. - " <<  sum << " руб." << "\n";
    }
    chequeOut << "Итого: " << totalSum << " руб." << "\n\n";
    chequeFile.close();

    QMessageBox::information(this, "Заказ", "Ваш заказ оформлен");
    load();
}

