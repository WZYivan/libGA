# libGA - Geomatics Algorithm Library

**libGA** (Geomatics Algorithm Library) is a comprehensive C++ toolkit for geomatics computations, ranging from geodesy to photogrammetry. It provides high-precision algorithms for surveying, mapping, and geospatial analysis.

> **Status**: Currently in active development (v0.1.1)

## Table of Contents

- [Features](#features)
- [Modules](#modules)
- [Dependencies](#dependencies)
- [Build & Installation](#build--installation)
- [Usage Examples](#usage-examples)
- [Testing](#testing)
- [Documentation](#documentation)
- [Apologize](#apologize)

## Features

### Geodetic Computations
- **Ellipsoid Modeling**: Support for multiple reference ellipsoids (Krassovsky, IAG-1975, WGS-84, CGCS2000)
- **Geodetic Forward/Inverse Problems**: Bessel formula and Gauss lemma solvers
- **Meridian Arc Length**: Precise calculations with iterative bottom-solving
- **Gauss-Krüger Projection**: Forward/inverse projection with zone management
- **Coordinate Systems**: Latitude/Longitude handling with validation

### Coordinate Transformations
- **Gauss-Krüger Projection**: 3° and 6° zone support
- **Meridian Convergence**: Direction correction calculations
- **Distance Correction**: Scale factor computations
- **Zone Transformation**: Cross-zone coordinate conversion

### Photogrammetry
- **Interior Orientation**: Camera calibration parameters (focal length, principal point, scale)
- **Exterior Orientation**: Position (X, Y, Z) and attitude (ω, φ, κ) elements
- **Space Resection**: Iterative solution using collinearity condition equations
- **Space Intersection**: Single-pair and multi-image OLS solutions
- **Rotation Matrices**: X, Y, Z axis rotations with various simplification strategies

### Official Work Adjustment
- **Elevation Adjustment**: Closed and attached route adjustments
- **Traverse Adjustment**: Closed and connecting traverse computations
- **Network Adjustment**: Elevation network adjustment using graph theory
- **Tolerance Checking**: Built-in tolerance validation (40√L, 60√N rules)
- **DataFrame Output**: Results in `hmdf::DataFrame` format for easy analysis

### Linear Algebra (Eigen3 Wrapper)
- **Matrix Operations**: Normalized equations, OLS solving
- **Matrix Inversion**: Cholesky and SVD-based methods
- **Coordinate Transformations**: Translation, rotation, scaling
- **RMSE Calculation**: Root mean square error computations
- **Error Propagation**: Error matrix calculations

### Mathematical Utilities
- **Angle Handling**: Comprehensive `lga::Angle` class with DMS (Degrees-Minutes-Seconds) support
- **Unit Conversion**: Radians, degrees, minutes, seconds
- **Trigonometric Functions**: sin, cos, tan, sec, csc, cot
- **String Parsing**: Regex-based angle string parsing
- **Special Functions**: Sine, Double-Gaussian, Cubic Spline

### Boost Integration
- **JSON Support**: Boost.JSON serialization/deserialization
- **Graph Theory**: Network adjustment using Boost.Graph
- **Property Maps**: Vertex/edge property management
- **BFS Traversal**: Graph traversal visitors

## Modules

| Module | Header | Description |
|--------|--------|-------------|
| **Angle** | `<lga/Angle>` | Sexagesimal angle calculations |
| **Geodesy** | `<lga/Geodesy>` | Geodetic algorithms |
| **Photogrammetry** | `<lga/Photogrammetry>` | Photogrammetric computations |
| **OfficialWork** | `<lga/OfficialWork>` | Surveying adjustment algorithms |
| **Linalg** | `<lga/Linalg>` | Linear algebra utilities |
| **Math** | `<lga/Math>` | Mathematical utilities |
| **BoostUtils** | `<lga/BoostUtils>` | Boost library integration |
| **Base** | `<lga/Base>` | Core definitions and concepts |
| **Error** | `<lga/Error>` | Exception classes |

## Dependencies

| Library | Version | Usage | Link Type |
|---------|---------|-------|-----------|
| [Eigen3](https://libeigen.gitlab.io/) | 3.5.0 | Linear algebra | Header-only |
| [DataFrame](https://github.com/hosseinmoein/DataFrame) | 3.7.0 | Data frame operations | Static/Shared |
| [oneTBB](https://github.com/uxlfoundation/oneTBB) | 2022.3.0 | Parallel computing (DataFrame dependency) | Static/Shared |
| [Boost](https://www.boost.org/) | 1.90.0 | JSON, Graph | Static/Shared |
| [Catch2](https://github.com/catchorg/Catch2) | Latest | Testing (optional) | Static/Shared |

## Build & Installation

### Prerequisites

- C++23 compatible compiler (GCC 14+, others not sure)
- CMake or Make
- All dependencies installed

### Configuration

The `Makefile` supports the following configuration variables:

| Variable | Default | Description |
|----------|---------|-------------|
| `COMPILER` | `g++` | C++ compiler |
| `VERSION` | `23` | C++ standard version |
| `BUILD_TYPE` | `Debug` | `Debug` or `Release` |
| `OPTIMIZE` | `2` | Optimization level (Release mode) |
| `DEPS_LINK_TYPE` | `Dynamic` | `Dynamic` or `Static` linking |
| `THIRD_PARTY_ROOT` | Required | Path to third-party libraries |

### Build Commands

```bash
# Build shared library (libga.so)
make shared-lib BUILD_TYPE=Release THIRD_PARTY_ROOT="/path/to/libs" -j$(nproc)

# Build static library (libga_static.a)
make static-lib BUILD_TYPE=Release THIRD_PARTY_ROOT="/path/to/libs" -j$(nproc)

# Build test executables
make test-exe BUILD_TYPE=Debug THIRD_PARTY_ROOT="/path/to/libs" -j$(nproc)

# Run all tests
make run

# Clean build artifacts
make clean

# Show help
make help
```

### Environment Variables

Set the following environment variables for custom dependency paths:

```bash
export EIGEN3_ROOT="/path/to/eigen3"
export DATAFRAME_ROOT="/path/to/dataframe"
export ONETBB_ROOT="/path/to/onetbb"
export BOOST_ROOT="/path/to/boost"
export CATCH2_ROOT="/path/to/catch2"
```

## Usage Examples

### Angle Operations

```cpp
#include <lga/Angle>

// Create angle from DMS
lga::Angle angle = lga::Angle::fromDMS(45, 30, 30, false);  // 45°30'30"

// Convert to different units
double radians = angle.toRadian();
double degrees = angle.toDegrees();
double minutes = angle.toMinutes();
double seconds = angle.toSeconds();

// Trigonometric functions
double sin_val = angle.sin();
double cos_val = angle.cos();

// Angle arithmetic
lga::Angle sum = angle1 + angle2;
lga::Angle diff = angle1 - angle2;
lga::Angle scaled = angle * 2.0;

// String formatting
std::string str = angle.toString();  // Default: "045d30m30.000s"
std::string custom = angle.toString("{:1s}{:03d}.{:02d}{:02.0f}");  // "-045.3030"
```

### Geodetic Computations

```cpp
#include <lga/Geodesy>

// Define coordinates
lga::Latitude B1(lga::dms2rad(47, 46, 52.647));
lga::Longitude L1(lga::dms2rad(35, 49, 36.330));
lga::Angle A1(lga::dms2rad(44, 12, 13.664));
double distance = 44797.2826;

// Forward problem: compute destination point
lga::Geodetic_Forward_Solve_Result result = 
    lga::bessel_formula_solve(B1, L1, distance, A1, lga::krassovsky);

// Inverse problem: compute distance and azimuth
lga::Geodetic_Inverse_Solve_Result inverse = 
    lga::bessel_formula_solve(B1, L1, result.lat, result.lon, lga::krassovsky);

// Gauss-Krüger projection
lga::Geodetic_Coordinate gc(B1, L1);
lga::Gauss_Project_Coordinate gpc = 
    lga::gauss_project(gc, lga::param::geodesy.gauss_project_interval.six, lga::cgcs2000);
```

### Photogrammetry

```cpp
#include <lga/Photogrammetry>

// Interior orientation
lga::Interior interior{
    .x = 0,
    .y = 0,
    .f = 0.15324,  // focal length in meters
    .m = 50000     // image scale
};

// Exterior orientation
lga::Exterior exterior{
    .x = 4999.77, .y = 4999.73, .z = 2000.00,
    .phi = 0.000215, .omega = 0.029064, .kappa = 0.095247
};

// Space resection
lga::Matrix img_coords = ...;  // Image coordinates (n x 2)
lga::Matrix obj_coords = ...;  // Object coordinates (n x 3)

lga::Space_Resection_Result resection = 
    lga::spaceResection(interior, img_coords, obj_coords);

// Space intersection
lga::Image_Meta_Data left_meta{exterior_left, interior_left};
lga::Image_Meta_Data right_meta{exterior_right, interior_right};

lga::Matrix obj_point = lga::spaceIntersection(
    left_meta, left_img_coords,
    right_meta, right_img_coords
);
```

### Surveying Adjustment

```cpp
#include <lga/OfficialWork>

// Closed elevation adjustment
std::vector<double> distances = {0.8, 0.5, 1.2, 0.5, 1.0};
std::vector<double> diff = {0.23, 0.26, -0.55, -0.45, 0.49};
double benchmark_elev = 12.0;

lga::Adjust_Frame_Result result = lga::closedElevAdjust(
    distances, diff, benchmark_elev
);

// Connecting traverse adjustment
std::vector<lga::Angle> left_angles = {
    {99, 1, 0}, {167, 45, 36}, {123, 11, 24}
};
std::vector<double> dists = {225.85, 139.03, 172.57};
lga::Angle azimuth_start{237, 59, 30};
lga::Angle azimuth_end{46, 45, 24};

lga::Adjust_Frame_Result traverse = lga::connectingTraverseAdjust(
    left_angles, dists,
    azimuth_start, azimuth_end,
    x_start, y_start, x_end, y_end
);

// Elevation network adjustment
using Net = lga::Elev_Net;
Net net;

// Add vertices and edges
auto A = boost::add_vertex(lga::Elev_Net_Vertex_Ds{"A", 237.483, true}, net);
auto B = boost::add_vertex(lga::Elev_Net_Vertex_Ds{"B"}, net);
boost::add_edge(A, B, lga::Elev_Net_Edge_Ds{"1", 5.835, 3.5}, net);

auto result = lga::netAdjust(net);
```

### Linear Algebra

```cpp
#include <lga/Linalg>

// Matrix operations
lga::Matrix A = ...;
lga::Matrix L = ...;
lga::Matrix P = ...;  // Weight matrix (optional)

// Ordinary Least Squares
lga::Matrix X = lga::ols(A, L, P);

// Matrix inversion
lga::Matrix inv_A = lga::choleskyInverse(A);
lga::Matrix inv_svd = lga::svdInverse(A);

// Coordinate transformations
lga::Matrix coords = ...;
lga::Matrix translated = lga::translate(coords, dx, dy, dz);
lga::Matrix rotated = lga::rotateForward(coords, rotation_matrix);
lga::Matrix scaled = lga::scale(coords, scale_factor);

// RMSE calculation
double rmse_val = lga::rmse(residuals, n_observations, t_parameters);
```

## Testing

The project uses [Catch2](https://github.com/catchorg/Catch2) for unit testing. Test files are located in the `test/` directory.

### Available Tests

| Test File | Module | Description |
|-----------|--------|-------------|
| `Angle.cpp` | Angle | Angle construction, conversion, trigonometry |
| `BasselFormula.cpp` | Geodesy | Bessel formula forward/inverse solving |
| `GaussProject.cpp` | Geodesy | Gauss-Krüger projection accuracy |
| `SpaceResection.cpp` | Photogrammetry | Space resection algorithm |
| `SpaceIntersection.cpp` | Photogrammetry | Space intersection (single & OLS) |
| `ClosedElevAdjust.cpp` | OfficialWork | Closed elevation route |
| `AttachedElevAdjust.cpp` | OfficialWork | Attached elevation route |
| `ClosedTraverseAdjust.cpp` | OfficialWork | Closed traverse adjustment |
| `ConnectingTraverseAdjust.cpp` | OfficialWork | Connecting traverse adjustment |
| `ElevNetAdjust.cpp` | OfficialWork | Elevation network adjustment |

### Run Tests

```bash
# Build and run all tests
make test-exe BUILD_TYPE=Debug
make run

# Run individual test
./test/bin/Angle
./test/bin/SpaceResection
```

## Documentation

### Doxygen Documentation

This README serves as the main page for the Doxygen documentation. Generate HTML documentation:

```bash
doxygen Doxyfile
```

Output will be in `html/` directory. Open `html/index.html` in a browser.

### Key Design Patterns

- **Header-only Implementation**: Use `#define LGA_HEADER_ONLY` before including headers
- **Configuration System**: Runtime configuration via `lga::config::` namespace
- **Predefined Parameters**: Access via `lga::param::` namespace
- **Error Handling**: Exceptions derived from `std::exception`
- **Modern C++**: C++23 features (concepts, ranges, format)

## Apologize

I use AI to generate this README, but I've checked it.

---

**Version**: 0.1.1  
**Last Updated**: 2026-02-17  
**Author**: WZYivan (2270060975@qq.com)  
**Repository**: [libGA](https://github.com/WZYivan/libGA)
