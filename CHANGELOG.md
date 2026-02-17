## [0.1.1] - 2026-02-17

### 🚀 Features

- Add function to deal with float-point precesion
- :sparkles: add frame and algorithms for offical work
- Add global config, default construtor and ostream output for lga::Angle
- Add config reset function
- Add format function
- :sparkles: add more infomation for *Adjust function
- Add standard function for lga::Angle
- :sparkles: add closed traverse adjust
- Add angle fmt as json array
- :sparkles: add utils for boost
- Add global concepts
- Add json array format convert function
- Add concept `Iter_Like`
- Adjustment of connecting traverse
- :sparkles: add elevation net adjustment algorithm
- Add assertion to all singal route type

### 🐛 Bug Fixes

- :bookmark: update version macros
- :bug: fix pass value argument
- :bug: fix invalid output format config
- :bug: remove built in breakpoint
- Merge headers and add <concepts>
- Remove deps on merged headers
- Remove invoking deleted function

### 💼 Other

- Update auto mkdir
- Update Debug build configure
- :bug: update deps dir to global/customized install dir
- Add deps boost_json
- :construction_worker: update lib target and custom 3rd party deps
- Add target 'dev'
- Rewrite Makefile, add makehelp
- Depress warning from 3rd party lib

### 🚜 Refactor

- :fire: remove constexpr of lga::Angle
- Split to multi headers
- :bug: fix spell mistake
- Depress warning

### 📚 Documentation

- :memo: update dependencies
- Add lovely doc in Makefile
- :bulb: add doxygen comment
- :memo: add more doxygen document
- Doxygen comment in source files
- Update Makefile targets
- Add doxygen comment to Geodesy
- :memo: update doc of OfficialWork
- Update README.md and reuse it to doxygen

### ⚡ Performance

- Remove unnecessary loop

### 🎨 Styling

- Update style of function call
- Add candidate (e.g. mm, km) of DataFrame field
- Move if-else together

### 🧪 Testing

- :white_check_mark: add test for closed and attached route of elevation adjustment
- Add breakpoint macro
- Add test of hmdf::DataFrame output lga::Angle
- :white_check_mark: updata multi return of *Adjust
- Test closed traverse adjust
- :white_check_mark: update more test data for closed traverse adjust
- Update all test to catch2

### ⚙️ Miscellaneous Tasks

- :see_no_evil: update ignore local tmp files
- Test git remote
- :see_no_evil: add ignore to log file
- Ignore tmp
- :see_no_evil: ignore more log file
- Try to fix github linguist
- Ignore local dev folders
- Remove reduant item
## [0.1.0] - 2026-02-05

### 🚀 Features

- :sparkles: Sexagesimal angle calculation.
- Infrastructure of libGA
- Add errors throwed by libGA
- :sparkles: add linear algebra algorithm based on Eigen3
- :sparkles: add utilities about math
- Add global enum class represeting status
- :fire: add algorithms in photogrammetry
- :sparkles: add algorithms in geodesy
- :sparkles: add ultility for gauss project

### 💼 Other

- Add Makefile

### 🚜 Refactor

- :fire: remove include lga/impl/Config.hpp

### 📚 Documentation

- :page_facing_up: add MIT license
- Add CHANGELOG
- Add CHANGELOG for v0.1.0
- :memo: add README.md for v0.1.0

### 🧪 Testing

- :white_check_mark: add test for v0.1.0

### ⚙️ Miscellaneous Tasks

- :see_no_evil: add .gitignore
- :see_no_evil: update ignore *.toml
