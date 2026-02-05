# libGA

Geomatics Algorithm Library (libGA) is a comprehensive C++ toolkit for geomatics computations ranging from geodesy to photogrammetry. It's still **in developing**.

## Core Features

- **Geodetic Computations**: Ellipsoid modeling, latitude/longitude handling, geodetic forward/inverse problem solving, meridian arc length calculations
- **Coordinate Transformations**: Gauss-Kruger projections, coordinate system conversions
- **Photogrammetry Support**: Interior/exterior orientation, space resection/intersection algorithms, collinearity condition equations
- **Mathematical Functions**: Specialized geodetic functions, trigonometric operations, cubic spline interpolation
- **Linear Algebra**: An easy-to-use wrap of eigen3 for geomatics
- **Angle Handling**: Comprehensive angle class with multiple unit support (radians, degrees, DMS)

## Examples

### Basic Angle Operations
```cpp
#include <lga/Angle>

lga::Angle angle = lga::Angle::fromDegrees(45.0);
double radians = angle.toRadian();
auto [degrees, minutes, seconds] = angle.toDMS(); // Extract DMS components

// Trigonometric functions
double sin_val = angle.sin();
double cos_val = angle.cos();
```

### Geodetic Calculations
```cpp
#include <lga/Geodesy>

// Define coordinates
lga::Latitude B1(lga::deg2rad(47.781291)); // 47°46'52.647"N
lga::Longitude L1(lga::deg2rad(35.826758)); // 35°49'36.330"E

// Perform geodetic forward calculation using Bessel formula
lga::Geodetic_Forward_Solve_Result result = lga::bassel_formula_solve(
    B1, L1,                    // Starting point
    44797.2826,               // Distance (meters)
    lga::Angle::fromDMS(44, 12, 13.664), // Azimuth
    lga::krasovski             // Reference ellipsoid
);
```

### Gauss-Kruger Projection
```cpp
#include <lga/Geodesy>

// Convert geodetic coordinates to Gauss-Kruger projection
lga::Geodetic_Coordinate geo_coord{
    .lat = lga::Latitude(lga::deg2rad(45.0)),
    .lon = lga::Longitude(lga::deg2rad(115.0))
};

lga::Gauss_Project_Coordinate proj_coord = lga::gauss_project(
    geo_coord,
    lga::param::geodesy.gauss_project_interval.six, // 6-degree zones
    lga::cgcs2000                                   // Reference ellipsoid
);

// Convert back from projection to geodetic coordinates
lga::Geodetic_Coordinate geo_back = lga::gauss_project(
    proj_coord,
    lga::cgcs2000
);
```

### Photogrammetry - Space Resection
```cpp
#include <lga/Photogrammetry>

// Define interior orientation parameters
lga::Interior interior{
    .x = 0,           // Principal point x (mm, converted to meters)
    .y = 0,           // Principal point y (mm, converted to meters)
    .f = 153.24/1000, // Focal length (mm, converted to meters)
    .m = 50000        // Scale denominator
};

// Image coordinates (rows: points, cols: x,y in meters)
lga::Matrix img_points(4, 2);
// Object coordinates (rows: points, cols: X,Y,Z in meters)
lga::Matrix obj_points(4, 3);

// Solve for exterior orientation parameters
lga::Space_Resection_Result result = lga::spaceResection(
    interior,      // Interior orientation
    img_points,    // Image coordinates
    obj_points     // Ground coordinates
);
```

### Photogrammetry - Space Intersection
```cpp
#include <lga/Photogrammetry>

// Define left and right image metadata
lga::Image_Meta_Data left_meta{left_exterior, left_interior};
lga::Image_Meta_Data right_meta{right_exterior, right_interior};

// Perform space intersection to compute ground coordinates
lga::Matrix ground_coords = lga::spaceIntersection(
    left_meta,          // Left image metadata
    left_image_points,  // Left image coordinates
    right_meta,         // Right image metadata
    right_image_points  // Right image coordinates
);
```

### Curvature Radius Calculations
```cpp
#include <lga/Geodesy>

lga::Latitude lat(lga::deg2rad(45.0)); // 45 degrees North
auto curvature_radii = lga::principleCurvatureRadius(lat, lga::krasovski);

std::cout << "Meridian radius: " << curvature_radii.m << " m" << std::endl;
std::cout << "Prime vertical radius: " << curvature_radii.n << " m" << std::endl;
std::cout << "Mean radius: " << lga::meanCurvatureRadius(curvature_radii) << " m" << std::endl;
```

## Supported Ellipsoids

The library includes predefined reference ellipsoids:
- `lga::krasovski` - Krasovsky ellipsoid
- `lga::ie1975` - International Earth 1975 ellipsoid
- `lga::wgs84` - WGS84 ellipsoid
- `lga::cgcs2000` - China Geodetic Coordinate System 2000 ellipsoid

## Dependencies

- eigen 5.0.0
- catch2 (optional for test)

## Install

By default, it only builds dynamic library (shared object):
```bash
make lib
```
For extra static lib:
```bash
make lib STATIC_LIB=1
```

## Usage

The library provides namespaces and classes organized for geospatial computations:
- header:
    - `<lga/Angle>` - Angle handling and conversions
    - `<lga/Linalg>` - Linear algebra operations using Eigen
    - `<lga/Math>` - Uitilies in mathmatics
    - `<lga/Photogrammetry>` - Photogrammetric computations
    - `<lga/Geodesy>` - Geodetic calculations and coordinate systems
- namespace:
    - `lga` - Core namespace
    - `lga::config` - Globally configure behaviour of some algorithms
    - `lga::param` - Pre-defined object as input to avoid stupid mistake
- rules of naming:
    - `Gauss_Projector`: class
    - `gauss_project`: object
    - gauss_project.`meridianConvergence`: function or member function
    - `M_libga_geodesy`: private macros
    - `LGA_HEADER_ONLY`: public macros

## Configuration Options

The library supports several compile-time configuration options:
- `LGA_HEADER_ONLY` - Enable header-only compilation
- `LGA_DEBUG` - Enable debug output
- `LGA_HIGH_PRECISION`(experimantal) - Enable high precision calculations