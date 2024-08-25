# VANETs-Master Thesis

## Description 

This project was created as the implementation of my Master's Thesis @ NTUA ECE 2023, with the title:
"Simulation of Content Distribution Policies in Vehicular Ad-Hoc Networks (VANETs) with the use of Machine Learning Algorithms".
The code is written in C++ and Python and is built using OMNET++, Veins and SUMO.

The main goal of this diploma thesis is the creation of different simulation scenarios for Vehicular ad-hoc networks and the application of content distribution policies, 
in order to compare their effectiveness. 

To achieve this goal, the Discrete Event Simulator OMNET++, alongside with Veins and SUMO were used. 
More specifically, the code written adds various functionalities to the pre-existing Veins simulations. The most noteworthy out of those functionalities include 
metrics calculations and the implementation of caching and content distribution policies. At the same time, machine learning algorithms can be used to 
easily and quickly predict the best way to distribute content to the nodes of the network. These algorithms are pre-trained neural network models from the scikit-learn Python library.

Using these new functions, we can run many different simulations, specifying the parameters we want each simulation to have (such as the type of metric used) and measure the response times, 
in order to determine which metrics and policies are better for content distribution.

The thesis has been published as a paper: https://doi.org/10.3390/electronics12132901

## Installation

The prerequisites for running this code are:
- OMNET++ 5.6.2
- Veins 5.2
- Sumo 1.8.0

Once these are installed, copy the contents of the repository into the folder in which you installed Veins and build the project using OMNET++.
