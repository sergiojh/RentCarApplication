#ifndef INVENTORY_H
#define INVENTORY_H

#include <string>
#include <map>
#include <mutex>
#include "json.hpp"

/**
 * @brief Manages an inventory of car. Allows to add new cars, rent existing cars and return rented cars.
 *
 */
class Inventory
{
public:
    enum TypeOfCar
    {
        Premiun = 0,
        SUV,
        Small,
        Final
    };
    /**
     * @brief Construct a new Inventory object
     *
     */
    Inventory();
    /**
     * @brief Destroy the Inventory object
     *
     */
    ~Inventory();
    /**
     * @brief Adds cars to the inventory
     *
     * @param name Car of the car added
     * @param carType Type of car to be added
     * @param amountCars Total cars added to inventory
     */
    void AddCarRental(const std::string &name, TypeOfCar carType, int amountCars);
    /**
     * @brief Delete cars from the inventory
     *
     * @param name Car of the car deleted
     * @param amountCars Total cars deleted from inventory
     */
    bool DeleteCarRental(const std::string &name, int amountCars);
    /**
     * @brief Retrieve a car from inventory
     *
     * @param name name of the car to be rented
     * @return true Car could be rented
     * @return false Car could not be rented
     */
    bool RentCar(const std::string &name);
    /**
     * @brief Return a car that was rented
     *
     * @param name name of the car that was rented
     * @return true Car returned to inventory
     * @return false Car was not able to return to inventory
     */
    bool ReturnCar(const std::string &name);
    /**
     * @brief Get the TypeCar given a name of a car
     *
     * @param name name of the car
     * @return TypeOfCar TyperOfCar returned for the given name,
     * Returns TypeOfCar::Final if name doesnt exists in the inventory
     */
    TypeOfCar GetTypeCar(const std::string &name);
    /**
     * @brief Get all inventory and return it as string
     *
     * @return nlohmann::json
     */
    const nlohmann::json InventoryToJson();

private:
    struct CarInfo
    {
        TypeOfCar carType;
        int availableCars;
        int totalCars;
    };
    std::mutex m_mtx;
    std::map<std::string, CarInfo> *mp_inventory;
};

#endif