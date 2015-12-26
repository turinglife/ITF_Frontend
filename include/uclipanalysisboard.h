#ifndef UCLIPANALYSISBOARD_H
#define UCLIPANALYSISBOARD_H

#include <QWidget>
class QLabel;

class UClipAnalysisBoard : public QWidget
{
    Q_OBJECT
public:
    explicit UClipAnalysisBoard(QWidget *parent = 0);
    ~UClipAnalysisBoard();

    inline void set_channel(int channel) {
        channel_ = channel;
    }

    inline void set_task_type(std::string type) {
        task_type_ = type;
    }

    void set_task_name(std::string task_name);

    inline std::string task_type() const {
        return task_type_;
    }

    void set_analyze_board();
    void RemoveLayouts();
    QVector<QLayout *> layouts();
signals:
    void BackToClipBoard(int);

private slots:
    void OnBtnBackClicked();

private:
    int channel_;

    std::string task_type_;

    QLabel *p_task_name_;

    QVector<QLayout *> layouts_;
};

#endif // UCLIPANALYSISBOARD_H
