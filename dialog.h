#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <vector>
#include "ui_dialog.h"
#include "parser.h"
#include <QGraphicsScale>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

public slots:
    void onOpenImage();
    void onClear();
    void onSelectRow();
    void onZoomIn();
    void onZoomOut();
    void onCenterImage();
    void onZoomDefault();
    void onCalculateSelected();

private slots:
    void onCheckEnabled();
    void displayMenu(const QPoint &pos);

private:
    Ui::Dialog *ui;

    qreal scalingFactor = 1.0;
    int selectedImage;
    std::vector<Parser> m_Images;

    void DrawImage(int i);
    void UpdateTable();
    bool isCircle();
};

#endif // DIALOG_H
