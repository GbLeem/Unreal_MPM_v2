#pragma once

#include "Components/InstancedStaticMeshComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Parallel_Test.generated.h"

UCLASS()
class UNREAL_MPM_V2_API AParallel_Test : public AActor
{
	GENERATED_BODY()
	
public:	
	AParallel_Test();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void RunTask();
	void RunTaskOnMain();

public:
	UPROPERTY(VisibleAnywhere)
	UInstancedStaticMeshComponent* InstancedStaticMeshComponent;

	int NumParticles = 0;

	const int grid_res = 64;
	const int NumCells = grid_res * grid_res * grid_res;
};

//=========================================

class MyMultiThreading : public FNonAbandonableTask
{
public:
	MyMultiThreading(int32 _Count);
	~MyMultiThreading();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(MyMultiThreading, STATGROUP_ThreadPoolAsyncTasks);
	}
	void DoWork();
	void DoWorkMain();

	int32 m_iIndex;
};