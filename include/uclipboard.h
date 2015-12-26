#ifndef UCLIPBOARD_H
#define UCLIPBOARD_H


#include <QWidget>
#include "common.h"

#ifdef USE_GLWIDGET
#include "uclipglwidget.h"
#else
#include "uclipwidget.h"
#endif

class QLayout;

class UClipBoard : public QWidget
{
    Q_OBJECT
public:
    explicit UClipBoard(QWidget *parent = 0);
    ~UClipBoard();


#ifdef USE_GLWIDGET
    void set_channel(int channel, UClipGLWidget *p_clip);
#else
    void set_channel(int channel, UClipWidget *p_clip);
#endif
    void clear_channel(int channel);
signals:

public slots:
protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void ShowChannel9();

private:
    void RemoveLayout();

    bool clip_max_;

    QList<QLayout *> lay_;

#ifdef USE_GLWIDGET
    QList<UClipGLWidget *> clip_widget_;
    QHash<int, UClipGLWidget *> clip_widget_hash_;
#else
    QList<UClipWidget *> clip_widget_;
    QHash<int, UClipWidget *> clip_widget_hash_;
#endif


};

#endif // UCLIPBOARD_H
