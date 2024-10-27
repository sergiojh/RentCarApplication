#include "rentalManager.h"
#include <algorithm>

RentalManager *RentalManager::m_instance = nullptr;

RentalManager *RentalManager::GetInstace()
{
    if (m_instance == nullptr)
    {
        m_instance = new RentalManager();
    }

    return m_instance;
}

RentalManager::RentalManager()
{
    m_inventory.AddCarRental("BMW M4", Inventory::Premiun, 2);
    m_inventory.AddCarRental("Nissan Juke", Inventory::SUV, 4);
    m_inventory.AddCarRental("Kia Sorento", Inventory::SUV, 1);
    m_inventory.AddCarRental("Honda Civic", Inventory::Small, 7);
}

RentalManager::~RentalManager()
{
    for (auto clientInfo : clientPool)
    {
        clientInfo.second.rentedPool.clear();
    }
    clientPool.clear();
}

nlohmann::json RentalManager::RentCar(const std::string &clientId, const std::string &carName, int daysRented)
{
    if (clientId == "")
        return "{ \"Error\": \"clientId not valid\" }"_json;

    if (daysRented <= 0)
        return "{ \"Error\": \"DaysRented not valid\" }"_json;

    float rentPrice = -1.0f;
    std::string result = "";
    if (m_inventory.RentCar(carName))
    {
        m_mtx.lock();

        RentedInfo rentedInfo;
        rentedInfo.carName = carName;
        rentedInfo.daysRented = daysRented;

        // If client don not create new client
        if (!clientPool.count(clientId))
        {
            // Initialize new client point to 0
            ClientInfo clientInfo;
            clientInfo.points = 0;
            clientPool[clientId] = clientInfo;
        }
        clientPool[clientId].rentedPool.push_back(rentedInfo);
        rentPrice = CalculatePriceByTypeAndDays(m_inventory.GetTypeCar(carName), daysRented);
        m_mtx.unlock();
        result = "{ \"Cost\":" + std::to_string(rentPrice) + "}";
    }
    else
    {
        result = "{ \"Error\": \"" + carName + " could not be rented\" }";
    }

    return result;
}

nlohmann::json RentalManager::ReturnCar(const std::string &clientId, const std::string &carName, int daysRented, int totalDaysUsed)
{
    float rentPrice = -1.0f;
    RentedInfo rentedInfoGiven;
    rentedInfoGiven.carName = carName;
    rentedInfoGiven.daysRented = daysRented;
    // if client doesnt exist -> return
    m_mtx.lock();
    if (!clientPool.count(clientId))
    {
        m_mtx.unlock();
        return "{ \"Error\": \"Client not valid\" }"_json;
    }
    auto it = std::find(clientPool[clientId].rentedPool.begin(), clientPool[clientId].rentedPool.end(), rentedInfoGiven);
    // client did not rent this car -> return
    if (it == clientPool[clientId].rentedPool.end())
    {
        m_mtx.unlock();
        return "{ \"Error\": \"Client did not rent this car\" }"_json;
    }
    // If found then return
    if (!m_inventory.ReturnCar(carName))
    {
        m_mtx.unlock();
        return "{ \"Error\": \"Could not return car\" }"_json;
    }

    std::string result = "";
    clientPool[clientId].points += CalculatePoints(carName);
    rentPrice = CalculatePriceExtraDays(m_inventory.GetTypeCar(carName), totalDaysUsed - daysRented);
    clientPool[clientId].rentedPool.erase(it);
    m_mtx.unlock();

    result = "{ \"Status\":\"Car Returned\", \"Status\": " + std::to_string(rentPrice) + "}";

    return result;
}

std::string RentalManager::ShowInventory()
{
    return m_inventory.InventoryToJson().dump();
}

nlohmann::json RentalManager::GetClientPoints(const std::string &clientId)
{
    int points = 0;
    m_mtx.lock();
    std::string result = "";
    if (clientPool.count(clientId))
    {
        points = clientPool[clientId].points;
        result = "{\"" + clientId + "\":" + std::to_string(points) + "}";
    }
    else
    {
        result = "{ \"Error\": \"Client not found\" }";
    }
    m_mtx.unlock();
    return result;
}

int RentalManager::CalculatePoints(const std::string &carName)
{
    int points = 0;
    Inventory::TypeOfCar carType = m_inventory.GetTypeCar(carName);
    switch (carType)
    {
    case Inventory::TypeOfCar::Premiun:
        points = 5;
        break;
    case Inventory::TypeOfCar::SUV:
        points = 3;
        break;
    case Inventory::TypeOfCar::Small:
        points = 1;
        break;
    default:
        break;
    }

    return points;
}

float RentalManager::CalculatePrice(Inventory::TypeOfCar carType, int daysRented, int totalDaysUsed)
{
    float finalPrice = 0.0f;
    finalPrice += CalculatePriceByTypeAndDays(carType, daysRented);
    return finalPrice;
}

float RentalManager::CalculatePriceExtraDays(Inventory::TypeOfCar carType, int extraDays)
{
    float finalPrice = 0.0f;
    if (extraDays <= 0)
        return finalPrice;

    switch (carType)
    {
    case Inventory::TypeOfCar::Premiun:
        finalPrice = extraDays * (basePricePremiun + (basePricePremiun * 0.2f));
        break;
    case Inventory::TypeOfCar::SUV:
        finalPrice = extraDays * (basePriceSUV + (basePriceSUV * 0.6f));
        break;
    case Inventory::TypeOfCar::Small:
        finalPrice = extraDays * (basePriceSmall + (basePriceSmall * 0.3f));
        break;
    default:
        break;
    }

    return finalPrice;
}

float RentalManager::CalculatePriceByTypeAndDays(Inventory::TypeOfCar carType, int daysRented)
{
    float finalPrice = 0.0f;
    switch (carType)
    {
    case Inventory::TypeOfCar::Premiun:
        finalPrice = basePricePremiun * daysRented;
        break;
    case Inventory::TypeOfCar::SUV:
        finalPrice = CalculatePriceSUV(daysRented);
        break;
    case Inventory::TypeOfCar::Small:
        finalPrice = CalculatePriceSmall(daysRented);
        break;
    default:
        break;
    }

    return finalPrice;
}

float RentalManager::CalculatePriceSmall(int daysRented)
{
    float finalPrice = 0.0f;
    if (daysRented <= 0)
        return finalPrice;

    int daysNormalPrice = (daysRented <= 7) ? daysRented : 7;
    int daysLeft = (daysRented > 7) ? daysRented - 7 : 0;

    finalPrice += basePriceSmall * daysNormalPrice;
    finalPrice += (basePriceSmall * 0.6f) * daysLeft;

    return finalPrice;
}

float RentalManager::CalculatePriceSUV(int daysRented)
{
    float finalPrice = 0.0f;
    if (daysRented <= 0)
        return finalPrice;

    int daysNormalPrice = (daysRented <= 7) ? daysRented : 7;
    int daysFirstDiscount = 0;
    int daysLeft = 0;

    if (daysRented > 7)
    {
        daysFirstDiscount = (daysRented <= 30) ? daysRented - 7 : 23;
        daysLeft = (daysRented > 30) ? daysRented - 30 : 0;
    }

    finalPrice += basePriceSUV * daysNormalPrice;
    finalPrice += (basePriceSUV * 0.8f) * daysFirstDiscount;
    finalPrice += (basePriceSUV * 0.5f) * daysLeft;

    return finalPrice;
}

nlohmann::json RentalManager::AddCarsToInventory(const std::string &carName, Inventory::TypeOfCar carType, int carNumber)
{
    if (carName == "")
        return "{ \"Error\": \"CarName not valid\" }"_json;

    if (m_inventory.GetTypeCar(carName) == Inventory::Final &&
        carType >= Inventory::Final)
        return "{ \"Error\": \"CarType not valid\" }"_json;

    if (carNumber <= 0)
        return "{ \"Error\": \"CarNumber not valid\" }"_json;

    m_inventory.AddCarRental(carName, carType, carNumber);
    std::string result = "{ \"Result\": \"" + carName + " added\" }";
    return result;
}

nlohmann::json RentalManager::DeleteCarsFromInventory(const std::string &carName, int carNumber)
{
    if (carName == "")
        return "{ \"Error\": \"CarName not valid\" }"_json;

    if (carNumber <= 0)
        return "{ \"Error\": \"CarNumber not valid\" }"_json;
    std::string result = "";
    if (m_inventory.DeleteCarRental(carName, carNumber))
        result = "{ \"Result\": \"" + carName + " Deleted\" }";
    else
        result = "{ \"Result\": \"" + carName + " could not be deleted\" }";
    return result;
}