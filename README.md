# Ride-Sharing Platform (C++ Project)

This project implements a basic ride-sharing platform in C++, aimed at connecting users traveling between cities and allowing them to share rides when their routes overlap. It uses a static dataset of cities and distances to simulate route planning, cost calculation, and ride matching.

## Overview

The system is designed to support the following workflow:
- Users and drivers register with the system.
- The system holds a map of cities and distances between them.
- When two users enter their source and destination cities, the system determines whether their routes overlap.
- If overlap is found, they are allowed to share the ride and split the cost based on distance traveled.

The main goal of the project is to apply core concepts of data structures (linked lists, file handling, graphs) while simulating a real-world application.

## Core Features

1. Driver and User Management  
Manages driver and user records using linked lists.

2. Static City Connectivity  
Uses static data of connected cities and their distances to calculate routes.

3. Ride Matching Algorithm  
Determines whether two users can share a ride based on a common sub-path in their routes.

4. Cost Calculation and Sharing  
Calculates the ride cost and fairly distributes it among users sharing the trip.

## File Structure

**DataBase_management_src.cpp**  
Contains the logic to manage driver and user data using linked lists. Handles:
- Adding new drivers or users
- Deleting or updating existing records
- Reading from and writing to text files

**ride_sharing.cpp**  
Implements the logic for:
- Reading city data
- Checking for shared routes
- Calculating total and individual costs
- Interacting with the user via input/output

**cities.txt**  
Contains city-to-city distance data used for simulating a transportation map. Each line represents a connection:
Format: CityA CityB Distance

Example:
Jaipur Ajmer 132  
Ajmer Bhilwara 132

**driver.txt**  
Stores driver registration data. Format:
Name, LicenseNumber, PhoneNumber

Example:
Mukesh Kumar, DLIN1234567890, 8764546890  
Priya Sharma, DLIN0987654321, 4567822358

This file is read during ride-matching operations.

## Prerequisites

To run this project, you need:
- A C++ compiler (e.g., GCC for Linux/Mac or MinGW for Windows)
- A terminal or command prompt to compile and run the program
- Basic understanding of how to run command-line C++ programs

## Setup Instructions

1. Download or clone the repository:  
git clone https://github.com/Saurav1061/Ride_Sharing.git

2. Move to the project directory:  
cd DSA_Project_Ride_Sharing_Platform

3. Compile the source code:  
g++ -o ride_sharing DataBase_management_src.cpp ride_sharing.cpp -std=c++11

4. Run the program:  
./ride_sharing

## How to Use the Platform

**Step 1: Register Users and Drivers**  
- Add driver details to driver.txt in this format:  
  Full Name, License Number, Phone Number

- Add user details to user.txt.  
  Example format may include:  
  User Name, Phone Number, Source City, Destination City

**Step 2: Check for Ride Sharing Compatibility**  
- The system will prompt you to input details of two users.
- Enter the starting and ending cities for both.
- The system will analyze if a shared path is available between both city pairs.
- If ride sharing is possible, it displays:
  - Shared segment of the journey
  - Distance covered
  - Cost split between users

## Example Data

**cities.txt**  
Jaipur Ajmer 132  
Jaipur Alwar 150  
Ajmer Bhilwara 132  
Bhilwara Udaipur 150  
Ajmer Kota 200  
Kota Udaipur 270  
Udaipur MountAbu 185

**driver.txt**  
Mukesh Kumar, DLIN1234567890, 8764546890  
Priya Sharma, DLIN0987654321, 4567822358

**user.txt**  
Ravi Meena, 9876543210, Jaipur, Udaipur  
Anjali Singh, 8765432109, Ajmer, Udaipur

## Potential Enhancements

- Replace static data with dynamic input or API-based city data.
- Add shortest path algorithm (like Dijkstra’s) for more realistic routing.
- Create a GUI for user-friendly interaction.
- Add authentication and session management for users and drivers.

