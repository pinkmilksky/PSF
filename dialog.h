#pragma once

#include <QDialog>
#include <vector>
#include "ui_dialog.h"
#include "parser.h"
#include "core.h"
#include <QGraphicsScale>
#include "enums.h"

namespace Ui {
class Dialog;
}

class Core;

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

    double getEnergy();
    double getRadius();

    QPair<double, double> getAreaCenter();
    CalcType getCalcType();
    AreaCenterType getCenterType();
    bool isCircle();

public slots:
    void onOpenImage();
    void onClear();
    void onSelectRow();
    void onZoomIn();
    void onZoomOut();
    void onCenterImage();
    void onZoomDefault();
    void onCalculateSelected();

    void onCalculateAvarage();
private slots:
    void onCheckEnabled();
    void displayMenu(const QPoint &pos);

private:
    Ui::Dialog *ui;
    Core* core;

    qreal scalingFactor = 1.0;
    int selectedImage;
    std::vector<Parser> m_Images;

    void DrawImage(int i);
    void UpdateTable();

};

