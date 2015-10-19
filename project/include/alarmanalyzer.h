#ifndef ALARMANALYZER_H
#define ALARMANALYZER_H

#include <QWidget>

class QLineEdit;
class QDateEdit;
class QTimeEdit;
class QTreeView;
class QLabel;
class UPlotWidget;
class TableModel;
class AlarmFilterModel;

class AlarmAnalyzer : public QWidget
{
    Q_OBJECT
public:
    explicit AlarmAnalyzer(QWidget *parent = 0);
    ~AlarmAnalyzer();

signals:

private slots:
    void DateTimeFilterChanged();
    void TextFilterChanged();
    bool set_model_data();
    void Analyzer();

private:
    bool SelectFromTable(const std::string &table, const std::string &key, const std::string &val, std::vector<std::map<std::string, std::string> > &res);
    void HistoricalPlot(std::string task_name, std::string str);
    double DateTimeStringToDouble(std::string str);

    QLineEdit *filter_;
    QDateEdit *date_from_;
    QDateEdit *date_to_;
    QTimeEdit *time_from_;
    QTimeEdit *time_to_;

    QTreeView *treeview_;

    QLabel *frame_src_;
    QLabel *frame_dst_;

    UPlotWidget *plot_;

    TableModel *alarm_model_;
    AlarmFilterModel *filter_model_;
};

#endif // ALARMANALYZER_H
