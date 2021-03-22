#pragma once

#include <vector>
#include <QPair>
#include "parser.h"
#include "overlay.h"
#include "dialog.h"
#include "enums.h"

class Dialog;
class Core
{
public:
    Core(Dialog* ui);

    void setUi(Dialog* ui);
    //метод для загрузки файлов с изображениями
    void loadImages(QStringList file_names);
    //метод для вычисления выделенного
    void calculate(QModelIndexList selection, CalcType calc_type);
    //метод дял вычисления всех изображений
    void calculate(CalcType calc_type);

    void calculateOverlay(QModelIndexList selectedNums);

    Parser* getImage(int num);

    size_t getImageCount();

    void deleteImage(int num);

    //очистить вектор images
    void deleteAll();


private:

    std::vector<Parser*> images;
    Dialog* ui;

    void calculateOneImage(size_t num, CalcType calc_type);
    //рассчитать кол-во энергии по размеру области
    void calcEnergyByRegion(Parser* parser, AreaCenterType center_type);
    //рассчитать размер области по кол-ву энергии
    void calcRegionByEnergy(Parser* parser);
    //номер изображения
    int id_counter = 0;
};
