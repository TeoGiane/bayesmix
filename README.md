<img src="resources/logo_full.svg" alt="drawing" width="250"/>

[![Documentation Status](https://readthedocs.org/projects/bayesmix/badge/?version=latest)](https://bayesmix.readthedocs.io/en/latest/?badge=latest)

`bayesmix` is a C++ library for running MCMC simulation in Bayesian mixture models.

Current state of the software:
- `bayesmix` performs inference for mixture models of the kind

<a href="https://www.codecogs.com/eqnedit.php?latex=y_1,&space;\ldots,&space;y_n&space;\sim&space;\int&space;k(\cdot&space;\mid&space;\theta)&space;P(d\theta)" target="_blank"><img src="https://latex.codecogs.com/gif.latex?y_1,&space;\ldots,&space;y_n&space;\sim&space;\int&space;k(\cdot&space;\mid&space;\theta)&space;P(d\theta)" title="y_1, \ldots, y_n \sim \int k(\cdot \mid \theta) \Pi(d\theta)" /></a>

<a href="https://www.codecogs.com/eqnedit.php?latex=P&space;\sim&space;\Pi" target="_blank"><img src="https://latex.codecogs.com/gif.latex?P&space;\sim&space;\Pi" title="\Pi \sim P" /></a>

Where P is either the Dirichlet process or the Pitman--Yor process.

- We currently support univariate and multivariate location-scale mixture of Gaussian densities

- Inference is carried out using either Algorithm 2 or Algorithm 8 in [Neal (2000)](http://www.stat.columbia.edu/npbayes/papers/neal_sampling.pdf).

- Serialization of the MCMC chains is possible using [Google's protocol buffers](https://developers.google.com/protocol-buffers)


## Installation

### For end users

Just clone the repository with
```shell
git clone --recurse-submodule git@github.com:bayesmix-dev/bayesmix.git
```

To run the executable:
```shell
mkdir build
cd build
cmake .. -DDISABLE_DOCS -DDISABLE_BENCHMARKS -DDISABLE_TESTS
make run
cd ..
```

The executable `./build/run` can be used to perform all the necessary analysis. See the file `bash/run_test.sh` for examples of usage with command line arguments.


## For developers


We heavily depend on Google's [Protocol Buffers](https://github.com/protocolbuffers/protobuf). The CMakeLists.txt file is set up to install the library if it does not find it in the computer. However any call to `make clean` will uninstall it, causing a huge waste of time... so make sure to install it beforehand!

On Linux machine the following will install the library
```shell
sudo apt-get install autoconf automake libtool curl make g++ unzip cmake
wget https://github.com/protocolbuffers/protobuf/releases/download/v3.14.0/protobuf-python-3.14.0.zip
unzip protobuf-python-3.14.0.zip
cd protobuf-3.14.0/
./configure --prefix=/usr
make check
sudo make install
sudo ldconfig # refresh shared library cache.
```
On Mac and Windows machines, follow the official install guide ([link](https://github.com/protocolbuffers/protobuf/blob/master/src/README.md))

Another very useful tool is [`ccache`](https://ccache.dev) that significantly speed up the compilation.

Finally, to work with `bayesmix`, just clone the repository with
```shell
git clone --recurse-submodule git@github.com:bayesmix-dev/bayesmix.git
```

To run the executable:
```shell
mkdir build
cd build
cmake ..
make run
cd ..
```

To run unit tests:
```shell
cd build
cmake ..
make test_bayesmix
./test/test_bayesmix
```

## Documentation
Documentation is available at https://bayesmix.readthedocs.io.

## Contributions are welcome!
Please check out [CONTRIBUTORS.md](CONTRIBUTORS.md) for details on how to collaborate with us.
