#include "dialog.h"
#include "ui_dialog.h"
#include <QDebug>
#include <QFileDialog>
#include <QDoubleValidator>
#include <QImage>
#include <QMessageBox>
#include <QMenu>

#include <QApplication>
#include <QSet>
#include "overlay.h"

//-------------------------------------------------------------------------------------------------
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog) 
{
    ui->setupUi(this);

    ui->m_TW_Res->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->m_TW_Res->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->m_TW_Res, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(displayMenu(QPoint)));

    // connect([кто посылает], SIGNAL([сигнал]), [кто обрабатывает], SLOT([слот]));
    connect(ui->m_PB_Open, SIGNAL(clicked()), this, SLOT(onOpenImage()));    
    connect(ui->m_Clear, SIGNAL(clicked()), this, SLOT(onClear()));
    connect(ui->m_TW_Res, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectRow()));
    connect(ui->m_RB_CalcEnergy, SIGNAL(clicked()), this, SLOT(onCheckEnabled()));
    connect(ui->m_RB_CalcRadius, SIGNAL(clicked()), this, SLOT(onCheckEnabled()));
    connect(ui->m_RB_CenterEnergy, SIGNAL(clicked()), this, SLOT(onCheckEnabled()));
    connect(ui->m_RB_CenterMass, SIGNAL(clicked()), this, SLOT(onCheckEnabled()));
    connect(ui->m_RB_CenterSet, SIGNAL(clicked()), this, SLOT(onCheckEnabled()));
    connect(ui->m_PB_Zoom_In, SIGNAL(released()), this, SLOT(onZoomIn()));
    connect(ui->m_PB_Zoom_Out, SIGNAL(released()), this, SLOT(onZoomOut()));
    connect(ui->m_PB_Zoom_Default, SIGNAL(released()), this, SLOT(onZoomDefault()));
    connect(ui->m_PB_Center, SIGNAL(released()), this, SLOT(onCenterImage()));
    connect(ui->m_PB_Calc_Avarage, SIGNAL(released()), this, SLOT(onCalculateSelected()));
    connect(ui->m_PB_Calc_Energy, SIGNAL(released()), this, SLOT(onCalculateAvarage()));

    // одинаковые размеры у Сплиттера
    ui->m_Splitter->setStretchFactor(0,0);
    ui->m_Splitter->setStretchFactor(1,1);

    // размер таблицы - по содержимому колонок
    ui->m_TW_Res->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    // выбираем только строки
    ui->m_TW_Res->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->m_TW_Res->setSelectionMode(QAbstractItemView::MultiSelection);

    // только числа
    ui->m_LE_AreaSize->setValidator(new QDoubleValidator(ui->m_LE_AreaSize));
    ui->m_LE_EnergySize->setValidator(new QDoubleValidator(ui->m_LE_EnergySize));
    ui->m_LE_Center_X->setValidator(new QDoubleValidator(ui->m_LE_Center_X));
    ui->m_LE_Center_Y->setValidator(new QDoubleValidator(ui->m_LE_Center_Y));
    // сразу значение по умолчанию
    ui->m_LE_AreaSize->setText("100");
    ui->m_LE_EnergySize->setText("90");

    // вычисление радиуса пока не работает
//    ui->m_RB_CalcRadius->setEnabled(false);

    // проверяем и устанавливаем недоступные поля
    onCheckEnabled();
    core = new Core(this);
}
//-------------------------------------------------------------------------------------------------
Dialog::~Dialog()
{
    delete ui;
    delete core;
}
//-------------------------------------------------------------------------------------------------
void Dialog::onClear()
{
    ui->m_TW_Res->setRowCount(0);
    core->deleteAll();

}
//-------------------------------------------------------------------------------------------------
void Dialog::DrawImage(int i)
{
    Parser* image = core->getImage(size_t(i));

    // новая сцена и на нее картинка
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->addPixmap(QPixmap::fromImage(*image));
    // рисуем эллипс и прямоугольник
    // x y w h
    double x=image->m_CenterXCircle,
            y=image->m_CenterYCircle,
            r=image->m_SizeCircle;
    if (isCircle()) {
        scene->addEllipse(x-r, y-r, r*2, r*2, QPen(Qt::red));
    } else {
        scene->addRect(x-r, y-r, r*2, r*2, QPen(Qt::red));
    }
    // задаем размер сцены, положение линеее прокрутки, устанавливаем сцену на элемент управления
    ui->m_GV_Image->setSceneRect(0,0,image->width(),image->height());
    ui->m_GV_Image->centerOn(x,y);
    ui->m_GV_Image->setScene(scene);
    selectedImage = i;
}

void Dialog::UpdateTable()
{
    // после вычислений записываем результат в таблицу
    ui->m_TW_Res->setSortingEnabled(false);
    ui->m_TW_Res->setRowCount(0);
    for(size_t iRow=0; iRow<core->getImageCount(); ++iRow)
    {
        // вставляем новую строку
        ui->m_TW_Res->insertRow(int(iRow));

        // создаем новую ячейку (Item) таблицы для имени файла
        QTableWidgetItem* qItem = new QTableWidgetItem();
        // устнавливаем параметры: ячейка доступна + ее можно выбрать (+ ее можно редактировать | Qt::ItemIsEditable
        qItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        // имя файла
        Parser* image = core->getImage(iRow);

        qItem->setData(0, image->absolute_position);
        ui->m_TW_Res->setItem(int(iRow), 0, qItem);
        QTableWidgetItem* qItem2 = new QTableWidgetItem();
        qItem2->setData(0, image->m_sFileName);
        // устанавливаем созданную ячейку в нужное место в таблицы
        ui->m_TW_Res->setItem(int(iRow), 1, qItem2);

        // записываем численные результаты в таблицу
        for(int iCol=2; iCol<8; ++iCol)
        {
            double dValue;
            switch (iCol)
            {

                case 2: // оставляем 2 знака после запятой (хотя лучше это делать при переводе в строку)
                    dValue = isCircle() ?
                                double(int(image->m_EnergyCircle*100))/100 :
                                double(int(image->m_EnergySquare*100))/100;
                    break;

                case 3:
                    dValue = isCircle() ?
                                int(image->m_CenterXCircle):
                                int(image->m_CenterXSquare);
                    break;
                case 4:
                    dValue = isCircle() ?
                                int(image->m_CenterYCircle):
                                int(image->m_CenterYSquare);
                    break;

                case 5:
                    dValue = isCircle() ?
                                image->m_SizeCircle:
                                image->m_SizeSquare;
                    break;

                case 7:
                    dValue = image->amount_overexposure;
                    break;
                default:
                    dValue = -1;
                    break;
            }
            qDebug() << iCol << " " << dValue;
            // создаем новую ячейку (Item) таблицы
            QTableWidgetItem* qItemN = new QTableWidgetItem();
            // устанавливаем параметры: ячейка доступна + ее можно выбрать (+ ее можно редактировать | Qt::ItemIsEditable
            qItemN->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            qItemN->setTextAlignment(Qt::AlignCenter);
            // устанавливаем значение в ячейку как double
            qItemN->setData(0, QVariant::fromValue(dValue));
            // устанавливаем созданную ячейку в нужное место в таблицы
            ui->m_TW_Res->setItem(int(iRow), iCol, qItemN);

        }
    }
    ui->m_TW_Res->setSortingEnabled(false);
    //ui->m_TW_Res->selectAll();
    ui->m_PB_Center->setEnabled(true);
    ui->m_PB_Zoom_In->setEnabled(true);
    ui->m_PB_Zoom_Out->setEnabled(true);
    ui->m_PB_Zoom_Default->setEnabled(true);
}
//-------------------------------------------------------------------------------------------------
// рисуем ФРТ от выделенной строчки в таблице
void Dialog::onSelectRow()
{
    QModelIndexList selection = ui->m_TW_Res->selectionModel()->selectedRows();

    // Multiple rows can be selected
    if(selection.count()>0)
    {
        // берем значение из колонки "индекс"
        //int row = ui->m_TW_Res->item(selection.at(0).row(), 5)->text().toInt();
        int row = selection.at(0).row();
        DrawImage(row);
    }
}
//-------------------------------------------------------------------------------------------------
void Dialog::onOpenImage()
{
    // создаем диалоговое окно для выбора имени файла
    QStringList sFileName = QFileDialog::getOpenFileNames(this, "Открыть изображение", "", "Image (*.bmp)");

    // загружаем изображения (добавляем к уже загруженным)   
    core->loadImages(sFileName);
    core->calculate(getCalcType());
    UpdateTable();

}
//-------------------------------------------------------------------------------------------------
void Dialog::onCheckEnabled()
{
    bool bCalcEnergy=ui->m_RB_CalcEnergy->isChecked();
    ui->m_LE_AreaSize->setEnabled(bCalcEnergy);
    ui->m_LE_EnergySize->setEnabled(!bCalcEnergy);

    bool bCenterSet=ui->m_RB_CenterSet->isChecked();
    ui->m_LE_Center_X->setEnabled(bCenterSet);
    ui->m_LE_Center_Y->setEnabled(bCenterSet);

    ui->m_RB_CenterEnergy->setEnabled(bCalcEnergy);
}
//-------------------------------------------------------------------------------------------------
void Dialog::onZoomIn() {
    ui->m_GV_Image->scale(1.2, 1.2);
    scalingFactor *= 1.2;
}
void Dialog::onZoomOut() {
    ui->m_GV_Image->scale(1.0 / 1.2, 1.0 / 1.2);
    scalingFactor /= 1.2;
}
void Dialog::onZoomDefault() {
    ui->m_GV_Image->scale(1.0 / scalingFactor, 1.0 / scalingFactor);
    scalingFactor = 1.0;
}
void Dialog::onCenterImage() {
    Parser* image = core->getImage(selectedImage);
    double x=image->m_CenterXCircle,
           y=image->m_CenterYCircle;
    ui->m_GV_Image->centerOn(x, y);

}

void Dialog::displayMenu(const QPoint &pos)
{

    QMenu menu(this);

    QAction *remove = menu.addAction("Удалить выбранное изображение");

    QAction *a = menu.exec(ui->m_TW_Res->viewport()->mapToGlobal(pos));
    QModelIndexList selection = ui->m_TW_Res->selectionModel()->selectedRows();

    if (a == remove){
        if (selection.count() > 0) {
            QModelIndex index = selection.at(0);
            int row = index.row();
            ui->m_TW_Res->removeRow(row);
            // delete from core
            core->deleteImage(row);
        }}
}
//переимeновать, они перепутаны
void Dialog::onCalculateSelected() {

    QModelIndexList selection = ui->m_TW_Res->selectionModel()->selectedRows();
    core->calculateOverlay(selection);
    UpdateTable();
}
void Dialog::onCalculateAvarage() {

    QModelIndexList selection = ui->m_TW_Res->selectionModel()->selectedRows();
    core->calculate(selection, getCalcType());
    UpdateTable();
}

bool Dialog::isCircle(){
    return ui->m_RB_Circle->isChecked();
}

AreaCenterType Dialog::getCenterType()
{
        if (ui->m_RB_CenterMass->isChecked())
        {
            return AUTO_BY_MASS_CENTER;
        }
        else if (ui->m_RB_CenterEnergy->isChecked())
        {
            return  AUTO_BY_ENERGY_MAX;
        }
        else
        {
            return MANUAL;
        }
}

CalcType Dialog::getCalcType()
{
        if (ui->m_RB_CalcEnergy->isChecked())
        {
             return ENERGY_BY_REGION;
        }
        else
        {
            return REGION_BY_ENERGY;
        }
}

QPair<double, double> Dialog::getAreaCenter()
{
    return QPair<double, double> ( ui->m_LE_Center_X->text().toDouble(), ui->m_LE_Center_Y->text().toDouble());

}


double Dialog::getEnergy()
{
    return ui->m_LE_EnergySize->text().toDouble();
}

double Dialog::getRadius()
{
    return ui->m_LE_AreaSize->text().toDouble();
}

