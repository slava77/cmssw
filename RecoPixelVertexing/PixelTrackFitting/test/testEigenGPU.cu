#include "test_common.h"
#include <iostream>

#include "RecoPixelVertexing/PixelTrackFitting/interface/RiemannFit.h"
#include <Eigen/Core>
#include <Eigen/Eigenvalues>

using namespace Eigen;

__global__ void kernelFullFit(Rfit::Matrix3xNd * hits, Rfit::Matrix3Nd * hits_cov,
    double B, Rfit::circle_fit * circle_fit_resultsGPU, Rfit::line_fit * line_fit_resultsGPU) {
  Vector4d fast_fit = Rfit::Fast_fit(*hits);

  u_int n = hits->cols();
  Rfit::VectorNd rad = (hits->block(0, 0, 2, n).colwise().norm());

  (*circle_fit_resultsGPU) =
    Rfit::Circle_fit(hits->block(0,0,2,n), hits_cov->block(0, 0, 2 * n, 2 * n),
      fast_fit, rad, B, false, false);

  (*line_fit_resultsGPU) = Rfit::Line_fit(*hits, *hits_cov, *circle_fit_resultsGPU, fast_fit, true);

  return;
}

__global__ void kernelFastFit(Rfit::Matrix3xNd * hits, Vector4d * results) {
  (*results) = Rfit::Fast_fit(*hits);
}

__global__ void kernelCircleFit(Rfit::Matrix3xNd * hits,
    Rfit::Matrix3Nd * hits_cov, Vector4d * fast_fit_input, double B,
    Rfit::circle_fit * circle_fit_resultsGPU) {
  u_int n = hits->cols();
  Rfit::VectorNd rad = (hits->block(0, 0, 2, n).colwise().norm());

  if (!NODEBUG) {
    printf("fast_fit_input(0): %f\n", (*fast_fit_input)(0));
    printf("fast_fit_input(1): %f\n", (*fast_fit_input)(1));
    printf("fast_fit_input(2): %f\n", (*fast_fit_input)(2));
    printf("fast_fit_input(3): %f\n", (*fast_fit_input)(3));
    printf("rad(0,0): %f\n", rad(0,0));
    printf("rad(1,1): %f\n", rad(1,1));
    printf("rad(2,2): %f\n", rad(2,2));
    printf("hits_cov(0,0): %f\n", (*hits_cov)(0,0));
    printf("hits_cov(1,1): %f\n", (*hits_cov)(1,1));
    printf("hits_cov(2,2): %f\n", (*hits_cov)(2,2));
    printf("hits_cov(11,11): %f\n", (*hits_cov)(11,11));
    printf("B: %f\n", B);
  }
  (*circle_fit_resultsGPU) =
    Rfit::Circle_fit(hits->block(0,0,2,n), hits_cov->block(0, 0, 2 * n, 2 * n),
      *fast_fit_input, rad, B, false, false);
}

__global__ void kernelLineFit(Rfit::Matrix3xNd * hits,
                              Rfit::Matrix3Nd * hits_cov,
                              Rfit::circle_fit * circle_fit,
                              Vector4d * fast_fit,
                              Rfit::line_fit * line_fit) {
  (*line_fit) = Rfit::Line_fit(*hits, *hits_cov, *circle_fit, *fast_fit, true);
}

void fillHitsAndHitsCov(Rfit::Matrix3xNd & hits, Rfit::Matrix3Nd & hits_cov) {
  hits << 1.98645, 4.72598, 7.65632, 11.3151,
          2.18002, 4.88864, 7.75845, 11.3134,
          2.46338, 6.99838,  11.808,  17.793;
  hits_cov(0,0) = 7.14652e-06;
  hits_cov(1,1) = 2.15789e-06;
  hits_cov(2,2) = 1.63328e-06;
  hits_cov(3,3) = 6.27919e-06;
  hits_cov(4,4) = 6.10348e-06;
  hits_cov(5,5) = 2.08211e-06;
  hits_cov(6,6) = 1.61672e-06;
  hits_cov(7,7) = 6.28081e-06;
  hits_cov(8,8) = 5.184e-05;
  hits_cov(9,9) = 1.444e-05;
  hits_cov(10,10) = 6.25e-06;
  hits_cov(11,11) = 3.136e-05;
  hits_cov(0,4) = hits_cov(4,0) = -5.60077e-06;
  hits_cov(1,5) = hits_cov(5,1) = -1.11936e-06;
  hits_cov(2,6) = hits_cov(6,2) = -6.24945e-07;
  hits_cov(3,7) = hits_cov(7,3) = -5.28e-06;
}

void testFit() {
  constexpr double B = 0.0113921;
  Rfit::Matrix3xNd hits(3,4);
  Rfit::Matrix3Nd hits_cov = MatrixXd::Zero(12,12);
  Rfit::Matrix3xNd * hitsGPU = new Rfit::Matrix3xNd(3,4);
  Rfit::Matrix3Nd * hits_covGPU = nullptr;
  Vector4d * fast_fit_resultsGPU = new Vector4d();
  Vector4d * fast_fit_resultsGPUret = new Vector4d();
  Rfit::circle_fit * circle_fit_resultsGPU = new Rfit::circle_fit();
  Rfit::circle_fit * circle_fit_resultsGPUret = new Rfit::circle_fit();

  fillHitsAndHitsCov(hits, hits_cov);

  // FAST_FIT_CPU
  Vector4d fast_fit_results = Rfit::Fast_fit(hits);
  if (!NODEBUG) {
    std::cout << "Generated hits:\n" << hits << std::endl;
  }
  std::cout << "Fitted values (FastFit, [X0, Y0, R, tan(theta)]):\n" << fast_fit_results << std::endl;

  // FAST_FIT GPU
  cudaMalloc((void**)&hitsGPU, sizeof(Rfit::Matrix3xNd(3,4)));
  cudaMalloc((void**)&fast_fit_resultsGPU, sizeof(Vector4d));
  cudaMemcpy(hitsGPU, &hits, sizeof(Rfit::Matrix3xNd(3,4)), cudaMemcpyHostToDevice);

  kernelFastFit<<<1, 1>>>(hitsGPU, fast_fit_resultsGPU);
  cudaDeviceSynchronize();
  
  cudaMemcpy(fast_fit_resultsGPUret, fast_fit_resultsGPU, sizeof(Vector4d), cudaMemcpyDeviceToHost);
  std::cout << "Fitted values (FastFit, [X0, Y0, R, tan(theta)]): GPU\n" << *fast_fit_resultsGPUret << std::endl;
  assert(isEqualFuzzy(fast_fit_results, (*fast_fit_resultsGPUret)));

  // CIRCLE_FIT CPU
  u_int n = hits.cols();
  Rfit::VectorNd rad = (hits.block(0, 0, 2, n).colwise().norm());

  Rfit::circle_fit circle_fit_results = Rfit::Circle_fit(hits.block(0, 0, 2, n),
      hits_cov.block(0, 0, 2 * n, 2 * n),
      fast_fit_results, rad, B, false, false);
  std::cout << "Fitted values (CircleFit):\n" << circle_fit_results.par << std::endl;

  // CIRCLE_FIT GPU
  cudaMalloc((void **)&hits_covGPU, sizeof(Rfit::Matrix3Nd(12,12)));
  cudaMalloc((void **)&circle_fit_resultsGPU, sizeof(Rfit::circle_fit));
  cudaMemcpy(hits_covGPU, &hits_cov, sizeof(Rfit::Matrix3Nd(12,12)), cudaMemcpyHostToDevice);

  kernelCircleFit<<<1,1>>>(hitsGPU, hits_covGPU,
      fast_fit_resultsGPU, B, circle_fit_resultsGPU);
  cudaDeviceSynchronize();

  cudaMemcpy(circle_fit_resultsGPUret, circle_fit_resultsGPU,
      sizeof(Rfit::circle_fit), cudaMemcpyDeviceToHost);
  std::cout << "Fitted values (CircleFit) GPU:\n" << circle_fit_resultsGPUret->par << std::endl;
  assert(isEqualFuzzy(circle_fit_results.par, circle_fit_resultsGPUret->par));

  // LINE_FIT CPU
  Rfit::line_fit line_fit_results = Rfit::Line_fit(hits, hits_cov, circle_fit_results, fast_fit_results, true);
  std::cout << "Fitted values (LineFit):\n" << line_fit_results.par << std::endl;

  // LINE_FIT GPU
  Rfit::line_fit * line_fit_resultsGPU = nullptr;
  Rfit::line_fit * line_fit_resultsGPUret = new Rfit::line_fit();

  cudaMalloc((void **)&line_fit_resultsGPU, sizeof(Rfit::line_fit));

  kernelLineFit<<<1,1>>>(hitsGPU, hits_covGPU, circle_fit_resultsGPU, fast_fit_resultsGPU, line_fit_resultsGPU);
  cudaDeviceSynchronize();

  cudaMemcpy(line_fit_resultsGPUret, line_fit_resultsGPU, sizeof(Rfit::line_fit), cudaMemcpyDeviceToHost);
  std::cout << "Fitted values (LineFit) GPU:\n" << line_fit_resultsGPUret->par << std::endl;
  assert(isEqualFuzzy(line_fit_results.par, line_fit_resultsGPUret->par));
}

void testFitOneGo() {
  constexpr double B = 0.0113921;
  Rfit::Matrix3xNd hits(3,4);
  Rfit::Matrix3Nd hits_cov = MatrixXd::Zero(12,12);

  fillHitsAndHitsCov(hits, hits_cov);

  // FAST_FIT_CPU
  Vector4d fast_fit_results = Rfit::Fast_fit(hits);
  // CIRCLE_FIT CPU
  u_int n = hits.cols();
  Rfit::VectorNd rad = (hits.block(0, 0, 2, n).colwise().norm());

  Rfit::circle_fit circle_fit_results = Rfit::Circle_fit(hits.block(0, 0, 2, n), 
      hits_cov.block(0, 0, 2 * n, 2 * n),
      fast_fit_results, rad, B, false, false);
  // LINE_FIT CPU
  Rfit::line_fit line_fit_results = Rfit::Line_fit(hits, hits_cov, circle_fit_results, fast_fit_results, true);

  // FIT GPU
  Rfit::Matrix3xNd * hitsGPU = new Rfit::Matrix3xNd(3,4);
  Rfit::Matrix3Nd * hits_covGPU = nullptr;
  Rfit::line_fit * line_fit_resultsGPU = nullptr;
  Rfit::line_fit * line_fit_resultsGPUret = new Rfit::line_fit();
  Rfit::circle_fit * circle_fit_resultsGPU = new Rfit::circle_fit();
  Rfit::circle_fit * circle_fit_resultsGPUret = new Rfit::circle_fit();

  cudaMalloc((void**)&hitsGPU, sizeof(Rfit::Matrix3xNd(3,4)));
  cudaMalloc((void **)&hits_covGPU, sizeof(Rfit::Matrix3Nd(12,12)));
  cudaMalloc((void **)&line_fit_resultsGPU, sizeof(Rfit::line_fit));
  cudaMalloc((void **)&circle_fit_resultsGPU, sizeof(Rfit::circle_fit));
  cudaMemcpy(hitsGPU, &hits, sizeof(Rfit::Matrix3xNd(3,4)), cudaMemcpyHostToDevice);
  cudaMemcpy(hits_covGPU, &hits_cov, sizeof(Rfit::Matrix3Nd(12,12)), cudaMemcpyHostToDevice);

  kernelFullFit<<<1, 1>>>(hitsGPU, hits_covGPU, B, circle_fit_resultsGPU, line_fit_resultsGPU);
  cudaDeviceSynchronize();

  cudaMemcpy(circle_fit_resultsGPUret, circle_fit_resultsGPU,
      sizeof(Rfit::circle_fit), cudaMemcpyDeviceToHost);
  cudaMemcpy(line_fit_resultsGPUret, line_fit_resultsGPU, sizeof(Rfit::line_fit), cudaMemcpyDeviceToHost);

  std::cout << "Fitted values (CircleFit) GPU:\n" << circle_fit_resultsGPUret->par << std::endl;
  std::cout << "Fitted values (LineFit): GPU\n" << line_fit_resultsGPUret->par << std::endl;
  assert(isEqualFuzzy(circle_fit_results.par, circle_fit_resultsGPUret->par));
  assert(isEqualFuzzy(line_fit_results.par, line_fit_resultsGPUret->par));
}

int main (int argc, char * argv[]) {
  testFit();
  testFitOneGo();
  return 0;
}
