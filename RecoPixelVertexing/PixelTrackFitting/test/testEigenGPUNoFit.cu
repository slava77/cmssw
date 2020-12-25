#include "test_common.h"
#include <iostream>

#include <Eigen/Core>
#include <Eigen/Eigenvalues>


using namespace Eigen;

__host__ __device__ void eigenValues(Matrix3d * m, Eigen::SelfAdjointEigenSolver<Matrix3d>::RealVectorType * ret) {
  if (!NODEBUG) {
    printf("Matrix(0,0): %f\n", (*m)(0,0));
    printf("Matrix(1,1): %f\n", (*m)(1,1));
    printf("Matrix(2,2): %f\n", (*m)(2,2));
  }
  SelfAdjointEigenSolver<Matrix3d> es;
  es.computeDirect(*m);
  (*ret) = es.eigenvalues();
  return;
}

__global__ void kernel(Matrix3d * m, Eigen::SelfAdjointEigenSolver<Matrix3d>::RealVectorType * ret) {
  eigenValues(m, ret);
}

__global__ void kernelInverse(Matrix3d * in, Matrix3d * out) {
//  (*out) = in->inverse();
}

template<typename M1, typename M2, typename M3>
__global__ void kernelMultiply(M1 * J,
                               M2 * C,
                               M3 * result) {
//  Map<M3> res(result->data());
  if (!NODEBUG)
    printf("*** GPU IN ***\n");
  printIt(J);
  printIt(C);
//  res.noalias() = (*J) * (*C);
//  printIt(&res);
  (*result) = (*J) * (*C);
  if (!NODEBUG)
    printf("*** GPU OUT ***\n");
  return;
}

template<int row1, int col1, int row2, int col2>
void testMultiply() {
  std::cout << "TEST MULTIPLY" << std::endl;
  std::cout << "Product of type " << row1 << "x" << col1
    << " * " << row2 << "x" << col2 << std::endl;
  Eigen::Matrix<double, row1, col1> J;
  fillMatrix(J);
  Eigen::Matrix<double, row2, col2> C;
  fillMatrix(C);
  Eigen::Matrix<double, row1, col2> multiply_result = J * C;
  if (!NODEBUG) {
    std::cout << "Input J:" << std::endl; printIt(&J);
    std::cout << "Input C:" << std::endl; printIt(&C);
    std::cout << "Output:" << std::endl;
    printIt(&multiply_result);
  }
  // GPU
  Eigen::Matrix<double, row1, col1> *JGPU = nullptr;
  Eigen::Matrix<double, row2, col2> *CGPU = nullptr;
  Eigen::Matrix<double, row1, col2> *multiply_resultGPU = nullptr;
  Eigen::Matrix<double, row1, col2> *multiply_resultGPUret = new Eigen::Matrix<double, row1, col2>();

  cudaMalloc((void **)&JGPU, sizeof(Eigen::Matrix<double, row1, col1>));
  cudaMalloc((void **)&CGPU, sizeof(Eigen::Matrix<double, row2, col2>));
  cudaMalloc((void **)&multiply_resultGPU, sizeof(Eigen::Matrix<double, row1, col2>));
  cudaMemcpy(JGPU, &J, sizeof(Eigen::Matrix<double, row1, col1>), cudaMemcpyHostToDevice);
  cudaMemcpy(CGPU, &C, sizeof(Eigen::Matrix<double, row2, col2>), cudaMemcpyHostToDevice);
  cudaMemcpy(multiply_resultGPU, &multiply_result, sizeof(Eigen::Matrix<double, row1, col2>), cudaMemcpyHostToDevice);

  kernelMultiply<<<1,1>>>(JGPU, CGPU, multiply_resultGPU);
  cudaDeviceSynchronize();

  cudaMemcpy(multiply_resultGPUret, multiply_resultGPU, 
      sizeof(Eigen::Matrix<double, row1, col2>), cudaMemcpyDeviceToHost);
  printIt(multiply_resultGPUret);
  assert(isEqualFuzzy(multiply_result, (*multiply_resultGPUret)));
}

void testInverse() {
  std::cout << "TEST INVERSE" << std::endl;
  Matrix3d m = Matrix3d::Random();
  Matrix3d *mGPU = nullptr;
  Matrix3d *mGPUret = nullptr;
  Matrix3d *mCPUret = new Matrix3d();

  if (!NODEBUG) {
    std::cout << "Here is the matrix m:" << std::endl << m << std::endl;
    std::cout << "Its inverse is:" << std::endl << m.inverse() << std::endl;
  }
  cudaMalloc((void **)&mGPU, sizeof(Matrix3d));
  cudaMalloc((void **)&mGPUret, sizeof(Matrix3d));
  cudaMemcpy(mGPU, &m, sizeof(Matrix3d), cudaMemcpyHostToDevice);

  kernelInverse<<<1,1>>>(mGPU, mGPUret);
  cudaDeviceSynchronize();

  cudaMemcpy(mCPUret, mGPUret, sizeof(Matrix3d), cudaMemcpyDeviceToHost);
  if (!NODEBUG)
    std::cout << "Its GPU inverse is:" << std::endl << (*mCPUret) << std::endl;
}

void testEigenvalues() {
  std::cout << "TEST EIGENVALUES" << std::endl;
  Matrix3d m = Matrix3d::Random();
  Matrix3d mt = m.transpose();
  m += mt;
  Matrix3d * m_gpu = nullptr;
  Matrix3d * mgpudebug = new Matrix3d();
  Eigen::SelfAdjointEigenSolver<Matrix3d>::RealVectorType *ret = new Eigen::SelfAdjointEigenSolver<Matrix3d>::RealVectorType;
  Eigen::SelfAdjointEigenSolver<Matrix3d>::RealVectorType *ret1 = new Eigen::SelfAdjointEigenSolver<Matrix3d>::RealVectorType;
  Eigen::SelfAdjointEigenSolver<Matrix3d>::RealVectorType *ret_gpu = nullptr;
  eigenValues(&m, ret);
  if (!NODEBUG) {
    std::cout << "Generated Matrix M 3x3:\n" << m << std::endl;
    std::cout << "The eigenvalues of M are:" << std::endl << (*ret) << std::endl;
    std::cout << "*************************\n\n" << std::endl;
  }
  cudaMalloc((void **)&m_gpu, sizeof(Matrix3d));
  cudaMalloc((void **)&ret_gpu, sizeof(Eigen::SelfAdjointEigenSolver<Matrix3d>::RealVectorType));
  cudaMemcpy(m_gpu, &m, sizeof(Matrix3d), cudaMemcpyHostToDevice);

  kernel<<<1,1>>>(m_gpu, ret_gpu);
  cudaDeviceSynchronize();

  cudaMemcpy(mgpudebug, m_gpu, sizeof(Matrix3d), cudaMemcpyDeviceToHost);
  cudaMemcpy(ret1, ret_gpu, sizeof(Eigen::SelfAdjointEigenSolver<Matrix3d>::RealVectorType), cudaMemcpyDeviceToHost);
  if (!NODEBUG) {
    std::cout << "GPU Generated Matrix M 3x3:\n" << (*mgpudebug) << std::endl;
    std::cout << "GPU The eigenvalues of M are:" << std::endl << (*ret1) << std::endl;
    std::cout << "*************************\n\n" << std::endl;
  }
  assert(isEqualFuzzy(*ret, *ret1));
}


int main (int argc, char * argv[]) {

  testEigenvalues();
  testInverse();
  testMultiply<1, 2, 2, 1>();
  testMultiply<1, 2, 2, 2>();
  testMultiply<1, 2, 2, 3>();
  testMultiply<1, 2, 2, 4>();
  testMultiply<1, 2, 2, 5>();
  testMultiply<2, 1, 1, 2>();
  testMultiply<2, 1, 1, 3>();
  testMultiply<2, 1, 1, 4>();
  testMultiply<2, 1, 1, 5>();
  testMultiply<2, 2, 2, 2>();
  testMultiply<2, 3, 3, 1>();
  testMultiply<2, 3, 3, 2>();
  testMultiply<2, 3, 3, 4>();
  testMultiply<2, 3, 3, 5>();
  testMultiply<3, 2, 2, 3>();
  testMultiply<2, 3, 3, 3>(); // DOES NOT COMPILE W/O PATCHING EIGEN
  testMultiply<3, 3, 3, 3>();
  testMultiply<8, 8, 8, 8>();
  testMultiply<3, 4, 4, 3>();
  testMultiply<2, 4, 4, 2>();
  testMultiply<3, 4, 4, 2>(); // DOES NOT COMPILE W/O PATCHING EIGEN

  return 0;
}
