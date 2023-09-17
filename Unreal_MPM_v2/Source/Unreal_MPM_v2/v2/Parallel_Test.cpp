#include "Parallel_Test.h"

AParallel_Test::AParallel_Test()
{
	PrimaryActorTick.bCanEverTick = true;

	InstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedStaticMesh"));

	SetRootComponent(InstancedStaticMeshComponent);

	InstancedStaticMeshComponent->SetMobility(EComponentMobility::Static);
	InstancedStaticMeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	InstancedStaticMeshComponent->SetGenerateOverlapEvents(false);
}

void AParallel_Test::BeginPlay()
{
	Super::BeginPlay();
	
}
 
void AParallel_Test::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AParallel_Test::RunTask()
{
	(new FAutoDeleteAsyncTask<MyMultiThreading>(NumParticles))->StartBackgroundTask();
}

void AParallel_Test::RunTaskOnMain()
{
	MyMultiThreading* task = new MyMultiThreading(10);

	task->DoWorkMain();
	delete task;
}

MyMultiThreading::MyMultiThreading(int32 _Count)
{
	m_iIndex = _Count;
}

MyMultiThreading::~MyMultiThreading()
{
	UE_LOG(LogTemp, Warning, TEXT("Task Finished"));
}

void MyMultiThreading::DoWork()
{
	//do something

}

void MyMultiThreading::DoWorkMain()
{
	DoWork();
}
