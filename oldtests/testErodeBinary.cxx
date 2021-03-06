#include <iomanip>
#include "itkCommand.h"
#include "itkSimpleFilterWatcher.h"
#include "itkChangeInformationImageFilter.h"
#include "ioutils.h"
#include <itkBinaryThresholdImageFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkBinaryErodeImageFilter.h>

#include "itkBinaryErodeParaImageFilter.h"

#include "itkTimeProbe.h"
#include "itkMultiThreader.h"

int main(int argc, char *argv[])
{
  if ( argc != 3 )
    {
    std::cerr << "Usage: " << argv[0] << " radius outimpref " << std::endl;
    return ( EXIT_FAILURE );
    }

  itk::MultiThreader::SetGlobalMaximumNumberOfThreads(1);
  constexpr int dim = 2;

  using PType = unsigned char;
  using IType = itk::Image< PType, dim >;
  using FType = itk::Image< float, dim >;

  // create an image to test the erosion shape:

  IType::Pointer    image = IType::New();
  IType::SizeType   size;
  IType::RegionType region;
  IType::IndexType  ind;
  size.Fill(256);
  ind.Fill(256 / 2);

  region.SetSize(size);
  image->SetRegions(region);
  image->Allocate();
  image->FillBuffer(1);
  image->SetPixel(ind, 0);

  using FilterType = itk::BinaryErodeParaImageFilter< IType, IType >;

  FilterType::Pointer filter = FilterType::New();
  int                 testrad = atoi(argv[1]);
  filter->SetInput(image);
  filter->SetUseImageSpacing(false);
  filter->SetRadius(testrad);
  //filter->SetCircular(false);

  writeIm< IType >(filter->GetOutput(), std::string(argv[2]) + "_" + argv[1] + ".png");

  // traditional erosion for comparison
  using SEType = itk::BinaryBallStructuringElement< PType, dim >;
  SEType             SE;
  SEType::RadiusType SErad;
  SErad.Fill(testrad);

  SE.SetRadius(SErad);
  SE.CreateStructuringElement();

  using OldBinErodeType = itk::BinaryErodeImageFilter< IType, IType, SEType >;
  OldBinErodeType::Pointer olderode = OldBinErodeType::New();
  olderode->SetInput(image);
  olderode->SetKernel(SE);
  olderode->SetErodeValue(1);

  writeIm< IType >(olderode->GetOutput(), std::string(argv[2]) + "_" + argv[1] + "_old.png");

  return EXIT_SUCCESS;
}
