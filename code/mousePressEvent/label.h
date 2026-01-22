#ifndef LABEL_H
#define LABEL_H

#include <QWidget>
#include <QLabel>

class Label : public QLabel
{
    Q_OBJECT
public:
    Label(QWidget* parent);

    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void mouseDoubleClickEvent(QMouseEvent *ev);
};

#endif // LABEL_H
