#ifndef RENTALMANAGER_H
#define RENTALMANAGER_H

#include "inventory.h"
#include <list>

/**
 * @brief Manages rental system by allowing clients to rent and return cars.
 * Also tracks points for each client.
 *
 */
class RentalManager
{
public:
    /**
     * @brief Singletons should not be cloneable.
     */
    RentalManager(RentalManager &other) = delete;
    /**
     * @brief Singletons should not be assignable.
     */
    void operator=(const RentalManager &) = delete;
    /**
     * @brief This is the static method that controls the access to the RentalManager
     * instance. On the first run, it creates a RentalManager object and places it
     * into the static field.
     */
    static RentalManager *GetInstace();
    /**
     * @brief Destroy the Rental Manager object
     *
     */
    ~RentalManager();
    /**
     * @brief Rents a car for a given client
     *
     * @param clientId Client id
     * @param carName Name of the car to be rented
     * @param daysRented Total days of the rent
     * @return nlohmann::json Price of rent or errors as json
     */
    nlohmann::json RentCar(const std::string &clientId, const std::string &carName, int daysRented);
    /**
     * @brief Return a car from the client
     *
     * @param clientId Client id
     * @param carName Name of the rented car
     * @param daysRented Total days of the rent days given by client
     * @param totalDaysUsed Total days past since client rented the car
     * @return nlohmann::json Cost of the rent, Error if client did not rent this car
     */
    nlohmann::json ReturnCar(const std::string &clientId, const std::string &carName, int daysRented, int totalDaysUsed);
    /**
     * @brief Return the inventory that the Rental has as string
     *
     * @return std::string
     */
    std::string ShowInventory();
    /**
     * @brief Get the Client Points given a ClientId
     *
     * @param clientId
     * @return nlohmann::json Total points or error as json
     */
    nlohmann::json GetClientPoints(const std::string &clientId);
    /**
     * @brief Adds car to inventory, if car already exists carType will no be taken into account
     *
     * @param carName Name of car to be added
     * @param carType Type of car to be added
     * @param carNumber Number of cars to be added
     * @return nlohmann::json Successful msg or Error
     */
    nlohmann::json AddCarsToInventory(const std::string &carName, Inventory::TypeOfCar carType, int carNumber);
    /**
     * @brief Delete car from inventory
     *
     * @param carName CarName to be deleted
     * @param carNumber Numbers of car to be deleted
     * @return nlohmann::json Successful msg or Error
     */
    nlohmann::json DeleteCarsFromInventory(const std::string &carName, int carNumber);

private:
    /**
     * @brief Construct a new Rental Manager object
     *
     */
    RentalManager();
    /**
     * @brief Calculates points given to customer given a CarName
     *
     * @param carName
     * @return int
     */
    int CalculatePoints(const std::string &carName);
    /**
     * @brief Calculates the price of a rental given daysRented, totalDaysUsed by customer and car type
     *
     * @param carType
     * @param daysRented Days that client rented the car
     * @param totalDaysUsed Total days that client used the car
     * @return float Total Price of the rental
     */
    float CalculatePrice(Inventory::TypeOfCar carType, int daysRented, int totalDaysUsed);
    /**
     * @brief Calculates cost of the extra days that the car was return late
     *
     * @param carType
     * @param extraDays
     * @return float Price of the rental for the extra days
     */
    float CalculatePriceExtraDays(Inventory::TypeOfCar carType, int extraDays);
    /**
     * @brief Calculates cost of rental given car type and total days of rental
     *
     * @param carType
     * @param extraDays
     * @return float Price of the rental
     */
    float CalculatePriceByTypeAndDays(Inventory::TypeOfCar carType, int daysRented);
    /**
     * @brief Calculates cost of rental of Small car type given the days of rental
     *
     * @param daysRented
     * @return float
     */
    float CalculatePriceSmall(int daysRented);
    /**
     * @brief Calculates cost of rental of SUV car type given the days of rental
     *
     * @param daysRented
     * @return float
     */
    float CalculatePriceSUV(int daysRented);

    struct RentedInfo
    {
        int daysRented;
        std::string carName;
        inline bool operator==(RentedInfo a)
        {
            if (a.daysRented == daysRented && a.carName == carName)
                return true;
            else
                return false;
        }
    };

    struct ClientInfo
    {
        int points;
        std::list<RentedInfo> rentedPool;
    };
    std::mutex m_mtx;
    Inventory m_inventory;
    std::map<std::string, ClientInfo> clientPool;
    int basePricePremiun = 300;
    int basePriceSUV = 150;
    int basePriceSmall = 50;
    static RentalManager *m_instance;
};

#endif