#include "uclipboard.h"
#include <QMouseEvent>
#include <QLayout>
#include <QLabel>
#include <QGroupBox>
#include "utility.h"

UClipBoard::UClipBoard(QWidget *parent) : QWidget(parent)
{
    clip_max_ = false;

    setContextMenuPolicy(Qt::CustomContextMenu);

    QVBoxLayout *p_layout = new QVBoxLayout(this);

    QGroupBox *p_gbox = new QGroupBox(this);;
    QSizePolicy sizepolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizepolicy1.setHorizontalStretch(0);
    sizepolicy1.setVerticalStretch(0);
    sizepolicy1.setHeightForWidth(p_gbox->sizePolicy().hasHeightForWidth());
    p_gbox->setSizePolicy(sizepolicy1);

    QVBoxLayout *p_lay_main = new QVBoxLayout(p_gbox);
    p_lay_main->setSpacing(0);
    p_lay_main->setContentsMargins(11, 11, 11, 11);
//    p_lay_main->setContentsMargins(0, 0, 0, 0);

    // create 9 channel
    for (int i=0; i<9; i++) {
#ifdef USE_GLWIDGET
        UClipGLWidget *p_clip = new UClipGLWidget(p_gbox);
        Utility::SetWdgSizePolicy(p_clip, QSizePolicy::Ignored, QSizePolicy::Ignored);
#else
        UClipWidget *p_clip = new UClipWidget(p_gbox);
        Utility::SetLabelStyle(p_clip);
#endif
        p_clip->setVisible(false);
        clip_widget_.append(p_clip);
    }

    // create 4 layout
    for (int i=0; i<4; i++) {
        QHBoxLayout *p_lay = new QHBoxLayout;
        p_lay->setSpacing(0);
        p_lay_main->addLayout(p_lay);
        lay_.append(p_lay);
    }

    // installEventFilter for channel
    for (int i=0; i<9; i++) {
        clip_widget_[i]->installEventFilter(this);
        clip_widget_[i]->setProperty("clip", true);
//        clip_widget_[i]->setText(QString("Channel %1").arg(i));
    }

    RemoveLayout();

    ShowChannel9();

    p_layout->addWidget(p_gbox);
}

UClipBoard::~UClipBoard()
{

}

#ifdef USE_GLWIDGET
void UClipBoard::set_channel(int channel, UClipGLWidget *p_clip)
{
    // InstallEventFilter
    p_clip->installEventFilter(this);
    clip_widget_hash_.insert(channel, p_clip);
    RemoveLayout();
    ShowChannel9();
}
#else
void UClipBoard::set_channel(int channel, UClipWidget *p_clip)
{
    // InstallEventFilter
    p_clip->installEventFilter(this);
    clip_widget_hash_.insert(channel, p_clip);
    RemoveLayout();
    ShowChannel9();
}
#endif

void UClipBoard::clear_channel(int channel)
{
    RemoveLayout();
    // RemoveEventFilter
    if (clip_widget_hash_.contains(channel)) {
        clip_widget_hash_.value(channel)->removeEventFilter(this);
        clip_widget_hash_.remove(channel);
    }
    ShowChannel9();
}

bool UClipBoard::eventFilter(QObject *obj, QEvent *event)
{
    QMouseEvent *MouseEvent = static_cast<QMouseEvent *>(event);
    if ((event->type() == QEvent::MouseButtonDblClick) &&
        (MouseEvent->button() == Qt::LeftButton)) {
#ifdef USE_GLWIDGET
        UClipGLWidget *p_clip = qobject_cast<UClipGLWidget *>(obj);
#else
        UClipWidget *p_clip = qobject_cast<UClipWidget *>(obj);
#endif
        if (!clip_max_) {
            RemoveLayout();
            clip_max_ = true;
            lay_[0]->addWidget(p_clip);
            p_clip->setVisible(true);
        } else {
            clip_max_ = false;
            RemoveLayout();
            ShowChannel9();
        }

        p_clip->setFocus();
        return true;
    }

    return QObject::eventFilter(obj, event);
}

void UClipBoard::ShowChannel9()
{
    for (int i=0; i<3; i++) {
        for (int j=3*i; j<(3*i+3); j++) {
            if (clip_widget_hash_.contains(j)) {
                lay_[i]->addWidget(clip_widget_hash_.value(j));
                clip_widget_hash_.value(j)->setVisible(true);
            } else {
                lay_[i]->addWidget(clip_widget_[j]);
                clip_widget_[j]->setVisible(true);
            }
        }
    }
}

void UClipBoard::RemoveLayout()
{
    for (int i=0; i<3; i++) {
        for (int j=3*i; j<3*i + 3; j++) {
            if (clip_widget_hash_.contains(j)) {
                lay_[i]->removeWidget(clip_widget_hash_.value(j));
                clip_widget_hash_.value(j)->setVisible(false);
            }

            lay_[i]->removeWidget(clip_widget_[j]);
            clip_widget_[j]->setVisible(false);
        }
    }
}
