#include <gtest/gtest.h>
#include "../src/mesh.hpp"
#include "../src/plasma.hpp"
#include <cmath>

TEST(MeshTest, Mesh) {
  Mesh mesh;
  // Expect equality.
  EXPECT_EQ(mesh.t, 0.0);
  EXPECT_EQ(mesh.dt, 0.01);
  EXPECT_EQ(mesh.nt, 1000);
  EXPECT_EQ(mesh.nintervals, 10);
  EXPECT_EQ(mesh.nmesh, 11);
  EXPECT_EQ(mesh.dx, 0.1);
  for(int i = 0; i < mesh.nmesh-1; i++){
  	EXPECT_EQ(mesh.mesh[i], double(i)*mesh.dx);
  }
  for(int i = 0; i < mesh.nmesh-2; i++){
  	EXPECT_EQ(mesh.mesh_staggered[i], double(i+0.5)*mesh.dx);
  }
}

TEST(MeshTest, get_index_pair) {
  Mesh mesh;
  double x;
  for(int i = 0; i < mesh.nmesh-1; i++){
	  std::cout << mesh.mesh_staggered[i] << " ";
  }
  std::cout << "\n";

  int index_down, index_up;
  int index_down_expected, index_up_expected;

  for(int i = 0; i < mesh.nmesh; i++){
	x = double(i)/double(mesh.nmesh-1);
	std::cout << x << "\n";

  	mesh.get_index_pair(x,mesh.mesh_staggered,mesh.nmesh-1,&index_down,&index_up);
  	std::cout << index_down << " " << index_up  << "\n";

	index_down_expected = i-1;
	index_up_expected = i;
	if(index_down_expected < 0){
		index_down_expected = mesh.nmesh-2;
	}
	if(index_up_expected > mesh.nmesh-2){
		index_up_expected = 0;
	}

  	EXPECT_EQ(index_down, index_down_expected);
  	EXPECT_EQ(index_up, index_up_expected);
  }
}

TEST(MeshTest, evaluate_electric_field) {
  Mesh mesh;
  //for(int i = 0; i < mesh.nmesh-1; i++){
  //	  std::cout << mesh.mesh_staggered[i] << " ";
  //}
  //std::cout << "\n";

  // mock up electric field to interpolate
  for(int i = 0; i < mesh.nmesh-1; i++){
	  mesh.electric_field_staggered[i] = double(i);
	  //std::cout << mesh.electric_field_staggered[i] << " ";
  }
  //std::cout << "\n";

  // Test selection of points:

  // below lowest point
  double x = 0.0;
  double E = mesh.evaluate_electric_field(x);
  ASSERT_NEAR(E, 4.5, 1e-8); // midpoint between 0 and 9

  x = 0.075;
  E = mesh.evaluate_electric_field(x);
  ASSERT_DOUBLE_EQ(E, 0.25); // 0.75 * 0 + 0.25 * 1

  x = 0.25;
  E = mesh.evaluate_electric_field(x);
  ASSERT_DOUBLE_EQ(E, 2); // on grid point 2

  x = 0.6;
  E = mesh.evaluate_electric_field(x);
  ASSERT_DOUBLE_EQ(E, 5.5); // midpoint between 5 and 6

  x = 0.975;
  E = mesh.evaluate_electric_field(x);
  ASSERT_NEAR(E, 6.75, 1e-8); // 0.75*9 + 0.25*0
}



TEST(MeshTest, deposit) {
  Mesh mesh;
  // Single particle plasma
  Plasma plasma(1,1.0);

  // Single particle at midpoint between first two grid points
  plasma.x[0] = 0.05;
  mesh.deposit(&plasma);
  ASSERT_NEAR(mesh.charge_density[0], 0.5, 1e-8);
  ASSERT_NEAR(mesh.charge_density[1], 0.5, 1e-8);
  for(int i = 2; i < mesh.nmesh-1; i++){
    ASSERT_NEAR(mesh.charge_density[i], 0.0, 1e-8);
  }
  ASSERT_NEAR(mesh.charge_density[mesh.nmesh-1], 0.5, 1e-8);

  double total_charge = 0.0;
  for(int i = 0; i < mesh.nmesh-1; i++){ // Skip repeat point
	  total_charge += mesh.charge_density[i];
  }
  ASSERT_NEAR(total_charge, 1.0, 1e-8);


  plasma.x[0] = 0.5;
  mesh.deposit(&plasma);
  for(int i = 0; i < mesh.nmesh; i++){
	  if(i == 5){
    		ASSERT_NEAR(mesh.charge_density[i], 1.0, 1e-8);
	  } else {
		ASSERT_NEAR(mesh.charge_density[i], 0.0, 1e-8);
	  }
  }
  total_charge = 0.0;
  for(int i = 0; i < mesh.nmesh-1; i++){ // Skip repeat point
	  total_charge += mesh.charge_density[i];
  }
  ASSERT_NEAR(total_charge, 1.0, 1e-8);

  plasma.x[0] = 0.925;
  mesh.deposit(&plasma);
  ASSERT_NEAR(mesh.charge_density[0], 0.25, 1e-8);
  for(int i = 1; i < mesh.nmesh-2; i++){
	ASSERT_NEAR(mesh.charge_density[i], 0.0, 1e-8);
  }
  ASSERT_NEAR(mesh.charge_density[mesh.nmesh-2], 0.75, 1e-8);
  ASSERT_NEAR(mesh.charge_density[mesh.nmesh-1], 0.25, 1e-8);
  total_charge = 0.0;
  for(int i = 0; i < mesh.nmesh-1; i++){ // Skip repeat point
	  total_charge += mesh.charge_density[i];
  }
  ASSERT_NEAR(total_charge, 1.0, 1e-8);

  // Two particle plasma
  Plasma plasma2(2,1.0);

  // Single particle at midpoint between first two grid points
  plasma2.x[0] = 0.05;
  plasma2.x[1] = 0.1;
  mesh.deposit(&plasma2);
  ASSERT_NEAR(mesh.charge_density[0], 0.25, 1e-8);
  ASSERT_NEAR(mesh.charge_density[1], 0.75, 1e-8);
  for(int i = 2; i < mesh.nmesh-1; i++){
    ASSERT_NEAR(mesh.charge_density[i], 0.0, 1e-8);
  }
  ASSERT_NEAR(mesh.charge_density[mesh.nmesh-1], 0.25, 1e-8);

  total_charge = 0.0;
  for(int i = 0; i < mesh.nmesh-1; i++){ // Skip repeat point
	  total_charge += mesh.charge_density[i];
  }
  ASSERT_NEAR(total_charge, 1.0, 1e-8);

}

TEST(MeshTest, get_electric_field) {
  Mesh mesh;

  for(int i = 0; i < mesh.nmesh; i++){
  	  mesh.potential[i] = 0.0;
  }
  mesh.get_electric_field();
  for(int i = 0; i < mesh.nmesh-1; i++){
    ASSERT_NEAR(mesh.electric_field_staggered[i], 0.0, 1e-8);
  }

  for(int i = 0; i < mesh.nmesh; i++){
  	  mesh.potential[i] = double(i);
  }
  mesh.get_electric_field();
  for(int i = 0; i < mesh.nmesh-1; i++){
    ASSERT_NEAR(mesh.electric_field_staggered[i], -1.0/mesh.dx, 1e-8);
  }

  for(int i = 0; i < mesh.nmesh; i++){
  	  mesh.potential[i] = double(i*i);
  }
  mesh.get_electric_field();
  for(int i = 0; i < mesh.nmesh-1; i++){
	  ASSERT_NEAR(mesh.electric_field_staggered[i], -double(2.0*i+1)/mesh.dx, 1e-8);
  }
}

TEST(MeshTest, solve) {
  Mesh mesh;

  // Zero charge density
  // d^2 u / dx^2 = 1
  // u = x(x-1)/2 = x^2/2 - x/2
  // to satisfy u(0) = u(1) = 0
  for(int i = 0; i < mesh.nmesh; i++){
  	  mesh.charge_density[i] = 0.0;
  	  std::cout << mesh.charge_density[i] << " ";
  }
  std::cout << "\n";

  mesh.solve_for_potential();

  double x;
  for(int i = 0; i < mesh.nmesh; i++){
	x = mesh.mesh[i];
	//std::cout << mesh.potential[i] << " " << 0.5*x*(x-1.0) << "\n";
	ASSERT_NEAR(mesh.potential[i], 0.5*x*(x-1.0), 1e-8);
  }

  // Sine charge density
  // d^2 u / dx^2 = 1 - sin(2*pi*x)
  // u = sin(2*pi*x)/(4 pi**2) + x(x-1)/2 = sin(2*pi*x)/(4 pi**2) + x^2/2 - x/2
  // to satisfy u(0) = u(1) = 0
  for(int i = 0; i < mesh.nmesh; i++){
  	  mesh.charge_density[i] = sin(2.0*M_PI*mesh.mesh[i]);
  	  std::cout << mesh.charge_density[i] << " ";
  }
  std::cout << "\n";

  mesh.solve_for_potential();

  for(int i = 0; i < mesh.nmesh; i++){
	x = mesh.mesh[i];
	//std::cout << mesh.potential[i] << " " << 0.5*x*(x-1.0) + sin(2.0*M_PI*x)/(4.0*M_PI*M_PI)<< "\n";
	ASSERT_NEAR(mesh.potential[i], 0.5*x*(x-1.0) + sin(2.0*M_PI*x)/(4.0*M_PI*M_PI), 1e-3);
  }

}