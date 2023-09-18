#include "CUDA_Test.h"

ACUDA_Test::ACUDA_Test()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ACUDA_Test::BeginPlay()
{
	Super::BeginPlay();
	
	//SimpleCUDATest();
}

void ACUDA_Test::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

