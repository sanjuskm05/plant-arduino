
# Motivation

Plants are great, especially indoor plants, which improve air quality inside the house and help reduce our stress levels. But it is often a tedious task to maintain and take care of them. Sometimes we forget to water them and sometimes we water them too much. Some prefer more humidity and some less. Tracking the amount of light, moisture, water, and temperature can be a guessing game. Following are few problems which motivated us to build an autonomous system to solve them:
1.	No one is present to take care of plants. It happens when we go for vacation.
2.	Different type of plants has different need such as different soil moisture, temperature, humidity, and light. It is hard to keep track of each plant needs such as bamboo needs watering once in a week whereas tomato needs watering every 2-3 days. Also, it is hard to take other parameter such as temperature and light to account while watering them.
Our autonomous system - “Mintonomous” helps us do following:
1.	Monitor the soil moisture, temperature, humidity, light and log the data.
2.	Irrigate the plant in most efficient way possible.
3.	Control supplemental artificial lighting.
4.	Less expensive. (<$100)
5.	Manual controlled by mobile or web application.



![Picture1](https://user-images.githubusercontent.com/8688478/145160164-03544b3e-0b34-48be-b2ab-2eb4017fc32b.png)

Mintonomous at the lowest level serves just a plant in the current POC. Here, it has an “arduino pro micro” with 2 sensors, soil moisture and light sensors. These are analog sensors, so we use the ADC available on the arduino to read values. Aside from sensing, system has an RGB LED strip connected to the arduino. The arduino receives a command from the “ESP 32” to turn on the LEDs. The LED strip being addressable, system can specify brightness levels of all 3 colors to add artificial lighting as needed. In short, the arduino sends, when requested, data points and receive some amount of data to manipulate the LED strip. 
The next level up, is the ESP 32. 	Here, it senses humidity and temperature, 2 in 1 sensor. ESP32 fetches the data from the arduino(s), parse/format all the data, to push it to the database over wifi using MQTT.

The “last” bit of hardware is a pump and relay board. The pump operates at 12V, much higher than the 3.3V logic that the ESP32 and arduino uses. There are 4 available relays, and each is controlled independently. Upon a request to water a plant by the backend software via MQTT, the ESP32 controls the relay board which controls the pump, turning it on/off.

The next level up is the backend software. The ESP32 pushes data using web service on AWS cloud via MQTT. Web services also computes the delta in each reading and decides on a proper action based on threshold configured for the plant.
The system also has a ReactJS GUI to help us visualize the real-time data, monitor, and configure the threshold for the plant.

Web service:
https://github.com/sanjuskm05/mintonomous-boot-app

GUI:
https://github.com/sanjuskm05/my-plant-main

# Topology
![image](https://user-images.githubusercontent.com/8688478/145686292-f824b964-2349-453d-8b8b-2d39714b3d6f.png)
