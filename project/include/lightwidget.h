#ifndef LIGHTWIDGET_H
#define LIGHTWIDGET_H

#include <QLabel>

class LightWidget : public QLabel
{
    Q_OBJECT
public:
    explicit LightWidget(const QColor &color, QWidget *parent = 0);
    ~LightWidget();

    bool isOn() const;
    void setOn(bool on);
signals:

public slots:

protected:
    void paintEvent(QPaintEvent *);

private:
    QColor color_;
    bool on_;
};

#endif // LIGHTWIDGET_H
