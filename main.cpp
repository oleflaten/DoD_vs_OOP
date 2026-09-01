#include <iostream>
#include <vector>
#include <chrono>
#include <memory>

constexpr size_t ENTITY_COUNT = 100000; //1'000'000;
constexpr int ITERATIONS = 25;

// ==========================================
// 1. OOP Approach (Array of Structures / Polymorphism)
// ==========================================


// Base class
class Entity {
public:
    virtual ~Entity() = default;
    virtual void update(float dt) = 0;          // just addition and multiplication
    virtual void update2(float dt) = 0;         // more to calculate (sqrt) in the function
};

// Derived class with local data members
class Particle1 : public Entity {
public:
    float x{1.0f}, y{2.0f}, z{3.0f};
    float vx{0.1f}, vy{0.2f}, vz{0.3f};
    void update(float dt) override {
        x += vx * dt;
        y += vy * dt;
        z += vz * dt;
    }

    // this version requires much more compute,
    // and is intended to stress the CPU more and give a better
    // idea of the performance difference between OOP and DoD
    void update2(float dt) override {
        // 1. Calculate speed (magnitude of velocity vector)
        float speed = std::sqrt(vx * vx + vy * vy + vz * vz);

        // 2. Compute inverse speed (avoids 3 separate floating-point divisions)
        float invSpeed = 1.0f / (speed + 0.0001f);

        // 3. Update position using normalized direction vector
        x += (vx * invSpeed) * dt;
        y += (vy * invSpeed) * dt;
        z += (vz * invSpeed) * dt;
    }
};

// Helper struct for the Particle2 implementation
struct Vec3
{
    float x;
    float y;
    float z;
};


// Derived class with pointers to its data members
class Particle2 : public Entity {
public:
    Particle2()
    {
        mPosition = new Vec3{1.0f, 2.0f, 3.0f};
        mVelosity = new Vec3{0.1f, 0.2f, 0.3f};
    }

    ~Particle2()
    {
        delete mPosition;
        delete mVelosity;
    }

    Vec3 *mPosition;
    Vec3 *mVelosity;

    void update(float dt) override {
        mPosition->x += mVelosity->x * dt;
        mPosition->y += mVelosity->y * dt;
        mPosition->z += mVelosity->z * dt;
    }

    // this version requires much more compute,
    // and is intended to stress the CPU more and give a better
    // idea of the performance difference between OOP and DoD
    void update2(float dt) override {
        // Calculate speed (velocity magnitude)
        float speed = std::sqrt(mVelosity->x * mVelosity->x +
                                mVelosity->y * mVelosity->y +
                                mVelosity->z * mVelosity->z);

        // Prevent division by zero if velocity is zero
        float invSpeed = 1.0f / (speed + 0.0001f);

        // Update positions using normalized velocity directions
        mPosition->x += (mVelosity->x * invSpeed) * dt;
        mPosition->y += (mVelosity->y * invSpeed) * dt;
        mPosition->z += (mVelosity->z * invSpeed) * dt;
    }
};

// ==========================================
// 2. DoD Approach (Structure of Arrays / Data Locality)
// ==========================================

// This is the Struct with Arrays
// It of course have Arrays with local data members
struct ParticleSystemDoD {
    std::vector<float> x, y, z;
    std::vector<float> vx, vy, vz;

    void resize(size_t size) {
        x.resize(size, 1.0f); y.resize(size, 2.0f); z.resize(size, 3.0f);
        vx.resize(size, 0.1f); vy.resize(size, 0.2f); vz.resize(size, 0.3f);
    }

    void update(float dt) {
        const size_t size = x.size();
        for (size_t i = 0; i < size; ++i) {
            x[i] += vx[i] * dt;
            y[i] += vy[i] * dt;
            z[i] += vz[i] * dt;
        }
    }

    // Heavier compute loop: higher ratio of math operations per byte loaded
    // This should stress the CPU more and give a better idea of the performance difference between OOP and DoD
    void update2(float dt) {
        const size_t size = x.size();
        for (size_t i = 0; i < size; ++i) {
            // Trigonometric or polynomial math keeps SIMD units busy
            float speed = std::sqrt(vx[i] * vx[i] + vy[i] * vy[i] + vz[i] * vz[i]);
            x[i] += (vx[i] / (speed + 0.0001f)) * dt;
            y[i] += (vy[i] / (speed + 0.0001f)) * dt;
            z[i] += (vz[i] / (speed + 0.0001f)) * dt;
        }
    }
};


int main() {
    // --------------------------------------------------
    // Setup OOP Data and using polymorphy to call update()
    // --------------------------------------------------

    std::vector<std::unique_ptr<Entity>> oopEntities;
    oopEntities.reserve(ENTITY_COUNT);
    for (size_t i = 0; i < ENTITY_COUNT; ++i) {
        oopEntities.push_back(std::make_unique<Particle1>());//                  <<<<   Particle1 version
    }

    // Warm-up cache
    // (not sure this is necessary, but the idea is to get the program started and
    // have less of the main program initialization overhead in our timed test)
    for (auto& entity : oopEntities)
        entity->update2(0.016f); //                                             <<<< test update() and update2()

    // Benchmark OOP
    auto startOOP1 = std::chrono::steady_clock::now();
    for (int iter = 0; iter < ITERATIONS; ++iter) {
        for (auto& entity : oopEntities) {
            entity->update2(0.016f);//                                          <<<< test update() and update2()
        }
    }
    auto endOOP1 = std::chrono::steady_clock::now();


    // OOP version with pointers to the internal data members:
    // Clear existing unique_ptrs (resets size to 0, keeps capacity)
    oopEntities.clear();
    // Repopulate with Particle2 version
    for (size_t i = 0; i < ENTITY_COUNT; ++i) {
        oopEntities.push_back(std::make_unique<Particle2>());//                  <<<<   Particle2 version
    }

    // Warm-up cache
    for (auto& entity : oopEntities)
        entity->update2(0.016f);//                                             <<<< test update() and update2()


    // Benchmark OOP
    auto startOOP2 = std::chrono::steady_clock::now();
    for (int iter = 0; iter < ITERATIONS; ++iter) {
        for (auto& entity : oopEntities) {
            entity->update2(0.016f);//                                         <<<< test update() and update2()
        }
    }
    auto endOOP2 = std::chrono::steady_clock::now();

    // --------------------------------------------------
    // Setup DoD Data
    // --------------------------------------------------

    // Making one struct that contains all the arrays!
    ParticleSystemDoD dodSystem;
    dodSystem.resize(ENTITY_COUNT);

    // Warm-up cache
    dodSystem.update2(0.016f);//                                         <<<< test update() and update2()

    // Benchmark DoD
    auto startDoD = std::chrono::steady_clock::now();
    for (int iter = 0; iter < ITERATIONS; ++iter) {
        dodSystem.update2(0.016f);//                                     <<<< test update() and update2()
    }
    auto endDoD = std::chrono::steady_clock::now();

    // --------------------------------------------------
    // Print Results
    // --------------------------------------------------
    std::chrono::duration<double, std::milli> oopDuration1 = endOOP1 - startOOP1;   //Particle1 version
    std::chrono::duration<double, std::milli> oopDuration2 = endOOP2 - startOOP2;   //Particle2 version
    std::chrono::duration<double, std::milli> dodDuration = endDoD - startDoD;

    std::cout << "\nNB: ONLY VALID IF COMPILED IN RELEASE MODE!!! \n\n";
    std::cout << "--- Benchmark Results (" << ENTITY_COUNT << " entities, " << ITERATIONS << " runs) ---\n";
    std::cout << "OOP1 Time: " << oopDuration1.count() << " ms (local data members)\n";
    std::cout << "OOP2 Time: " << oopDuration2.count() << " ms (pointer to data members)\n";
    std::cout << "DoD Time: " << dodDuration.count() << " ms\n";

    float result = oopDuration1.count() / dodDuration.count();
    std::cout << "---\nDoD speedup over Particle1 version:  " << result;
    if (result < 1.0)
        std::cout << " x slower\n";
    else
        std::cout << " x faster\n";

    result = oopDuration2.count() / dodDuration.count();
    std::cout << "DoD speedup over Particle2 version:  " << result;
    if (result < 1.0)
        std::cout << " x slower\n";
    else
        std::cout << " x faster\n";

    std::cout << "\n";

    return 0;
}