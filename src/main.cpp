#include "main.hpp"
#include "plasma.hpp"
#include "mesh.hpp"
#if __has_include(<SYCL/sycl.hpp>)
#include <SYCL/sycl.hpp>
#else
#include <CL/sycl.hpp>
#endif

class hello_world;

int main() {
  auto defaultQueue = sycl::queue{};

  defaultQueue
      .submit([&](sycl::handler& cgh) {
        auto os = sycl::stream{128, 128, cgh};

        cgh.single_task<hello_world>([=]() { os << "Hello World!\n"; });
      })
      .wait();

  //initialize();
  // Initialize by calling Mesh and Particle constructors
  Mesh mesh;
  Plasma plasma;
  evolve(&mesh,&plasma);
  
  return 0;
};

/*
 * Initialize all code components
 */
//void initialize() {

  // TODO: [Set input parameters]

//};

/*
 * Evolve simulation through all timesteps
 */
void evolve(Mesh *mesh, Plasma *plasma) {

  for (int i = 0; i < mesh->nt; i++) {
    plasma->push(mesh);
    mesh->deposit(plasma);
    mesh->solve(plasma);
  };
};