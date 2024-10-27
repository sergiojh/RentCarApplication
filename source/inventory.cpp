#include "inventory.h"

Inventory::Inventory()
{
    mp_inventory = new std::map<std::string, CarInfo>();
}

Inventory::~Inventory()
{
    mp_inventory->clear();
    delete mp_inventory;
}

void Inventory::AddCarRental(const std::string &name, TypeOfCar carType, int amountCars)
{
    if (!mp_inventory)
        return;

    m_mtx.lock();

    if (mp_inventory->count(name))
    {
        mp_inventory->at(name).totalCars += amountCars;
        mp_inventory->at(name).availableCars += amountCars;
    }
    else
    {
        CarInfo car = {
            carType,
            amountCars,
            amountCars};
        (*mp_inventory)[name] = car;
    }

    m_mtx.unlock();
}

bool Inventory::DeleteCarRental(const std::string &name, int amountCars)
{
    if (!mp_inventory)
        return false;

    bool bResult = false;
    if (mp_inventory->count(name))
    {
        int carsNumber = 0;
        if (mp_inventory->at(name).availableCars < amountCars)
            carsNumber = mp_inventory->at(name).availableCars;
        else
            carsNumber = amountCars;
        mp_inventory->at(name).totalCars -= carsNumber;
        mp_inventory->at(name).availableCars -= carsNumber;

        if (mp_inventory->at(name).totalCars < 0)
        {
            mp_inventory->at(name).totalCars = 0;
            mp_inventory->at(name).availableCars = 0;
        }

        bResult = true;
    }

    return bResult;
}

bool Inventory::RentCar(const std::string &name)
{
    bool bResult = false;
    if (!mp_inventory)
        return bResult;

    m_mtx.lock();

    if (mp_inventory->count(name) && mp_inventory->at(name).availableCars > 0)
    {
        mp_inventory->at(name).availableCars--;
        bResult = true;
    }

    m_mtx.unlock();

    return bResult;
}
bool Inventory::ReturnCar(const std::string &name)
{
    bool bResult = false;
    if (!mp_inventory)
        return bResult;

    m_mtx.lock();

    if (mp_inventory->count(name) &&
        mp_inventory->at(name).availableCars < mp_inventory->at(name).totalCars)
    {
        mp_inventory->at(name).availableCars++;
        bResult = true;
    }

    m_mtx.unlock();

    return bResult;
}

const nlohmann::json Inventory::InventoryToJson()
{
    nlohmann::json jsonData = {};
    nlohmann::json inventoryData = {};
    for (auto a : *mp_inventory)
    {
        inventoryData +=
            {{"Name", a.first},
             {"CarType", a.second.carType},
             {"CarAvailable", a.second.availableCars},
             {"TotalCars", a.second.totalCars}};
    }
    jsonData["Inventory"] = inventoryData;
    nlohmann::json typeOfCarsData = {{"Premiun", Inventory::TypeOfCar::Premiun},
                                     {"SUV", Inventory::TypeOfCar::SUV},
                                     {"Small", Inventory::TypeOfCar::Small}};
    jsonData["TypeOfCar"] = typeOfCarsData;
    return jsonData;
}

Inventory::TypeOfCar Inventory::GetTypeCar(const std::string &name)
{
    Inventory::TypeOfCar carType;
    m_mtx.lock();

    if (mp_inventory->count(name))
        carType = mp_inventory->at(name).carType;
    else
        carType = Inventory::TypeOfCar::Final;

    m_mtx.unlock();
    return carType;
}
