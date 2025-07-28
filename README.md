# Master's Thesis

## Description

This project was created as the implementation of my Master's Thesis @ NTUA ECE 2023, with the title:
**"Simulation of Content Distribution Policies in Vehicular Ad-Hoc Networks (VANETs) with the use of Machine Learning Algorithms".**

The code is written in C++ and Python and is built using OMNET++, Veins and SUMO.

The main goal of this diploma thesis is the creation of different simulation scenarios for Vehicular ad-hoc networks and the application of content distribution policies,
in order to compare their effectiveness.

To achieve this goal, the Discrete Event Simulator OMNET++, alongside with Veins and SUMO were used.
More specifically, the code written adds various functionalities to the pre-existing Veins simulations. The most noteworthy out of those functionalities include
metrics calculations and the implementation of caching and content distribution policies. At the same time, machine learning algorithms can be used to
easily and quickly predict the best way to distribute content to the nodes of the network. These algorithms are pre-trained neural network models from the scikit-learn Python library.

Using these new functions, we can run various simulations, specifying the parameters we want each simulation to have (such as the type of metric used) and measure the response times,
so that we can determine which metrics and policies are better for content distribution.

> 📄 Published in *Electronics* (MDPI), 2023: [https://doi.org/10.3390/electronics12132901](https://doi.org/10.3390/electronics12132901)

## 🛠️ Installation

### Prerequisites
Ensure you have the following installed:

- [OMNeT++ 5.6.2](https://omnetpp.org/)
- [Veins 5.2](https://veins.car2x.org/)
- [SUMO 1.8.0](https://sumo.dlr.de/)

### Setup Steps
1. Clone or download the repository.
2. Either:
   - Use the **modified files** in your own Veins installation, or
   - Download the latest **release (v1.0-full)** containing the entire modified framework.
3. Copy the repository contents into your Veins project directory.
4. Open the project in OMNeT++ and build it.
5. Adjust `import` and `include` paths as needed if you’re integrating with a custom setup.

## 📂 Repository Structure
```
.
├── src/  
│ ├── modules/ # Custom modules
│ └── nodes/  
├── algorithms/ # ML algorithms & utilities
├── results/ # Different scenario results and comparisons
├── scenario/ # Configured scenario
└── README.md
```

You can find the paper [here](https://doi.org/10.3390/electronics12132901)
