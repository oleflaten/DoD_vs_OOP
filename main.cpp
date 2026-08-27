#include <iostream>
#include <vector>
#include <chrono>
#include <memory>

// Prevent compiler from optimizing away unused variables
// template <typename T>
// void doNotOptimize(T&& value) {
    // asm volatile("" : : "r,m"(value) : "memory");
// }

constexpr size_t ENTITY_COUNT = 100;   //1'000'000;
constexpr int ITERATIONS = 1;

// ==========================================
// 1. OOP Approach (Array of Structures / Polymorphism)
// ==========================================

struct Vec3
{
    float x;
    float y;
    float z;
};

class Entity {
public:
    virtual ~Entity() = default;
    virtual void update(float dt) = 0;
};

class Particle : public Entity {
public:
    float x{1.0f}, y{2.0f}, z{3.0f};
    float vx{0.1f}, vy{0.2f}, vz{0.3f};
    void update(float dt) override {
        x += vx * dt;
        y += vy * dt;
        z += vz * dt;
    }
};

class Particle2 : public Entity {
public:
    Particle2()
    {
        mPosition = new Vec3{1.0f, 2.0f, 3.0f};
        mVelosity = new Vec3{0.1f, 0.2f, 0.3f};
    }

    Vec3 *mPosition;
    Vec3 *mVelosity;

    void update(float dt) override {
        mPosition->x += mVelosity->x * dt;
        mPosition->y += mVelosity->y * dt;
        mPosition->z += mVelosity->z * dt;
    }
};

// ==========================================
// 2. DoD Approach (Structure of Arrays / Data Locality)
// ==========================================
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
};

int main() {
    // --------------------------------------------------
    // Setup OOP Data
    // --------------------------------------------------
    std::vector<std::unique_ptr<Entity>> oopEntities;
    oopEntities.reserve(ENTITY_COUNT);
    for (size_t i = 0; i < ENTITY_COUNT; ++i) {
        oopEntities.push_back(std::make_unique<Particle2>()); // also test Particle2 here
    }

    // Warm-up cache (not sure this is necessary...)
    for (auto& entity : oopEntities) entity->update(0.016f);

    // Benchmark OOP
    auto startOOP = std::chrono::steady_clock::now();
    for (int iter = 0; iter < ITERATIONS; ++iter) {
        for (auto& entity : oopEntities) {
            entity->update(0.016f);
        }
    }
    auto endOOP = std::chrono::steady_clock::now();
    //doNotOptimize(oopEntities[0].get()); // Force compiler to evaluate

    // --------------------------------------------------
    // Setup DoD Data
    // --------------------------------------------------
    ParticleSystemDoD dodSystem;
    dodSystem.resize(ENTITY_COUNT);

    // Warm-up cache
    dodSystem.update(0.016f);

    // Benchmark DoD
    auto startDoD = std::chrono::steady_clock::now();
    for (int iter = 0; iter < ITERATIONS; ++iter) {
        dodSystem.update(0.016f);
    }
    auto endDoD = std::chrono::steady_clock::now();
    // doNotOptimize(dodSystem.x[0]); // Force compiler to evaluate

    // --------------------------------------------------
    // Print Results
    // --------------------------------------------------
    std::chrono::duration<double, std::milli> oopDuration = endOOP - startOOP;
    std::chrono::duration<double, std::milli> dodDuration = endDoD - startDoD;

    std::cout << "--- Benchmark Results (" << ENTITY_COUNT << " entities, " << ITERATIONS << " runs) ---\n";
    std::cout << "OOP Time: " << oopDuration.count() << " ms\n";
    std::cout << "DoD Time: " << dodDuration.count() << " ms\n";
    float result = oopDuration.count() / dodDuration.count();
    std::cout << "---\nDoD speedup:  " << oopDuration.count() / dodDuration.count();
    if (result < 1.0)
        std::cout << " x slower\n";
    else
        std::cout << " x faster\n";

    return 0;
}