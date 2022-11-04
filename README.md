<h1 align = "center">Automated irrigation controller for greenhouses </h1>

<p align="center">
  <img src="https://user-images.githubusercontent.com/101992463/199425279-c3ccb938-c73b-416d-9bfd-d49b3f211f4c.png" />
</p>

This program is for an automatic controller for greenhouse using arduino, you can use this guide to replicate the project, the project was reviewed and accepted by IEEE 2022.
<p>The project was focused in precision agriculture and was created with recycled materials. If you want to read more about the project you can read all the project<a href="https://github.com/DCenten0/Automated-irrigation-controller-for-greenhouses/blob/main/Design%20and%20implementation%20of%20an%20automated%20greenhouse%20irrigation%20controller%2C%20made%20with%20recycled%20materials%2C%20focused%20on%20precision%20agriculture..pdf" title="Title">
Here.</a></p>

The program is built in such a way that each process is independent of each other, in other words, no process affects the operation of the other. This means that a multiprogramming environment was achieved.

<p>You can use the arduino code freely and without limitations. If you want to see the code go <a href="https://github.com/DCenten0/Automated-irrigation-controller-for-greenhouses/blob/main/GreenHouse_Controller.ino" title="Title">
Here.</a></p>


<h2>The materials you need are:</h2> 

- Arduino nano. 
- Bluetooth module HC-06 or ESP-01 as communication model.
- DHT11 (Sensor used to take the values of relative temperature, relative humidity and heat index).
- LDR (Component used to calculate the ambient light).
- Two Raindrop sensor analyser (Sensors used to calculate the Humidity of floor).
- A powerful DC motor or Stepper motor capable to move or open a water tap.
- Four or eight IRF640 mosfet the amount depend if you will use a stepper motor or a DC motor in my case I used a DC motor.
- Nine lithium batteries 18650 to make a bank of batteries with 10,800mAh.
- DC-DC voltage regulator with 3.3V and 5V output to power the microcontroller and communication models.  
- Timer 555 to create a voltage doubler to saturate the mosfet IRF640.

<h2>Structure of the controller and percentage of recycled materials, reused and new.</h2> 

![design](https://user-images.githubusercontent.com/101992463/199433894-5e852649-2730-4489-8112-acb3c16af4d3.PNG)

<h2>Some Electrical diagrams</h2> 

<h3>Mosfet Bridge H and connection with the arduino</h3>

![D640DVE V2](https://user-images.githubusercontent.com/101992463/199432464-18c25fc9-18aa-44fc-bbd0-aa046e1e72eb.PNG)

<h3>Voltage doubler with timer 555</h3>

![Duplicador de voltaje imagen](https://user-images.githubusercontent.com/101992463/199432839-c9db5d66-ce88-49df-8e5d-4c54c98916ed.PNG)

<h2>Data Collected by the controller in the last 48 hours</h2> 

![Plots](https://user-images.githubusercontent.com/101992463/199434379-de5b1003-0183-4cfd-9551-b26e21dc2e01.PNG)





