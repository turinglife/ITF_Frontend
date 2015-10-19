#ifndef UANALYZEBOARD_H
#define UANALYZEBOARD_H

#include <QWidget>

class QLabel;

class UAnalyzeBoard : public QWidget
{
    Q_OBJECT
public:
    explicit UAnalyzeBoard(QWidget *parent = 0);
    ~UAnalyzeBoard();

    void set_channel(int channel);
    void set_task_name(std::string task_name);
    void set_task_type(std::string type);
    std::string task_type();
    void set_analyze_board();
    void RemoveLayouts();
    QVector<QLayout *> layouts();

signals:
    void BackToTaskBoard(int);

public slots:
private slots:
    void OnBtnBackClicked();

private:
    int channel_;
    std::string task_type_;

    QLabel *task_name_;

    QVector<QLayout *> layouts_;
};

#endif // UANALYZEBOARD_H
