#include "parser.h"

#include <QTextStream>
#include <QDebug>
#include <QFile>

//-------------------------------------------------------------------------------------------------
Parser::Parser()
    : QImage ()
{}
//-------------------------------------------------------------------------------------------------
void Parser::PreProcessing()
{
    cleanMinimum();

    int h=size().height();
    int w=size().width();
    double massCenterX = 0, massCenterY = 0;
    for (int x = 0; x <w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            massCenterX += (x + 1) * Get_Value(x, y);
            massCenterY += (y + 1) * Get_Value(x, y);
        }
    }

    calcSumEnergy();

    m_CenterXCircle = m_CenterXSquare = massCenterX / m_sumEnergy;
    m_CenterYCircle = m_CenterYSquare = massCenterY / m_sumEnergy;

    qDebug() <<"Center mass X,Y:"<<m_CenterXCircle<<", "<<m_CenterYCircle;
}
//-------------------------------------------------------------------------------------------------
Parser::Parser(int w, int h)
    : QImage(w,h, QImage::Format_RGB32)
{
}
//-------------------------------------------------------------------------------------------------
// создание из матрицы
Parser::Parser(QVector<QVector<int>> matrix)
    : QImage(matrix[0].size(), matrix.size(), QImage::Format_RGB32)
{
    for (int i = 0; i < matrix.size(); ++i)
        for (int j = 0; j < matrix[0].size(); ++j)
            setValue(j, i, matrix[i][j]);
    PreProcessing();
}
//-------------------------------------------------------------------------------------------------
//чтение изображения
Parser::Parser(QImage* image)
    : QImage(*image)
{
    PreProcessing();
}
//-------------------------------------------------------------------------------------------------
void Parser::cleanMinimum()
{
    m_background = 256;

    for (int x = 0; x < width(); x++)
    {
        for (int y = 0; y < height(); y++)
        {
            int value = pixelColor(x, y).lightness();
            m_background = m_background > value ? value : m_background;
        }
    }

    qDebug() << "Minimum power of image:" << m_background;
}
//-------------------------------------------------------------------------------------------------
void Parser::calcSumEnergy()
{
    long sumEnergy = 0;
    for (int x = 0; x < size().width(); x++)
    {
        for (int y = 0; y < size().height(); y++)
        {
            sumEnergy += Get_Value(x, y);
        }
    }

    m_sumEnergy = sumEnergy;
}
//-------------------------------------------------------------------------------------------------
double Parser::SquareEnergy(double c_x, double c_y, double r)
{
    int h=size().height();
    int w=size().width();
    double oblEnergy = 0;
    for (int x = c_x - r ; x <= c_x + r; x++)
    {
        for (int y = c_y - r ; y <= c_y + r; y++)
        {
            if (x > 0 && x < w && y > 0 && y < h)
            {
                oblEnergy += Get_Value(x, y);
            }
        }
    }

    return oblEnergy / m_sumEnergy * 100;
}
//-------------------------------------------------------------------------------------------------
double Parser::CircleEnergy(double c_x, double c_y, double r)
{
    int h=size().height();
    int w=size().width();
    double radiusSquare = r * r;

    double circleEnergy = 0;
    for (int x = c_x - r; x <= c_x + r; x++)
    {
        for (int y = c_y - r; y <= c_y + r; y++)
        {
            if (x > 0 && x < w &&
                y > 0 && y < h &&
                (x - c_x) * (x - c_x) + (y - c_y) * (y - c_y) <= radiusSquare )
            {
                circleEnergy += Get_Value(x, y);
            }
        }
    }
    return circleEnergy / m_sumEnergy * 100;
}
//-------------------------------------------------------------------------------------------------
// методы для получения суммарной энергии по квадратной области
// берет радиус из m_SizeSquare
// берет центр из m_CenterXSquare, m_CenterYSquare
// записывает результат в m_EnergySquare
void Parser::calcSquareEnergy()
{
    m_EnergySquare=SquareEnergy(m_CenterXSquare, m_CenterYSquare, m_SizeSquare);
}
//-------------------------------------------------------------------------------------------------
// методы для получения суммарной энергии по круглой области
// берет радиус из m_SizeCircle
// берет центр из m_CenterXCircle, m_CenterYCircle
// записывает результат в m_EnergyCircle
void Parser::calcCircleEnergy()
{
    m_EnergyCircle=CircleEnergy(m_CenterXCircle, m_CenterYCircle, m_SizeCircle);
}
//-------------------------------------------------------------------------------------------------
// поиск центра квадрата в диапазоне m_Center+-0.5*radius
// берет радиус из m_SizeSquare
// записывает результат в m_CenterXSquare, m_CenterYSquare
// и в m_EnergySquare
void Parser::calcSquareCenter()
{
    double oblEnergy = 0, oblEnergyCurrent = 0;
    double xSquareCenter = m_CenterXSquare , ySquareCenter = m_CenterYSquare;
    for (int x = m_CenterXSquare - m_SizeSquare/2; x <= m_CenterXSquare + m_SizeSquare/2; x++)
    {
        for (int y = m_CenterYSquare - m_SizeSquare/2; y <= m_CenterYSquare + m_SizeSquare/2; y++)
        {
            oblEnergyCurrent=SquareEnergy(x, y, m_SizeSquare);
            if (oblEnergy < oblEnergyCurrent)
            {
                oblEnergy = oblEnergyCurrent;
                xSquareCenter = x;
                ySquareCenter = y;
            }
            oblEnergyCurrent = 0;
        }
    }
    m_CenterXSquare=xSquareCenter;
    m_CenterYSquare=ySquareCenter;
    m_EnergySquare=oblEnergy ;
}
//-------------------------------------------------------------------------------------------------
// поиск центра круга в диапазоне m_Center+-0.5*radius
// берет радиус из m_SizeCircle
// записывает результат в m_CenterXCircle, m_CenterYCircle
// и в m_EnergyCircle
void Parser::calcCircleCenter()
{
    double oblEnergy = 0, oblEnergyCurrent = 0;
    double xCircleCenter = m_CenterXCircle , yCircleCenter = m_CenterYCircle;
    for (int x = m_CenterXCircle - m_SizeCircle/2; x <= m_CenterXCircle + m_SizeCircle/2; x++)
    {
        for (int y = m_CenterYCircle - m_SizeCircle/2; y <= m_CenterYCircle + m_SizeCircle/2; y++)
        {
            oblEnergyCurrent=CircleEnergy(x, y, m_SizeCircle);
            if (oblEnergy < oblEnergyCurrent)
            {
                oblEnergy = oblEnergyCurrent;
                xCircleCenter = x;
                yCircleCenter = y;
            }
            oblEnergyCurrent = 0;
        }
    }
    m_CenterXCircle=xCircleCenter;
    m_CenterYCircle=yCircleCenter;
    m_EnergyCircle=oblEnergy ;
}
//-------------------------------------------------------------------------------------------------
// поиск половины стороны квадрата перебором
// берет энергию из m_EnergySquare
// берет центр из m_CenterXSquare, m_CenterYSquare
// записывает результат в m_SizeSquare
void Parser::calcSquareRadius_old ()
{
    double oblEnergy;
    double lSquareEnergy = 0;
    double prevEnergy = 0;
    double sumEnergy = m_EnergySquare*m_sumEnergy / 100;
    double radius = 1;
    double radiusSquare = 0;
    bool condition;
    do
    {
        oblEnergy=SquareEnergy(m_CenterXSquare, m_CenterYSquare, radius);
        if (abs(sumEnergy - oblEnergy) <= abs(sumEnergy - lSquareEnergy))
        {
            lSquareEnergy = oblEnergy;
            radiusSquare = radius;
        }
        condition = (oblEnergy > prevEnergy);
        prevEnergy = oblEnergy;
        radius++;
    } while (condition);

    qDebug() << "squareEnergy:" << lSquareEnergy;
    m_SizeSquare=radiusSquare;
    m_EnergySquare=oblEnergy;
}
//-------------------------------------------------------------------------------------------------
// поиск радиуса круга перебором
// берет энергию из m_EnergyCircle
// берет центр из m_CenterXCircle, m_CenterYCircle
// записывает результат в m_SizeCircle
void Parser::calcCircleRadius_old ()
{
    double oblEnergy;
    double lCircleEnergy = 0;
    double prevEnergy = 0;
    double sumEnergy = m_EnergyCircle*m_sumEnergy / 100;
    double radius = 1;
    double radiusCircle = 0;
    bool condition;
    do
    {
        oblEnergy=CircleEnergy(m_CenterXCircle, m_CenterYCircle, radius);
        if (abs(sumEnergy - oblEnergy) <= abs(sumEnergy - lCircleEnergy))
        {
            lCircleEnergy = oblEnergy;
            radiusCircle = radius;
        }
        condition = (oblEnergy > prevEnergy);
        prevEnergy = oblEnergy;
        radius++;
    } while (condition);
    qDebug() << "circleEnergy:" << lCircleEnergy;
    m_SizeCircle=radiusCircle;
    m_EnergyCircle=oblEnergy;
}
//-------------------------------------------------------------------------------------------------
// поиск радиуса круга по методу половинного деления
// берет энергию из m_EnergyCircle
// берет центр из m_CenterXCircle, m_CenterYCircle
// записывает результат в m_SizeCircle
void Parser::calcCircleRadius()
{
    m_SizeCircle=calcRadius(m_CenterXCircle, m_CenterYCircle, m_EnergyCircle, true);
    m_EnergyCircle=CircleEnergy(m_CenterXCircle, m_CenterYCircle, m_SizeCircle);
}
//-------------------------------------------------------------------------------------------------
// поиск половины стороны квадрата по методу половинного деления
// берет энергию из m_EnergySquare
// берет центр из m_CenterXSquare, m_CenterYSquare
// записывает результат в m_SizeSquare
void Parser::calcSquareRadius()
{
    m_SizeSquare=calcRadius(m_CenterXSquare, m_CenterYSquare, m_EnergySquare, false);
    m_EnergySquare=SquareEnergy(m_CenterXSquare, m_CenterYSquare, m_SizeSquare);
}
//-------------------------------------------------------------------------------------------------
// поиск радиуса круга по методу половинного деления
// cx_p, cy_p - центр
// energy_p - требуемое кол-во энергии в %
// bCircle_p - вычислять для круга (true) или квадрата (false)
double Parser::calcRadius(double cx_p, double cy_p, double energy_p, bool bCircle_p)
{
    // три точки - начало отрезка (a), конец отрезка (b) и середина (x)
    int a = 1;
    int b = std::min(size().height(), size().width());
    int x = (a + b) / 2;
    double energy_x, energy_a, energy_b;
    // точность вычислений
    double eps=0.00001;

    do
    {
        x = (a + b) / 2;
        // минимизируемая функция во всех 3х точках
        if(bCircle_p)
        {
            energy_x = energy_p - CircleEnergy(cx_p, cy_p, x);
            energy_a = energy_p - CircleEnergy(cx_p, cy_p, a);
            energy_b = energy_p - CircleEnergy(cx_p, cy_p, b);
        }
        else
        {
            energy_x = energy_p - SquareEnergy(cx_p, cy_p, x);
            energy_a = energy_p - SquareEnergy(cx_p, cy_p, a);
            energy_b = energy_p - SquareEnergy(cx_p, cy_p, b);
        }
        // проверка, чтобы радиус удовлетворяющий условию был минимальным
        if (energy_b*energy_x <= 0 && energy_x != energy_b)
        {
            a = x;
        }
        else
        {
            b = x;
        }
    } while ((fabs(energy_x) > eps || energy_x == energy_b) && (b - a) >= 2);

    // выбираем лучшее значение из трех
    if(fabs(energy_x) < fabs(energy_a) && fabs(energy_x) < fabs(energy_b))
    {
        return x;
    }
    else if(fabs(energy_a) < fabs(energy_x) && fabs(energy_a) < fabs(energy_b))
    {
        return a;
    }
    else
    {
        return b;
    }
}
