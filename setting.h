#ifndef SETTING_H
#define SETTING_H

#include <QWidget>

class Setting : public QWidget
{
    Q_OBJECT
public:
    explicit Setting(QWidget *parent = nullptr);
    ~Setting();

protected:
    void keyPressEvent(QKeyEvent *event) override;

public slots:
};

#endif // SETTING_H
