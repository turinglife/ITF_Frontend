#include "ucameramanager.h"
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QTableView>
#include <QLineEdit>

UCameraManager::UCameraManager(QWidget *parent) : QWidget(parent)
{
//! [layout for widget_title and widget_main]
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setObjectName(QStringLiteral("layout"));
    layout->setContentsMargins(1, 1, 1, 1);

//! [QSizePolicy]
    // widget_title
    QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);

    // lab_icon, widget_menu, gbox_right
    QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);

    // lab_title
    QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);

    // btn_menu_close
    QSizePolicy sizePolicy4(QSizePolicy::Minimum, QSizePolicy::Expanding);
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);

    // others
    QSizePolicy sizePolicy5(QSizePolicy::Minimum, QSizePolicy::Fixed);
    sizePolicy5.setHorizontalStretch(0);
    sizePolicy5.setVerticalStretch(0);

//!
//! [widget tiltle]
//!
    QWidget *widget_title = new QWidget(this);
    widget_title->setObjectName(QStringLiteral("widget_title"));
    sizePolicy1.setHeightForWidth(widget_title->sizePolicy().hasHeightForWidth());
    widget_title->setSizePolicy(sizePolicy1);
    widget_title->setMinimumSize(QSize(100, 28));
    QHBoxLayout *lay_widget_title = new QHBoxLayout(widget_title);
    lay_widget_title->setSpacing(0);
    lay_widget_title->setObjectName(QStringLiteral("lay_widget_title"));
    lay_widget_title->setContentsMargins(0, 0, 0, 0);
    // label icon
    lab_icon_ = new QLabel(widget_title);
    lab_icon_->setObjectName(QStringLiteral("lab_icon"));
    sizePolicy2.setHeightForWidth(lab_icon_->sizePolicy().hasHeightForWidth());
    lab_icon_->setSizePolicy(sizePolicy2);
    lab_icon_->setMinimumSize(QSize(31, 0));
    lab_icon_->setAlignment(Qt::AlignCenter);

    lay_widget_title->addWidget(lab_icon_);

    // label title
    lab_title_ = new QLabel(widget_title);
    lab_title_->setObjectName(QStringLiteral("lab_title"));
    sizePolicy3.setHeightForWidth(lab_title_->sizePolicy().hasHeightForWidth());
    lab_title_->setSizePolicy(sizePolicy3);
    lab_title_->setStyleSheet(QString::fromUtf8("font: 10pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));
    lab_title_->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

    lay_widget_title->addWidget(lab_title_);

    // widget menu
    QWidget *widget_menu = new QWidget(widget_title);
    widget_menu->setObjectName(QStringLiteral("widget_menu"));
    sizePolicy2.setHeightForWidth(widget_menu->sizePolicy().hasHeightForWidth());
    widget_menu->setSizePolicy(sizePolicy2);
    QHBoxLayout *lay_widget_menu = new QHBoxLayout(widget_menu);
    lay_widget_menu->setSpacing(0);
    lay_widget_menu->setObjectName(QStringLiteral("lay_widget_menu"));
    lay_widget_menu->setContentsMargins(0, 0, 0, 0);
    //menu btn
    btn_menu_close_ = new QPushButton(widget_menu);
    btn_menu_close_->setObjectName(QStringLiteral("btn_menu_close"));
    sizePolicy4.setHeightForWidth(btn_menu_close_->sizePolicy().hasHeightForWidth());
    btn_menu_close_->setSizePolicy(sizePolicy4);
    btn_menu_close_->setMinimumSize(QSize(40, 0));
    btn_menu_close_->setCursor(QCursor(Qt::ArrowCursor));
    btn_menu_close_->setFocusPolicy(Qt::NoFocus);
    btn_menu_close_->setFlat(true);

    lay_widget_menu->addWidget(btn_menu_close_);

    lay_widget_title->addWidget(widget_menu);

//!
//!
//! [widget main]
//!
    QWidget *widget_main = new QWidget(this);
    widget_main->setObjectName(QStringLiteral("widget_main"));
    widget_main->setStyleSheet(QString::fromUtf8("font: 11pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));
    QHBoxLayout *lay_widget_main = new QHBoxLayout(widget_main);
    lay_widget_main->setSpacing(5);
    lay_widget_main->setObjectName(QStringLiteral("horizontalLayout_3"));
    lay_widget_main->setContentsMargins(5, 5, 5, 5);

    //! [main]
    tableview_ = new QTableView(widget_main);
    tableview_->setObjectName(QStringLiteral("tableview_"));

    lay_widget_main->addWidget(tableview_);

    QGroupBox *gbox_right = new QGroupBox(this);
    gbox_right->setObjectName(QStringLiteral("gbox_right"));
    sizePolicy2.setHeightForWidth(gbox_right->sizePolicy().hasHeightForWidth());
    gbox_right->setSizePolicy(sizePolicy2);
    gbox_right->setMinimumSize(QSize(220, 0));
    QVBoxLayout *lay_gbox_right = new QVBoxLayout(gbox_right);
    lay_gbox_right->setObjectName(QStringLiteral("lay_gbox_right"));

    QLabel *lab_cameraname = new QLabel(tr("Camera Name"), gbox_right);
    lab_cameraname->setObjectName(QStringLiteral("lab_cameraname"));

    lay_gbox_right->addWidget(lab_cameraname);

    cameraname_ = new QLineEdit(gbox_right);
    cameraname_->setObjectName(QStringLiteral("cameraname_"));
    sizePolicy5.setHeightForWidth(cameraname_->sizePolicy().hasHeightForWidth());
    cameraname_->setSizePolicy(sizePolicy5);

    lay_gbox_right->addWidget(cameraname_);

    QLabel *lab_address = new QLabel(tr("Address"), gbox_right);
    lab_address->setObjectName(QStringLiteral("lab_address"));

    lay_gbox_right->addWidget(lab_address);

    address_ = new QLineEdit(gbox_right);
    address_->setObjectName(QStringLiteral("lab_address"));
    sizePolicy5.setHeightForWidth(address_->sizePolicy().hasHeightForWidth());
    address_->setSizePolicy(sizePolicy5);

    lay_gbox_right->addWidget(address_);

    QLabel *lab_host = new QLabel(tr("Host"), gbox_right);
    lab_host->setObjectName(QStringLiteral("lab_host"));

    lay_gbox_right->addWidget(lab_host);

    host_ = new QLineEdit(gbox_right);
    host_->setObjectName(QStringLiteral("host_"));
    sizePolicy5.setHeightForWidth(host_->sizePolicy().hasHeightForWidth());
    host_->setSizePolicy(sizePolicy5);

    lay_gbox_right->addWidget(host_);

    QLabel *lab_port = new QLabel(tr("Port"), gbox_right);
    lab_port->setObjectName(QStringLiteral("lab_port"));

    lay_gbox_right->addWidget(lab_port);

    port_ = new QLineEdit(gbox_right);
    port_->setObjectName(QStringLiteral("port_"));
    sizePolicy5.setHeightForWidth(port_->sizePolicy().hasHeightForWidth());
    port_->setSizePolicy(sizePolicy5);

    lay_gbox_right->addWidget(port_);

    QLabel *lab_username = new QLabel(tr("Username"), gbox_right);
    lab_username->setObjectName(QStringLiteral("lab_username"));

    lay_gbox_right->addWidget(lab_username);

    username_ = new QLineEdit(gbox_right);
    username_->setObjectName(QStringLiteral("username_"));
    sizePolicy5.setHeightForWidth(username_->sizePolicy().hasHeightForWidth());
    username_->setSizePolicy(sizePolicy5);

    lay_gbox_right->addWidget(username_);

    QLabel *lab_password = new QLabel(tr("Password"), gbox_right);
    lab_password->setObjectName(QStringLiteral("lab_password"));

    lay_gbox_right->addWidget(lab_password);

    password_ = new QLineEdit(gbox_right);
    password_->setObjectName(QStringLiteral("password_"));
    sizePolicy5.setHeightForWidth(password_->sizePolicy().hasHeightForWidth());
    password_->setSizePolicy(sizePolicy5);

    lay_gbox_right->addWidget(password_);


    btn_select_ = new QPushButton(tr("Select Camera"), gbox_right);
    btn_select_->setObjectName(QStringLiteral("btn_select_"));
    btn_select_->setCursor(QCursor(Qt::PointingHandCursor));
//    QIcon icon1;
//    icon1.addFile(QStringLiteral(":/image/add.png"), QSize(), QIcon::Normal, QIcon::Off);
//    btn_select_->setIcon(icon1);
//    btn_select_->setIconSize(QSize(20, 20));

    lay_gbox_right->addWidget(btn_select_);

    btn_add_ = new QPushButton(tr("Add Camera"), gbox_right);
    btn_add_->setObjectName(QStringLiteral("btn_add"));
    btn_add_->setCursor(QCursor(Qt::PointingHandCursor));
//    QIcon icon2;
//    icon2.addFile(QStringLiteral(":/image/add.png"), QSize(), QIcon::Normal, QIcon::Off);
//    btn_add->setIcon(icon2);
//    btn_add->setIconSize(QSize(20, 20));

    lay_gbox_right->addWidget(btn_add_);


    btn_delete_ = new QPushButton(tr("Delete Camera"), gbox_right);
    btn_delete_->setObjectName(QStringLiteral("btn_delete"));
    btn_delete_->setCursor(QCursor(Qt::PointingHandCursor));
//    QIcon icon3;
//    icon3.addFile(QStringLiteral(":/image/delete.png"), QSize(), QIcon::Normal, QIcon::Off);
//    btn_delete->setIcon(icon3);
//    btn_delete->setIconSize(QSize(20, 20));

    lay_gbox_right->addWidget(btn_delete_);

    // end here
    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    lay_gbox_right->addItem(verticalSpacer);

    lay_widget_main->addWidget(gbox_right);

//!
//! [layout]
//!
    layout->addWidget(widget_title);
    layout->addWidget(widget_main);

    setLayout(layout);
}

UCameraManager::~UCameraManager()
{

}

void UCameraManager::Clear()
{
    cameraname_->clear();
    address_->clear();
    host_->clear();
    port_->clear();
    username_->clear();
    password_->clear();
}

