#include "utaskboard.h"
#include <QLayout>
#include <QLabel>
#include <QGroupBox>
#include "uclipwidget.h"
#include <QMouseEvent>
#include <QDebug>
#include "utility.h"

UTaskBoard::UTaskBoard(QWidget *parent)
    : QWidget(parent)
    , clip_max_(false)
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    QVBoxLayout *layout = new QVBoxLayout;

    QGroupBox *groupbox = new QGroupBox(this);
    groupbox->setObjectName(QStringLiteral("groupbox"));
    QSizePolicy groupboxSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    groupboxSizePolicy.setHorizontalStretch(0);
    groupboxSizePolicy.setVerticalStretch(0);
    groupboxSizePolicy.setHeightForWidth(groupbox->sizePolicy().hasHeightForWidth());
    groupbox->setSizePolicy(groupboxSizePolicy);

    QVBoxLayout *lay_main = new QVBoxLayout(groupbox);
    lay_main->setSpacing(0);
    lay_main->setContentsMargins(11, 11, 11, 11);
    lay_main->setObjectName(QStringLiteral("lay_main"));
    lay_main->setContentsMargins(0, 0, 0, 0);

    // create 16 channel
    for (int i=0; i<16; i++) {
        UClipWidget *clip = new UClipWidget(groupbox);
        clip->setVisible(false);
        Utility::SetLabelStyle(clip);

        clipwidget_.append(clip);
    }

    // create 4 layout
    for (int i=0; i<4; i++) {
        QHBoxLayout *lay = new QHBoxLayout;
        lay->setSpacing(0);
        lay_main->addLayout(lay);
        lay_.append(lay);
    }

    // installEventFilter for channel
    for (int i=0; i<16; i++) {
        clipwidget_[i]->installEventFilter(this);
        clipwidget_[i]->setProperty("clip", true);
        clipwidget_[i]->setText(QString("Channel %1").arg(i));
    }

    RemoveLayout();
    ShowChannel9();

    layout->addWidget(groupbox);

    setLayout(layout);

}

UTaskBoard::~UTaskBoard()
{

}

void UTaskBoard::set_channel(int channel, UClipWidget * clipwidget)
{
    // InstallEventFilter
    clipwidget->installEventFilter(this);
    clipwidget_hash_.insert(channel, clipwidget);
    RemoveLayout();
    ShowChannel9();
}

void UTaskBoard::clear_channel(int channel)
{
    RemoveLayout();

    // RemoveEventFilter
    if (clipwidget_hash_.contains(channel)) {
        clipwidget_hash_.value(channel)->removeEventFilter(this);
        clipwidget_hash_.remove(channel);
    }

    ShowChannel9();
}

bool UTaskBoard::eventFilter(QObject *obj, QEvent *event)
{
    QMouseEvent *MouseEvent = static_cast<QMouseEvent *>(event);
    if ((event->type() == QEvent::MouseButtonDblClick) &&
        (MouseEvent->button() == Qt::LeftButton)) {
        UClipWidget *clipwidget = qobject_cast<UClipWidget *>(obj);
        if (!clip_max_) {
            RemoveLayout();
            clip_max_ = true;
            lay_[0]->addWidget(clipwidget);
            clipwidget->setVisible(true);
        } else {
            clip_max_ = false;
            RemoveLayout();
            ShowChannel9();
        }

        clipwidget->setFocus();
        return true;
    }

    return QObject::eventFilter(obj, event);
}

void UTaskBoard::ShowChannel16()
{
    for (int i=0; i<4; i++) {
        for (int j= (4*i); j<(4*i +4); j++) {
            if (clipwidget_hash_.contains(j)) {
                lay_[i]->addWidget(clipwidget_hash_.value(j));
                clipwidget_hash_.value(j)->setVisible(true);
            } else {
                lay_[i]->addWidget(clipwidget_[j]);
                clipwidget_[j]->setVisible(true);
            }
        }
    }
}

void UTaskBoard::ShowChannel9()
{
    for (int i=0; i<3; i++) {
        for (int j=3*i; j<(3*i+3); j++) {
            if (clipwidget_hash_.contains(j)) {
                lay_[i]->addWidget(clipwidget_hash_.value(j));
                clipwidget_hash_.value(j)->setVisible(true);
            } else {
                lay_[i]->addWidget(clipwidget_[j]);
                clipwidget_[j]->setVisible(true);
            }
        }
    }
}

void UTaskBoard::RemoveLayout()
{
    for (int i=0; i<4; i++) {
        for (int j=4*i; j<4*i + 4; j++) {
            if (clipwidget_hash_.contains(j)) {
                lay_[i]->removeWidget(clipwidget_hash_.value(j));
                clipwidget_hash_.value(j)->setVisible(false);
            }

            lay_[i]->removeWidget(clipwidget_[j]);
            clipwidget_[j]->setVisible(false);
        }
    }
}
