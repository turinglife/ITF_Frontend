#include "alarmanalyzer.h"
#include <QLayout>
#include <QLabel>
#include <QSplitter>
#include <QGroupBox>
#include <QPushButton>
#include <QStyle>
#include <QDateEdit>
#include <QTimeEdit>
#include <QTreeView>
#include <QLineEdit>
#include "utility.h"
#include "tablemodel.h"
#include "alarmfiltermodel.h"
#include "uplotwidget.h"
#include <QDebug>

AlarmAnalyzer::AlarmAnalyzer(QWidget *parent) : QWidget(parent)
{
    //! [Alarm Model]
    alarm_model_ = new TableModel(this);

    //![Filter Model]
    filter_model_ = new AlarmFilterModel(0);
    filter_model_->setSourceModel(alarm_model_);

//! [connect to DB]
    if (!set_model_data()) {
        return;
    }

//! [Left]
    //! [Date Time From]
    QLabel *lab_from = new QLabel(tr("&From"), this);
    date_from_ = new QDateEdit(this);
    date_from_->setDate(QDate::currentDate());
    date_from_->setDisplayFormat("yyyy-MM-dd");
    date_from_->setCalendarPopup(true);
    time_from_ = new QTimeEdit(this);
    time_from_->setTime(QTime::currentTime());
    time_from_->setDisplayFormat("hh:mm:ss");
    lab_from->setBuddy(date_from_);
    lab_from->setBuddy(time_from_);

    //! [Date Time From]
    QLabel *lab_to = new QLabel(tr("&To"), this);
    date_to_ = new QDateEdit(this);
    date_to_->setDate(QDate::currentDate());
    date_to_->setDisplayFormat("yyyy-MM-dd");
    date_to_->setCalendarPopup(true);
    time_to_ = new QTimeEdit(this);
    time_to_->setTime(QTime::currentTime());
    time_to_->setDisplayFormat("hh:mm:ss");
    lab_to->setBuddy(date_to_);
    lab_to->setBuddy(time_to_);

    //! [Filter]
    QLabel *lab_filter = new QLabel(tr("Filter:"));
    filter_ = new QLineEdit(this);
    lab_filter->setBuddy(filter_);

    //! [Search Button]
    QPushButton *btn_search = new QPushButton(this);
    btn_search->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));

    //! [FilterLayout]
    QHBoxLayout *lay_filter = new QHBoxLayout;
    lay_filter->addWidget(lab_from);
    lay_filter->addWidget(date_from_);
    lay_filter->addWidget(time_from_);
    lay_filter->addWidget(lab_to);
    lay_filter->addWidget(date_to_);
    lay_filter->addWidget(time_to_);
    lay_filter->addStretch();
    lay_filter->addWidget(lab_filter);
    lay_filter->addWidget(filter_);
    lay_filter->addWidget(btn_search);


    //! [TreeView]
    treeview_ = new QTreeView;
    treeview_->setRootIsDecorated(false);
    treeview_->setAlternatingRowColors(true);
    treeview_->setModel(filter_model_);
    treeview_->setSortingEnabled(true);
    treeview_->sortByColumn(1, Qt::AscendingOrder);
    treeview_->setColumnHidden(3, true);

    //! [Left Layout] [Left Group]
    QGroupBox *gbox_left = new QGroupBox(this);
    QVBoxLayout *lay_left = new QVBoxLayout(gbox_left);
    lay_left->setSpacing(0);
    lay_left->setContentsMargins(0, 0, 0, 0);

    lay_left->addLayout(lay_filter);
    lay_left->addWidget(treeview_);

//! [Right]
    //! [right Layout] [Left Group]
    QGroupBox *gbox_right = new QGroupBox(this);
    QSizePolicy sizepolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizepolicy1.setHorizontalStretch(0);
    sizepolicy1.setVerticalStretch(0);
    sizepolicy1.setHeightForWidth(gbox_right->sizePolicy().hasHeightForWidth());
    gbox_right->setSizePolicy(sizepolicy1);

    QVBoxLayout *lay_right = new QVBoxLayout(gbox_right);
    lay_right->setSpacing(0);
    lay_right->setContentsMargins(0, 0, 0, 0);

    //! [Frame]
    QHBoxLayout *lay_frame = new QHBoxLayout;
    lay_frame->setSpacing(0);
    frame_src_ = new QLabel(this);
    frame_dst_ = new QLabel(this);
    Utility::SetLabelStyle(frame_src_);
    Utility::SetLabelStyle(frame_dst_);
    lay_frame->addWidget(frame_src_);
    lay_frame->addWidget(frame_dst_);

    //! [Plot]
    QHBoxLayout *lay_plot = new QHBoxLayout;
    lay_plot->setSpacing(0);
    plot_ = new UPlotWidget(this);
    lay_plot->addWidget(plot_);

    lay_right->addLayout(lay_frame);
    lay_right->addLayout(lay_plot);

    QVBoxLayout *layout = new QVBoxLayout;

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(gbox_left);
    splitter->addWidget(gbox_right);
    splitter->setStretchFactor(0 , 0);
    splitter->setStretchFactor(1, 1);

    layout->addWidget(splitter);

    setLayout(layout);

    //! [setup connection]
    connect(date_from_, SIGNAL(dateChanged(QDate)), this, SLOT(DateTimeFilterChanged()));
    connect(date_to_, SIGNAL(dateChanged(QDate)), this, SLOT(DateTimeFilterChanged()));
    connect(time_from_, SIGNAL(timeChanged(QTime)), this, SLOT(DateTimeFilterChanged()));
    connect(time_to_, SIGNAL(timeChanged(QTime)), this, SLOT(DateTimeFilterChanged()));
    connect(filter_, SIGNAL(textChanged(QString)), this, SLOT(TextFilterChanged()));
    connect(btn_search, SIGNAL(clicked()), this, SLOT(set_model_data()));
    connect(treeview_, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(Analyzer()));
}

AlarmAnalyzer::~AlarmAnalyzer()
{
}

void AlarmAnalyzer::DateTimeFilterChanged()
{
    QDateTime datetime_from = QDateTime(date_from_->date(), time_from_->time());
    QDateTime datetime_to = QDateTime(date_to_->date(), time_to_->time());
    filter_model_->set_datetime_min(datetime_from);
    filter_model_->set_datetime_max(datetime_to);
}

void AlarmAnalyzer::TextFilterChanged()
{
    QRegExp reg_exp(filter_->text(), Qt::CaseSensitive, QRegExp::RegExp2);
    filter_model_->setFilterRegExp(reg_exp);
}

bool AlarmAnalyzer::set_model_data()
{
    CDbi db_;
    if (! Utility::IsDBConnected(db_)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }
    std::vector<std::map<std::string, std::string> > res = db_.Query("SELECT * FROM DensityAlarmRecord ORDER BY date_time DESC LIMIT 1000 OFFSET 0;");

    alarm_model_->set_raw_data(res);

    return true;
}

void AlarmAnalyzer::Analyzer()
{
    plot_->clear_plot();

    QString datetime = treeview_->model()->index(treeview_->currentIndex().row(), 1).data().toString();
    QString task_name = treeview_->model()->index(treeview_->currentIndex().row(), 4).data().toString();

    std::vector<std::map<std::string, std::string> > res;
    if (!SelectFromTable("Tasks", "task_name", task_name.toStdString(), res)) {
        return;
    }
    if (res.empty()) {
        return;
    }

    QString task_path = QString::fromStdString(res[0]["task_path"]);
    QImage *img_src = new QImage;
    QImage *img_dst = new QImage;
    QString img_name = treeview_->model()->index(treeview_->currentIndex().row(), 3).data().toString();
    QString src_path = task_path + QString::fromStdString(kAlarmDir) + img_name + "_src.jpg";
    QString dst_path = task_path + QString::fromStdString(kAlarmDir) + img_name + "_dst.jpg";
//    qDebug() << src_path;
//    qDebug() << dst_path;
    img_src->load(src_path);
    img_dst->load(dst_path);
    frame_src_->setPixmap(QPixmap::fromImage(*img_src));
    frame_dst_->setPixmap(QPixmap::fromImage(*img_dst));

    HistoricalPlot(task_name.toStdString(), datetime.toStdString());
}

bool AlarmAnalyzer::SelectFromTable(const string &table, const string &key, const string &val, std::vector<std::map<string, string> > &res)
{
    // Connect to DB
    CDbi db;
    if (! Utility::IsDBConnected(db)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }

    // Get Info From Table
    res = db.Query("SELECT * FROM " + table + " WHERE " + key + "='" + val + "';");

    return true;
}

void AlarmAnalyzer::HistoricalPlot(std::string task_name, std::string str)
{
    // connect to db
    CDbi db_;
    if (! Utility::IsDBConnected(db_)) {
        QMessageBox::warning(0, "Warning", "Connect DB Failed!", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    std::string datetime_min = QDateTime::fromTime_t(DateTimeStringToDouble(str) - 120).toString("yyyy-MM-dd HH:mm:ss").toStdString();
    std::string datetime_max = QDateTime::fromTime_t(DateTimeStringToDouble(str) + 120).toString("yyyy-MM-dd HH:mm:ss").toStdString();
    std::cout << datetime_min << std::endl;
    std::cout << datetime_max << std::endl;

    /* view video as camera */
    plot_->set_plot(DateTimeStringToDouble(datetime_min), DateTimeStringToDouble(datetime_max), kCameraTypeHttp);

    std::vector<std::map<std::string, std::string> > res = db_.Query("SELECT * FROM DensityPredict WHERE task_name='" + task_name + "' AND date_time >='" + datetime_min + "' AND date_time <'" + datetime_max + "';");
    for (size_t i=0; i<res.size(); ++i) {
        double key = DateTimeStringToDouble(res[i]["date_time"]);
        int value = atoi(res[i]["count"].c_str());
        plot_->set_data(key, value);
    }
}

double AlarmAnalyzer::DateTimeStringToDouble(std::string str)
{
    QDateTime datetime = QDateTime::fromString(QString::fromStdString(str), "yyyy-MM-dd HH:mm:ss");
    return datetime.toMSecsSinceEpoch()/1000.0;
}

