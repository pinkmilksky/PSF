#pragma once

#include <QImage>
#include <QString>
#include <cmath>

// класс для упрощения работы с матрицами
class Parser : public QImage
{
private:
    // суммарная энергия и центр
    double m_sumEnergy = 0;
    // значение фона
    double m_background=0;

public:
    //энергия в области (%)
    double m_EnergyCircle = 0, m_EnergySquare;
    // радиус области
    double m_SizeCircle=0, m_SizeSquare=0;
    // координаты центра
    double m_CenterXCircle = 0, m_CenterYCircle = 0;
    double m_CenterXSquare = 0, m_CenterYSquare = 0;
    int absolute_position; // позиция картинки в векторе


public:
    // параметры для красивого отображения таблицы
    QString m_sFileName;

    // методы для создания объектов класса Parser из изображения или матрицы
    Parser(QImage* image);
    Parser(int w, int h);
    Parser();
    Parser(QVector <QVector <int> > matrix);

    void PreProcessing();

    // метод для очистки матрицы от шума
    void cleanMinimum();
    // метод для подсчета суммарной энергии
    void calcSumEnergy();

    inline double sumEnergy() const
    { return m_sumEnergy; }

    // вoзвращает значение БЕЗ фона (Pixel(x,y)-_background)
    inline int Get_Value(int x, int y) const
    {
        return pixelColor(x, y).lightness() - m_background;
    }

    // устанавливает значение пикселя
    inline void setValue(int x, int y, int value)
    {
        setPixel(x, y, qRgb(value, value, value));
    }
    // методы для получения суммарной энергии по квадратной области или круговой
    // берет радиус из m_SizeCircle,m_SizeSquare
    // берет центр из m_CenterXCircle, m_CenterYCircle, m_CenterXSquare, m_CenterYSquare
    // записывает результат в m_EnergyCircle, m_EnergySquare
    void calcSquareEnergy();
    void calcCircleEnergy();

    // поиск центра круга/квадрата в диапазоне m_Center+-0.5*radius
    // берет радиус из m_SizeCircle,m_SizeSquare
    // записывает результат в m_CenterXCircle, m_CenterYCircle, m_CenterXSquare, m_CenterYSquare
    // и в m_EnergyCircle, m_EnergySquare
    void calcSquareCenter ();
    void calcCircleCenter ();

    // поиск радиуса круга/стороны квадрата (перебором)
    // берет энергию из m_EnergyCircle, m_EnergySquare
    // берет центр из m_CenterXCircle, m_CenterYCircle, m_CenterXSquare, m_CenterYSquare
    // записывает результат в m_SizeCircle,m_SizeSquare
    void calcSquareRadius_old ();
    void calcCircleRadius_old ();

    // поиск радиуса круга/стороны квадрата (методом половинного деления)
    // берет энергию из m_EnergyCircle, m_EnergySquare
    // берет центр из m_CenterXCircle, m_CenterYCircle, m_CenterXSquare, m_CenterYSquare
    // записывает результат в m_SizeCircle,m_SizeSquare
    void calcSquareRadius ();
    void calcCircleRadius ();

private:
    double SquareEnergy(double X, double Y, double r);
    double CircleEnergy(double X, double Y, double r);

    // поиск радиуса круга по методу половинного деления
    // cx_p, cy_p - центр
    // energy_p - требуемое кол-во энергии в %
    // bCircle_p - вычислять для круга (true) или квадрата (false)
    double calcRadius(double cx_p, double cy_p, double energy_p, bool bCircle_p);

};
