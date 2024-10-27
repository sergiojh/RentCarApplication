#ifndef MESSAGES_H
#define MESSAGES_H

#include <string>
#include "inventory.h"

struct Points
{
    std::string clientId;
};

struct AddCar
{
    std::string carName;
    Inventory::TypeOfCar typeOfCar;
    int numberOfCars;
};

struct DeleteCar
{
    std::string carName;
    int numberOfCars;
};

struct RentCar
{
    std::string carName;
    std::string clientId;
    int daysRented;
};

struct ReturnCar
{
    std::string clientId;
    std::string carName;
    int daysRented;
    int daysUsed;
};
#endif