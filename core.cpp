#include "core.h"

using namespace std;

Core::Core(Dialog* ui)
{
    this->ui = ui;
}

void Core::setUi(Dialog *ui)
{
    this->ui = ui;
}
//реализация метода загрузки файлов с изображениями
void Core::loadImages(QStringList file_names)
{
    size_t old_size = images.size();
    images.resize(old_size + size_t(file_names.size()));

    for (size_t i = old_size; i < images.size(); i++)
    {
        images[i] = new Parser();
        images[i]->load(file_names[i - old_size]);
        images[i]->m_sFileName=file_names[i - old_size];
        images[i]->absolute_position = ++id_counter;
    }
}

void Core::calculate(QModelIndexList selection, CalcType calc_type)
{
    for (auto it = selection.begin(); it != selection.end(); it++)
    {
        size_t num = it->row();
        if (num > images.size())
        {
            throw overflow_error("num is too big");
        }
        calculateOneImage(num, calc_type);
    }
}

void Core::calculate(CalcType calc_type)
{
    for (int i = 0; i < images.size(); i++) {
        calculateOneImage(i, calc_type);
    }
}

void Core::calculateOverlay(QModelIndexList selectedNums)
{
    QSet<Parser*> parsers = QSet<Parser*>();
    if (selectedNums.size() == 0) return;
    for (int i = 0; i < selectedNums.size(); i++) {
        parsers.insert(images[selectedNums.at(i).row()]);
    }

    Overlay overlay = Overlay();
    Parser* simple = overlay.makeSimple(parsers);
    Parser* centered = overlay.makeCentered(parsers);
    simple->m_SizeCircle = ui->getRadius();
    centered->m_SizeCircle = ui->getRadius();

    qDebug() << simple->m_sFileName;
    qDebug() << centered->m_sFileName;

    simple->absolute_position = ++id_counter;
    images.push_back(simple);
    centered->absolute_position = ++id_counter;
    images.push_back(centered);
}

size_t Core::getImageCount()
{
    return images.size();
}

void Core::deleteImage(int num)
{
    images.erase(images.begin() + num);
}

void Core::deleteAll()
{
    images.resize(0);
}

void Core::calculateOneImage(size_t num, CalcType calc_type)
{
    Parser* parser = images[num];
    parser->PreProcessing();

    if (ui->getCenterType() == MANUAL)
    {
        QPair<double, double> coordinates = ui->getAreaCenter();
        parser->m_CenterXCircle = parser->m_CenterXSquare = coordinates.first; // TODO: remove duplicating variables in Parser
        parser->m_CenterYCircle = parser->m_CenterYSquare = coordinates.second;
    }

    switch (calc_type)
    {
        case ENERGY_BY_REGION:
            calcEnergyByRegion(parser, ui->getCenterType());
            break;
        case REGION_BY_ENERGY:
            calcRegionByEnergy(parser);
            break;
    }
}

Parser *Core::getImage(int num)
{
    if (images.size() < size_t(num) + 1) {
        throw std::overflow_error("num is too big");
    }
    qDebug() << images[num]->m_sFileName;
    return images[size_t(num)];
}

void Core::calcEnergyByRegion(Parser *parser, AreaCenterType center_type)
{
    parser->m_SizeCircle = ui->getRadius();
    parser->m_SizeSquare = ui->getRadius();
    if (center_type != AUTO_BY_ENERGY_MAX)
    {
        parser->calcCircleEnergy();
        parser->calcSquareEnergy();
    }
    else
    {
        parser->calcCircleCenter();
        parser->calcSquareCenter();
    }
}

void Core::calcRegionByEnergy(Parser *parser)
{
    parser->m_EnergyCircle = ui->getEnergy();
    parser->m_EnergySquare = ui->getEnergy();
    parser->calcCircleRadius();
    parser->calcSquareRadius();
}
