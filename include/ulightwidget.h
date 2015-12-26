#ifndef ULIGHTWIDGET_H
#define ULIGHTWIDGET_H

#include <QLabel>

class ULightWidget : public QLabel
{
    Q_OBJECT
public:
    explicit ULightWidget(const QColor &color, QWidget *parent = 0);
    ~ULightWidget();

    inline bool isOn() const { return on_;}
    void setOn(bool on);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QColor color_;
    bool on_;
};

#endif // ULIGHTWIDGET_H
