#ifndef ALARMMODULE_H
#define ALARMMODULE_H

#include <QWidget>
class QLineEdit;
class QDateEdit;
class QTimeEdit;
class QTreeView;
class QLabel;
class UPlotWidget;
class TableModel;
class AlarmFilterModel;
class UClipWidget;

class AlarmModule : public QWidget
{
    Q_OBJECT
public:
    explicit AlarmModule(QWidget *parent = 0);
    ~AlarmModule();

    void set_clear();

private slots:
    void DateTimeFilterChanged();
    void TextFilterChanged();
    bool set_model_data();
    void OnTreeViewDoubleClicked();

private:
    void ShowAlarmDetail(std::string task_name, std::string datetime, QString img_name);
    bool ShowImage(std::string task_name, QString img_name);
    void HistoricalPlot(std::string task_name, std::string str);
    double DateTimeStringToDouble(std::string str);

    QLineEdit *p_filter_;
    QDateEdit *p_date_from_;
    QDateEdit *p_date_to_;
    QTimeEdit *p_time_from_;
    QTimeEdit *p_time_to_;

    QTreeView *p_treeview_;

    UClipWidget *p_frame_src_;
    UClipWidget *p_frame_dst_;
    UPlotWidget *p_plot_;

    TableModel *p_alarm_model_;
    AlarmFilterModel *p_filter_model_;
};

#endif // ALARMMODULE_H
