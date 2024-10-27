#include <iostream>
#include "rentalManager.h"
#include "json.hpp"
#include "HttpServer.h"
#include "messages.h"

// This function defines how the web server will response to requests.
// This is a basic request handler, handling GET with a simple string
// and POSTs by reversing and returning the posted string.
// This type of function can take advantage of the simple
// interface of the Request class to support rich request handling.
static httplite::Response HandleRequest(const httplite::Request &request)
{
    httplite::Response response;
    if (request.Verb == "GET")
    {
        // Return inventory and types of cars availables
        RentalManager *rental = RentalManager::GetInstace();
        std::string info = rental->ShowInventory();
        std::wstring inventory(info.begin(), info.end());
        response.Payload.emplace(inventory);
    }
    else if (request.Verb == "POST" && request.GetTotalHeader().find("application/json"))
    {
        std::wstring str = request.Payload->ToString();
        std::string jsonRequest(str.begin(), str.end());
        nlohmann::json json = nlohmann::json::parse(jsonRequest);
        RentalManager *rental = RentalManager::GetInstace();
        nlohmann::json responseJson = {};
        for (auto j : json.at("Cars"))
        {
            AddCar addCarInfo = {};
            j.at("CarName").get_to(addCarInfo.carName);
            j.at("CarNumber").get_to(addCarInfo.numberOfCars);
            j.at("CarType").get_to(addCarInfo.typeOfCar);
            responseJson +=
                rental->AddCarsToInventory(addCarInfo.carName, addCarInfo.typeOfCar, addCarInfo.numberOfCars);
        }
        std::string finalResponse = responseJson.dump();
        std::wstring data(finalResponse.begin(), finalResponse.end());
        response.Payload.emplace(data);
    }
    else if (request.Verb == "PUT" && request.GetTotalHeader().find("application/json"))
    {
        std::wstring str = request.Payload->ToString();
        std::string jsonRequest(str.begin(), str.end());
        nlohmann::json json = nlohmann::json::parse(jsonRequest);
        RentalManager *rental = RentalManager::GetInstace();
        nlohmann::json responseJson = {};
        std::string path = "";
        if (request.Path.size() > 0)
            path = std::string(request.Path.at(0).begin(), request.Path.at(0).end());

        if (path == "Rent")
        {
            RentCar rentCarInfo = {};
            json.at("ClientId").get_to(rentCarInfo.clientId);
            json.at("CarName").get_to(rentCarInfo.carName);
            json.at("DaysRented").get_to(rentCarInfo.daysRented);
            responseJson =
                rental->RentCar(rentCarInfo.clientId, rentCarInfo.carName, rentCarInfo.daysRented);
        }
        else if (path == "Return")
        {
            ReturnCar returnCarInfo = {};
            json.at("ClientId").get_to(returnCarInfo.clientId);
            json.at("CarName").get_to(returnCarInfo.carName);
            json.at("DaysRented").get_to(returnCarInfo.daysRented);
            json.at("DaysUsed").get_to(returnCarInfo.daysUsed);
            responseJson =
                rental->ReturnCar(returnCarInfo.clientId, returnCarInfo.carName, returnCarInfo.daysRented, returnCarInfo.daysUsed);
        }
        else if (path == "Points")
        {
            Points pointsInfo = {};
            json.at("ClientId").get_to(pointsInfo.clientId);
            responseJson = rental->GetClientPoints(pointsInfo.clientId);
        }
        else if (path == "DeleteCars")
        {
            for (auto j : json.at("Cars"))
            {
                DeleteCar deleteCarInfo = {};
                j.at("CarName").get_to(deleteCarInfo.carName);
                j.at("CarNumber").get_to(deleteCarInfo.numberOfCars);
                responseJson +=
                    rental->DeleteCarsFromInventory(deleteCarInfo.carName, deleteCarInfo.numberOfCars);
            }
        }
        else
        {
            responseJson = "{ \"Error\": \"Not a valid path for PUT\"}"_json;
        }
        // Get the POST'd string, reverse it, and set it as the output
        std::string finalResponse = responseJson.dump();
        std::wstring data(finalResponse.begin(), finalResponse.end());
        response.Payload.emplace(data);
    }
    else if (request.Verb == "DELETE")
    {
        // Not implemented
    }
    return response;
}

int main()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0)
    {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        return -1;
    }
    uint16_t port = 9090;
    printf("Starting serving on port %d...\n", (int)port);
    httplite::HttpServer server(port, &HandleRequest);
    server.StartServing(); // binds to port, accepts connections

    printf("Hit [Enter] to stop serving and close the program:\n");
    std::string line;
    std::getline(std::cin, line); // program spends its life here
    return 0;
}