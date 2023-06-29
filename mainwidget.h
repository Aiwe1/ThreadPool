#pragma once

#include <QObject>
#include <QWidget>
#include <string>

class QLineEdit;
class QComboBox;
class QTableView;
class QStandardItemModel;
class ThreadPool;
class QPushButton;

class MainWindow: public QWidget
{
    Q_OBJECT
public:
    MainWindow(QWidget *ptr);

    ~MainWindow();
private:
    int addRow(QString name, QString status, QString result, int func_index);
    void changeRow(int func_index, QString status, QString result = QString());
    void deleteRow(int index);

    QLineEdit* threads_le_ = nullptr;
    QLineEdit* task_le_ = nullptr;
    QComboBox* task_cmb_ = nullptr;
    QTableView* table_ = nullptr;
    QStandardItemModel* model_ = nullptr;
    ThreadPool* tp_ = nullptr;
    QPushButton* start_btn_ = nullptr;
    QPushButton* stop_btn_ = nullptr;
    QPushButton* task_btn_ = nullptr;

    bool is_active_ = false;
    int last_index_ = 0;

public slots:
    void functionReleased(int index, std::string result);
    void functionInProgress(int index);

private slots:
    void onBtnAddClicked();
    void onBtnStartClicked();
    void onBtnStopClicked();
    void onDeleteRow();
    void slotMenuRequested(QPoint pos);

};










