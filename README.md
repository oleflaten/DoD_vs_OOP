## DoD vs OOP

*You MUST compile this code in RELEASE mode to evaluate the timings*
---
Small test to see speed benefits one could get from DoD vs OOP:
* hoping to get the cache work for us
* see SIMD get to work

Two versions of OOP are implemented.
Both inherit from a base class, and use its pure virtual function.
* The first has its data members local (float x{1.0f}, y{2.0f}, z{3.0f}; )
* The second has a pointer to a Vec3 (Vec3 *mPosition; )

The OOP version is then used in a AoS setup, 
    and in a for loop, calling the virtual function

The DoD version is straight up SoA version, 
    and in the for loop, calling a local function.