#include "alarmmodule.h"
#include <QDateEdit>
#include <QTimeEdit>
#include <QTreeView>
#include <QLineEdit>
#include "utility.h"
#include "tablemodel.h"
#include "alarmfiltermodel.h"
#include "uplotwidget.h"
#include "uclipwidget.h"
#include <QDebug>

AlarmModule::AlarmModule(QWidget *parent) : QWidget(parent)
{
//! [Alarm Model]
    p_alarm_model_ = new TableModel(this);

//![Filter Model]
    p_filter_model_ = new AlarmFilterModel(this);
    p_filter_model_->setSourceModel(p_alarm_model_);

//! [Left]
    //! [Date Time From]
    QLabel *p_lab_from = new QLabel(tr("&From"), this);
    p_date_from_ = new QDateEdit(this);
    p_date_from_->setDate(QDate::currentDate());
    p_date_from_->setDisplayFormat("yyyy-MM-dd");
    p_date_from_->setCalendarPopup(true);
    p_time_from_ = new QTimeEdit(this);
    p_time_from_->setTime(QTime::currentTime());
    p_time_from_->setDisplayFormat("hh:mm:ss");
    p_lab_from->setBuddy(p_date_from_);
    p_lab_from->setBuddy(p_time_from_);

    //! [Date Time From]
    QLabel *p_lab_to = new QLabel(tr("&To"), this);
    p_date_to_ = new QDateEdit(this);
    p_date_to_->setDate(QDate::currentDate());
    p_date_to_->setDisplayFormat("yyyy-MM-dd");
    p_date_to_->setCalendarPopup(true);
    p_time_to_ = new QTimeEdit(this);
    p_time_to_->setTime(QTime::currentTime());
    p_time_to_->setDisplayFormat("hh:mm:ss");
    p_lab_to->setBuddy(p_date_to_);
    p_lab_to->setBuddy(p_time_to_);

//! [Filter]
    QLabel *p_lab_filter = new QLabel(tr("Filter:"));
    p_filter_ = new QLineEdit(this);
    p_lab_filter->setBuddy(p_filter_);

//! [Search Button]
    QPushButton *p_btn_search = new QPushButton(this);
    p_btn_search->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));

//! [FilterLayout]
    QHBoxLayout *p_lay_filter = new QHBoxLayout;
    p_lay_filter->addWidget(p_lab_from);
    p_lay_filter->addWidget(p_date_from_);
    p_lay_filter->addWidget(p_time_from_);
    p_lay_filter->addWidget(p_lab_to);
    p_lay_filter->addWidget(p_date_to_);
    p_lay_filter->addWidget(p_time_to_);
    p_lay_filter->addStretch();
    p_lay_filter->addWidget(p_lab_filter);
    p_lay_filter->addWidget(p_filter_);
    p_lay_filter->addWidget(p_btn_search);


//! [TreeView]
    p_treeview_ = new QTreeView(this);
    p_treeview_->setRootIsDecorated(false);
    p_treeview_->setAlternatingRowColors(true);
    p_treeview_->setModel(p_filter_model_);
    p_treeview_->setSortingEnabled(true);
    p_treeview_->sortByColumn(1, Qt::AscendingOrder);
    p_treeview_->setColumnHidden(3, true);

//! [Left Layout] [Left Group]
    QGroupBox *p_gbox_left = new QGroupBox(this);
    QVBoxLayout *p_lay_left = new QVBoxLayout(p_gbox_left);
    p_lay_left->setSpacing(0);
    p_lay_left->setContentsMargins(0, 0, 0, 0);

    p_lay_left->addLayout(p_lay_filter);
    p_lay_left->addWidget(p_treeview_);

//! [Right]
    //! [right Layout] [Left Group]
    QGroupBox *p_gbox_right = new QGroupBox(this);
    Utility::SetWdgSizePolicy(p_gbox_right, QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *p_lay_right = new QVBoxLayout(p_gbox_right);
    p_lay_right->setSpacing(0);
    p_lay_right->setContentsMargins(0, 0, 0, 0);

//! [Frame]
    QHBoxLayout *p_lay_frame = new QHBoxLayout;
    p_lay_frame->setSpacing(0);
    p_frame_src_ = new UClipWidget(this);
    p_frame_dst_ = new UClipWidget(this);
    Utility::SetLabelStyle(p_frame_src_);
    Utility::SetLabelStyle(p_frame_dst_);
    p_lay_frame->addWidget(p_frame_src_);
    p_lay_frame->addWidget(p_frame_dst_);

//! [Plot]
    QHBoxLayout *p_lay_plot = new QHBoxLayout;
    p_lay_plot->setSpacing(0);
    p_plot_ = new UPlotWidget(this);
    Utility::SetWdgSizePolicy(p_plot_, QSizePolicy::Expanding, QSizePolicy::Expanding);
    p_lay_plot->addWidget(p_plot_);

    p_lay_right->addLayout(p_lay_frame);
    p_lay_right->addLayout(p_lay_plot);

    QVBoxLayout *p_layout = new QVBoxLayout(this);

    QSplitter *p_splitter = new QSplitter(Qt::Horizontal, this);
    p_splitter->addWidget(p_gbox_left);
    p_splitter->addWidget(p_gbox_right);
    p_splitter->setStretchFactor(0 , 0);
    p_splitter->setStretchFactor(1, 1);

    p_layout->addWidget(p_splitter);

//! [setup connection]
    connect(p_date_from_, SIGNAL(dateChanged(QDate)), this, SLOT(DateTimeFilterChanged()));
    connect(p_date_to_, SIGNAL(dateChanged(QDate)), this, SLOT(DateTimeFilterChanged()));
    connect(p_time_from_, SIGNAL(timeChanged(QTime)), this, SLOT(DateTimeFilterChanged()));
    connect(p_time_to_, SIGNAL(timeChanged(QTime)), this, SLOT(DateTimeFilterChanged()));
    connect(p_filter_, SIGNAL(textChanged(QString)), this, SLOT(TextFilterChanged()));
    connect(p_btn_search, SIGNAL(clicked()), this, SLOT(set_model_data()));
    connect(p_treeview_, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(OnTreeViewDoubleClicked()));

//! [connect to DB]
    set_model_data();
}

AlarmModule::~AlarmModule()
{
    std::cout << "release AlarmModule" << std::endl;
}

void AlarmModule::set_clear()
{
    if (!set_model_data()) {
        return;
    }

    if (p_treeview_->model()->rowCount()>0) {
        QString datetime = p_treeview_->model()->index(0, 1).data().toString();
        QString task_name = p_treeview_->model()->index(0, 4).data().toString();
        QString img_name = p_treeview_->model()->index(0, 3).data().toString();

        ShowAlarmDetail(task_name.toStdString(), datetime.toStdString(), img_name);
    } else {
        p_frame_src_->clear();
        p_frame_dst_->clear();
        p_plot_->clear_plot();
    }
}

void AlarmModule::DateTimeFilterChanged()
{
    QDateTime datetime_from = QDateTime(p_date_from_->date(), p_time_from_->time());
    QDateTime datetime_to = QDateTime(p_date_to_->date(), p_time_to_->time());
    p_filter_model_->set_datetime_min(datetime_from);
    p_filter_model_->set_datetime_max(datetime_to);
}

void AlarmModule::TextFilterChanged()
{
    QRegExp reg_exp(p_filter_->text(), Qt::CaseSensitive, QRegExp::RegExp2);
    p_filter_model_->setFilterRegExp(reg_exp);
}

bool AlarmModule::set_model_data()
{
    std::vector<std::map<std::string, std::string> > res;
    if(! DBHelper::SelectFromDensityAlarmRecord(res))
        return false;

    p_alarm_model_->set_raw_data(res);

    return true;
}

void AlarmModule::OnTreeViewDoubleClicked()
{
    if (p_treeview_->model()->rowCount()<=0)
        return;

    QString datetime = p_treeview_->model()->index(p_treeview_->currentIndex().row(), 1).data().toString();
    QString task_name = p_treeview_->model()->index(p_treeview_->currentIndex().row(), 4).data().toString();
    QString img_name = p_treeview_->model()->index(p_treeview_->currentIndex().row(), 3).data().toString();

    ShowAlarmDetail(task_name.toStdString(), datetime.toStdString(), img_name);
}

void AlarmModule::ShowAlarmDetail(string task_name, string datetime, QString img_name)
{
    if (!ShowImage(task_name, img_name)) {
        return;
    }
    HistoricalPlot(task_name, datetime);
}

bool AlarmModule::ShowImage(string task_name, QString img_name)
{
    std::vector<std::map<std::string, std::string> > res;
    if (!DBHelper::SelectFromTable("Tasks", "task_name", task_name, res)) {
        return false;
    }
    if (res.empty()) {
        return false;
    }

    QString task_path = QString::fromStdString(res[0]["task_path"]);
    QString src_path = task_path + QString::fromStdString(kAlarmDir) + img_name + "_src.jpg";
    QString dst_path = task_path + QString::fromStdString(kAlarmDir) + img_name + "_dst.jpg";

    cv::Mat img_src = cv::imread(src_path.toStdString());
    cv::Mat img_dst = cv::imread(dst_path.toStdString());

//    if (!img_src.empty())
//        cv::cvtColor(img_src, img_src, CV_BGR2RGB);

//    if (!img_dst.empty())
//        cv::cvtColor(img_dst, img_dst, CV_BGR2RGB);

    p_frame_src_->ShowImage(img_src);
    p_frame_dst_->ShowImage(img_dst);

    return true;
}

void AlarmModule::HistoricalPlot(string task_name, string str)
{
    p_plot_->clear_plot();

    std::string datetime_min = QDateTime::fromTime_t(DateTimeStringToDouble(str) - 120).toString("yyyy-MM-dd HH:mm:ss").toStdString();
    std::string datetime_max = QDateTime::fromTime_t(DateTimeStringToDouble(str) + 120).toString("yyyy-MM-dd HH:mm:ss").toStdString();
//    std::cout << datetime_min << std::endl;
//    std::cout << datetime_max << std::endl;

    /* view video as camera */
    p_plot_->set_plot(DateTimeStringToDouble(datetime_min), DateTimeStringToDouble(datetime_max), kCameraTypeHttp, kTaskTypeCounting);

    std::vector<std::map<std::string, std::string> > res;
    if (!DBHelper::SelectFromTable("DensityPredict", "task_name", task_name, datetime_min, datetime_max, res))
        return;

    for (size_t i=0; i<res.size(); ++i) {
        double key = DateTimeStringToDouble(res[i]["date_time"]);
        int value = atoi(res[i]["count"].c_str());
        p_plot_->set_data(key, value);
    }
}

double AlarmModule::DateTimeStringToDouble(string str)
{
    QDateTime datetime = QDateTime::fromString(QString::fromStdString(str), "yyyy-MM-dd HH:mm:ss");
    return datetime.toMSecsSinceEpoch()/1000.0;
}

