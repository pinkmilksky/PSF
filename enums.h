    #ifndef ENUMS_H
    #define ENUMS_H
    //способ, которым задается коодинаты центра области
    enum AreaCenterType
    {
        AUTO_BY_MASS_CENTER,
        AUTO_BY_ENERGY_MAX,
        MANUAL
    };
    //способ вычисления энергии
    enum CalcType
    {
        ENERGY_BY_REGION,
        REGION_BY_ENERGY
    };

    #endif // ENUMS_H
