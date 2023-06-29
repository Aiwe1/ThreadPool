#include "mainwidget.h"

#include <QComboBox>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>
#include <QTableView>
#include <QAbstractTableModel>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QAction>
#include <QMenu>
#include <QRegExp>

#include "threads_pool.h"
#include "functions.h"

QStringList functions = QStringList() << "Фибоначи" << "Факториал" << "Двойной факториал";

MainWindow::MainWindow(QWidget *ptr = nullptr) : QWidget(ptr)
{
    int thread_count = 5;

    QVBoxLayout* main_layout = new QVBoxLayout;
    QHBoxLayout* threads_layout = new QHBoxLayout;

    QLabel* threads_lbl = new QLabel("Количество потоков");
    threads_le_ = new QLineEdit;
    threads_le_->setValidator( new QIntValidator(0, 100, this) );
    threads_le_->setText(QString::number(thread_count));
    threads_layout->addWidget(threads_le_, 0, Qt::AlignLeft);
    threads_layout->addWidget(threads_lbl, 0, Qt::AlignLeft);
    threads_layout->addStretch();
    main_layout->addLayout(threads_layout);

    table_ = new QTableView;
    model_ = new QStandardItemModel();
    QStringList headers;
    headers << "Название" << "Статус" << "Результат";
    model_->setHorizontalHeaderLabels(headers);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setModel(model_);
    table_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setContextMenuPolicy(Qt::CustomContextMenu);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(table_, &QTableView::customContextMenuRequested, this, &MainWindow::slotMenuRequested, Qt::AutoConnection);
    main_layout->addWidget(table_);

    QHBoxLayout* task_layout = new QHBoxLayout;
    task_le_ = new QLineEdit;
    task_le_->setValidator( new QIntValidator(0, 10000000, this) );
    task_cmb_ = new QComboBox();
    task_cmb_->addItems(functions);
    task_btn_ = new QPushButton("Добавить");
    connect(task_btn_, &QPushButton::clicked, this, &MainWindow::onBtnAddClicked, Qt::AutoConnection);
    //task_btn_->setEnabled(false);
    task_layout->addWidget(task_le_, 0, Qt::AlignLeft);
    task_layout->addWidget(task_cmb_, 0, Qt::AlignLeft);
    task_layout->addWidget(task_btn_, 0, Qt::AlignLeft);
    task_layout->addStretch();
    main_layout->addLayout(task_layout);

    QHBoxLayout* ss_layout = new QHBoxLayout;
    start_btn_ = new QPushButton("Старт");
    start_btn_->setFixedSize(150, 50);
    connect(start_btn_, &QPushButton::clicked, this, &MainWindow::onBtnStartClicked, Qt::AutoConnection);
    stop_btn_ = new QPushButton("Стоп");
    stop_btn_->setFixedSize(150, 50);
    connect(stop_btn_, &QPushButton::clicked, this, &MainWindow::onBtnStopClicked, Qt::AutoConnection);
    stop_btn_->setEnabled(false);
    ss_layout->addWidget(start_btn_, 0, Qt::AlignLeft);
    ss_layout->addWidget(stop_btn_, 0, Qt::AlignLeft);
    ss_layout->addStretch();
    main_layout->addLayout(ss_layout);

    this->setLayout(main_layout);

    QPalette pal(palette());
    pal.setColor(QPalette::Background, Qt::gray);
    this->setAutoFillBackground(true);
    this->setPalette(pal);
    this->setWindowTitle("Пул Потоков");
    this->resize(700, 700);
}

MainWindow::~MainWindow()
{
    if (tp_) {
        delete tp_;
    }
}

int MainWindow::addRow(QString name, QString status, QString result, int func_index)
{
    QStandardItem* item_name = new QStandardItem(name);
    QStandardItem* item_status = new QStandardItem(status);
    QStandardItem* item_res = new QStandardItem(result);
    QStandardItem* item_index = new QStandardItem(QString::number(func_index));
    QList<QStandardItem*> row;
    row.append(item_name);
    row.append(item_status);
    row.append(item_res);
    row.append(item_index);
    model_->appendRow(row);
    table_->setColumnHidden(3, true);
    return model_->rowCount() - 1;
}

void MainWindow::changeRow(int func_index, QString status, QString result)
{
    for (int index = 0; index < model_->rowCount(); ++index) {
        QStandardItem* elem = model_->item(index, 3);

        if (elem->text().toInt() == func_index) {
            model_->item(index, 1)->setText(status);
            if (result != QString()) {
                model_->item(index, 2)->setText(result);
            }
        }
    }
}

void MainWindow::deleteRow(int index)
{
    if (tp_) {
        int idx = model_->item(index, 3)->text().toInt();
        tp_->deleteTask(idx);
    }
    model_->removeRow(index);
}

void MainWindow::functionReleased(int index, std::string result)
{
    changeRow(index, "Сделано", QString::fromStdString(result));
}

void MainWindow::functionInProgress(int index)
{
    changeRow(index, "В Обработке");
}

void MainWindow::onBtnAddClicked()
{
    QString name;

    if (task_cmb_->currentIndex() >= 0 && task_cmb_->currentIndex() < functions.size()) {
        name = functions.at(task_cmb_->currentIndex()) + " (" + task_le_->text() + ")";
    }

    if (is_active_) {
        if (task_cmb_->currentIndex() == 0) {
            int idx = last_index_++;
            tp_->addTask(idx, Fibonachi, task_le_->text().toInt());
            addRow(name, "В Очереди", "", idx);
        } else if (task_cmb_->currentIndex() == 1) {
            int idx = last_index_++;
            tp_->addTask(idx, Factorial, task_le_->text().toInt());
            addRow(name, "В Очереди", "", idx);
        } else if (task_cmb_->currentIndex() == 2) {
            int idx = last_index_++;
            tp_->addTask(idx, dblFactorial, task_le_->text().toInt());
            addRow(name, "В Очереди", "", idx);
        }
    }
    else {
        if (task_cmb_->currentIndex() == 0) {
            addRow(name, "Ожидает", "", last_index_++);
        } else if (task_cmb_->currentIndex() == 1) {
            addRow(name, "Ожидает", "", last_index_++);
        } else if (task_cmb_->currentIndex() == 2) {
            addRow(name, "Ожидает", "", last_index_++);
        }
    }

}

void MainWindow::onBtnStartClicked()
{
    if (tp_ != nullptr) {
        return;
    }

    tp_ = new ThreadPool(threads_le_->text().toInt());
    connect(tp_, &ThreadPool::functionReleased, this, &MainWindow::functionReleased, Qt::QueuedConnection);
    connect(tp_, &ThreadPool::functionInProgress, this, &MainWindow::functionInProgress, Qt::QueuedConnection);
    //task_btn_->setEnabled(true);
    threads_le_->setEnabled(false);
    start_btn_->setEnabled(false);
    stop_btn_->setEnabled(true);

    is_active_ = true;

    for (int index = 0; index < model_->rowCount(); ++index) {
        QStandardItem* elem = model_->item(index, 1);

        if (elem->text() == "Ожидает") {
            int func_index = model_->item(index, 3)->text().toInt();
            QString name = model_->item(index, 0)->text();

            int value = 0;
            QRegExp rx("([0-9]+)");
            if (rx.indexIn(name) != -1) {
                value = rx.cap(0).toInt();
            }
            //qDebug() << value << "   " << name;

            if (name.indexOf(functions.at(0)) != -1) {
                tp_->addTask(func_index, Fibonachi, value);
            }
            else if (name.indexOf(functions.at(1)) != -1) {
                tp_->addTask(func_index, Factorial, value);
            }
            else if (name.indexOf(functions.at(2)) != -1) {
                tp_->addTask(func_index, dblFactorial, value);
            }

            changeRow(func_index, "В Очереди");

        }
    }
}

void MainWindow::onBtnStopClicked()
{
    if(tp_) {
        delete tp_;
        tp_ = nullptr;
        disconnect(tp_, &ThreadPool::functionReleased, this, &MainWindow::functionReleased);
        disconnect(tp_, &ThreadPool::functionInProgress, this, &MainWindow::functionInProgress);
    }

    start_btn_->setEnabled(true);
    stop_btn_->setEnabled(false);
    threads_le_->setEnabled(true);

    is_active_ = false;

    for (int index = 0; index < model_->rowCount();) {
        QStandardItem* elem = model_->item(index, 1);

        if (elem->text() != "Сделано" && elem->text() != "В Обработке") {
            deleteRow(index);
            continue;
        }
        ++index;
    }
}

void MainWindow::onDeleteRow()
{
    QItemSelectionModel *select = table_->selectionModel();

    deleteRow(select->selectedRows().first().row());
}

void MainWindow::slotMenuRequested(QPoint pos)
{
    QMenu* menu = new QMenu(this);
    QAction* delete_row = new QAction("Удалить");
    connect(delete_row, &QAction::triggered, this, &MainWindow::onDeleteRow, Qt::AutoConnection);
    menu->addAction(delete_row);
    menu->popup(table_->viewport()->mapToGlobal(pos));
}





