# spiking_neural_network

This project aims to implement a spiking neural network and try to use it for unsupervised learning.

## Building this project

```
git clone --recursive https://github.com/mathieu-schertenleib/spiking_neural_network.git
cd spiking_neural_network
cmake -S . -B build
cmake --build build
```

## External libraries

- [ImGui](https://github.com/ocornut/imgui)
- [ImPlot](https://github.com/epezent/implot)

## References

- Gerstner, W., Kistler, W. M., Naud, R., & Paninski L. (2014). _Neuronal Dynamics_. Cambridge University
  Press. https://neuronaldynamics.epfl.ch
- [Spiking neural network (Wikipedia)](https://en.wikipedia.org/wiki/Spiking_neural_network)
- [Biological neuron model (Wikipedia)](https://en.wikipedia.org/wiki/Biological_neuron_model)
- [Spike response model (Wikipedia)](https://en.wikipedia.org/wiki/Spike_response_model)

## License

This software is released under [MIT License](LICENSE)
