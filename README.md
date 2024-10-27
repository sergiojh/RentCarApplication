This app is only usable in Windows. nlohmann::json and httplite are used for parsing json and managing the http servers.

To use the app just run the exe program, it will start listening at http://localhost:9090. To finish, press enter in the console.
There are 3 method implemented:

GET -> Return the current inventory and types of car availables.
POST -> Adds new cars to the inventory, it is possible to add multiple cars at the same time. It needs to be a json like with header content as "application/json":
"{\"Cars\":[{\"CarName\":\"BMW M3\",\"CarType\":1,\"CarNumber\":2}]}"

Cars contains and array of new cars, if the CarName already existing, it will add CarNumber to it's pool.

PUT -> Has 4 diferent path availables. /Points to get the number of points that a client has, /Rent to rent a car, /Return to return a car and /DeleteCars to delete multiple cars from inventory.

Example, remember to put the header content as "application/json":
For /Points -> "{\"ClientId\":\"ClientName\"}"
For /DeleteCars -> "{\"Cars\":[{\"CarName\":\"BMW M4\",\"CarNumber\":2}]}"
For /Rent -> "{\"ClientId\":\"ClientName\",\"CarName\":\"BMW M4\",\"DaysRented\":2}"
For /Return -> "{\"ClientId\":\"ClientName\",\"CarName\":\"BMW M4\",\"DaysRented\":2,\"DaysUsed\":3}" In this case DaysRented is the number of days that we rented the car and DaysUsed represent then total days the car has been used. If DaysUsed > DaysRented then extraDays fee will be charge, amount changes depending on the amount of extra days.


Json parse error has not been implemented properly because of the time constrain. The app return error when the data is not valid.
Possible ampliations like introducing a DataBase and saving every Rent purchase as an id would take to long for the time constrain and I prefered to implement maps and linked list to show mutex usage when multithread is been used.

A map is used for the inventory implementation because the key is the CarName and it helps managing keys. For clients another map is used that has the client id as key, when the key exists it returns a struct that contains the number of points and a linked list with all current rented car for that user.

Both maps could be swap for a database connection where a table is updated it time we need to rent a car o a new used is introduce.

HOW TO COMPILE
If you are using a Visual Studio Code, then open the folder and run there are already some json setup to run directly with VSCode.

In case VSCode is cannot be used, use the next command in a CMD (Swap PATH_TO_PROJECT for the correct path to the local project):
C:\msys64\mingw64\bin\g++.exe -fdiagnostics-color=always -std=c++17 -g "PATH_TO_PROJECT\source\*.cpp" "PATH_TO_PROJECT\httplite\*.cpp" "PATH_TO_PROJECT\main.cpp" -lws2_32 -o "PATH_TO_PROJECT\main.exe" "-IPATH_TO_PROJECTheader" "-IPATH_TO_PROJECT/nlohmann" "-IPATH_TO_PROJECT/httplite"

