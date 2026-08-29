## DoD vs OOP

*You MUST compile this code in RELEASE mode to evaluate the timings*
---
Small test to see speed benefits one could get from DoD vs OOP:
* hoping to get the cache work for us
* see SIMD get to work

Two versions of OOP are implemented.
Both inherit from a base class, and use it's pure virtual function.
* The first has its data members local (float x{1.0f}, y{2.0f}, z{3.0f}; )
* The second has a pointer to a Vec3 (Vec3 *mPosition; )

The OOP versions are then used in an AoS setup, 
    and inside a for-loop, call the virtual function

The DoD version is a straight-up SoA version, 
    and inside the for-loop, calls a local function.
